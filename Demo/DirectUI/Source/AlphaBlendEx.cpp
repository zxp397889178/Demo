#include "stdafx.h"
#include "AlphaBlendEx.h"
#include <emmintrin.h>

////////////////////////////////////////////////////////////////////
typedef int int32_t;
typedef unsigned uint32_t;
typedef unsigned char uint8_t;
typedef float   SkScalar;
typedef int32_t SkFixed;
typedef unsigned short uint16_t;
typedef uint32_t SkPMColor;
typedef unsigned U8CPU;


#ifdef _MSC_VER
static inline void getcpuid(int info_type, int info[4]) {
#if defined(_WIN64)
	__cpuid(info, info_type);
#else
	__asm {
		mov    eax, [info_type]
		cpuid
			mov    edi, [info]
		mov    [edi], eax
			mov    [edi+4], ebx
			mov    [edi+8], ecx
			mov    [edi+12], edx
	}
#endif
}
#else
#if defined(__x86_64__)
static inline void getcpuid(int info_type, int info[4]) {
	asm volatile (
		"cpuid \n\t"
		: "=a"(info[0]), "=b"(info[1]), "=c"(info[2]), "=d"(info[3])
		: "a"(info_type)
		);
}
#else
static inline void getcpuid(int info_type, int info[4]) {
	// We save and restore ebx, so this code can be compatible with -fPIC
	asm volatile (
		"pushl %%ebx      \n\t"
		"cpuid            \n\t"
		"movl %%ebx, %1   \n\t"
		"popl %%ebx       \n\t"
		: "=a"(info[0]), "=r"(info[1]), "=c"(info[2]), "=d"(info[3])
		: "a"(info_type)
		);
}
#endif
#endif

#if defined(__x86_64__) || defined(_WIN64)
/* All x86_64 machines have SSE2, so don't even bother checking. */
static inline bool hasSSE2() {
	return true;
}
#else

static inline bool hasSSE2() {
	int cpu_info[4] = { 0 };
	getcpuid(1, cpu_info);
	return (cpu_info[3] & (1<<26)) != 0;
}
#endif


static bool cachedHasSSE2() {
	static bool gHasSSE2 = hasSSE2();
	return gHasSSE2;
}

///////////////////////////////////////////
////////////////////////////////////////////////
#define SkIntToScalar(n)        ((float)(n))
struct  SkRect {
	SkScalar    fLeft, fTop, fRight, fBottom;
	SkScalar    width() const { return fRight - fLeft; }
	SkScalar    height() const { return fBottom - fTop; }
	bool isEmpty() const { return fLeft >= fRight || fTop >= fBottom; }
};


////////////////////////////////////////////////
#define SK_RESTRICT __restrict
const uint32_t gMask_00FF00FF = 0xFF00FF;
#define SkASSERT(cond)
#define SK_A32_SHIFT 24
#define SkGetPackedA32(packed)      ((uint32_t)((packed) << (24 - SK_A32_SHIFT)) >> 24)

static inline uint32_t SkAlphaMulQ(uint32_t c, unsigned scale) {
	uint32_t mask = gMask_00FF00FF;
	//    uint32_t mask = 0xFF00FF;

	uint32_t rb = ((c & mask) * scale) >> 8;
	uint32_t ag = ((c >> 8) & mask) * scale;
	return (rb & mask) | (ag & ~mask);
}
static inline unsigned SkAlpha255To256(U8CPU alpha) {
	SkASSERT(SkToU8(alpha) == alpha);
	// this one assues that blending on top of an opaque dst keeps it that way
	// even though it is less accurate than a+(a>>7) for non-opaque dsts
	return alpha + 1;
}


static inline SkPMColor SkPMSrcOver(SkPMColor src, SkPMColor dst) {
	return src + SkAlphaMulQ(dst, SkAlpha255To256(255 - SkGetPackedA32(src)));
}


#define UNROLL
static void S32A_Opaque_BlitRow32(SkPMColor* SK_RESTRICT dst,
								  const SkPMColor* SK_RESTRICT src,
								  int count, U8CPU alpha) {
  SkASSERT(255 == alpha);
  if (count > 0) {
#ifdef UNROLL
	  if (count & 1) {
		  *dst = SkPMSrcOver(*(src++), *dst);
		  dst += 1;
		  count -= 1;
	  }

	  const SkPMColor* SK_RESTRICT srcEnd = src + count;
	  while (src != srcEnd) {
		  *dst = SkPMSrcOver(*(src++), *dst);
		  dst += 1;
		  *dst = SkPMSrcOver(*(src++), *dst);
		  dst += 1;
	  }
#else
	  do {
#ifdef TEST_SRC_ALPHA
		  SkPMColor sc = *src;
		  if (sc) {
			  unsigned srcA = SkGetPackedA32(sc);
			  SkPMColor result = sc;
			  if (srcA != 255) {
				  result = SkPMSrcOver(sc, *dst);
			  }
			  *dst = result;
		  }
#else
		  *dst = SkPMSrcOver(*src, *dst);
#endif
		  src += 1;
		  dst += 1;
	  } while (--count > 0);
#endif
  }
}

void S32A_Opaque_BlitRow32_SSE2(SkPMColor* SK_RESTRICT dst,
								   const SkPMColor* SK_RESTRICT src,
								   int count, U8CPU alphaU8) {
   SkASSERT(alphaU8 == 255);
   if (count <= 0) {
	   return;
   }

   if (count >= 4) {
	   SkASSERT(((size_t)dst & 0x03) == 0);
	   while (((size_t)dst & 0x0F) != 0) {
		   *dst = SkPMSrcOver(*src, *dst);
		   src++;
		   dst++;
		   count--;
	   }

	   const __m128i *s = reinterpret_cast<const __m128i*>(src);
	   __m128i *d = reinterpret_cast<__m128i*>(dst);
#ifdef SK_USE_ACCURATE_BLENDING
	   __m128i rb_mask = _mm_set1_epi32(0x00FF00FF);
	   __m128i c_128 = _mm_set1_epi16(128);  // 8 copies of 128 (16-bit)
	   __m128i c_255 = _mm_set1_epi16(255);  // 8 copies of 255 (16-bit)
	   while (count >= 4) {
		   // Load 4 pixels
		   __m128i src_pixel = _mm_loadu_si128(s);
		   __m128i dst_pixel = _mm_load_si128(d);

		   __m128i dst_rb = _mm_and_si128(rb_mask, dst_pixel);
		   __m128i dst_ag = _mm_srli_epi16(dst_pixel, 8);
		   // Shift alphas down to lower 8 bits of each quad.
		   __m128i alpha = _mm_srli_epi32(src_pixel, 24);

		   // Copy alpha to upper 3rd byte of each quad
		   alpha = _mm_or_si128(alpha, _mm_slli_epi32(alpha, 16));

		   // Subtract alphas from 255, to get 0..255
		   alpha = _mm_sub_epi16(c_255, alpha);

		   // Multiply by red and blue by src alpha.
		   dst_rb = _mm_mullo_epi16(dst_rb, alpha);
		   // Multiply by alpha and green by src alpha.
		   dst_ag = _mm_mullo_epi16(dst_ag, alpha);

		   // dst_rb_low = (dst_rb >> 8)
		   __m128i dst_rb_low = _mm_srli_epi16(dst_rb, 8);
		   __m128i dst_ag_low = _mm_srli_epi16(dst_ag, 8);

		   // dst_rb = (dst_rb + dst_rb_low + 128) >> 8
		   dst_rb = _mm_add_epi16(dst_rb, dst_rb_low);
		   dst_rb = _mm_add_epi16(dst_rb, c_128);
		   dst_rb = _mm_srli_epi16(dst_rb, 8);

		   // dst_ag = (dst_ag + dst_ag_low + 128) & ag_mask
		   dst_ag = _mm_add_epi16(dst_ag, dst_ag_low);
		   dst_ag = _mm_add_epi16(dst_ag, c_128);
		   dst_ag = _mm_andnot_si128(rb_mask, dst_ag);

		   // Combine back into RGBA.
		   dst_pixel = _mm_or_si128(dst_rb, dst_ag);

		   // Add result
		   __m128i result = _mm_add_epi8(src_pixel, dst_pixel);
		   _mm_store_si128(d, result);
		   s++;
		   d++;
		   count -= 4;
	   }
#else
	   __m128i rb_mask = _mm_set1_epi32(0x00FF00FF);
	   __m128i c_256 = _mm_set1_epi16(0x0100);  // 8 copies of 256 (16-bit)
	   while (count >= 4) {
		   // Load 4 pixels
		   __m128i src_pixel = _mm_loadu_si128(s);
		   __m128i dst_pixel = _mm_load_si128(d);

		   __m128i dst_rb = _mm_and_si128(rb_mask, dst_pixel);
		   __m128i dst_ag = _mm_srli_epi16(dst_pixel, 8);

		   // (a0, g0, a1, g1, a2, g2, a3, g3)  (low byte of each word)
		   __m128i alpha = _mm_srli_epi16(src_pixel, 8);

		   // (a0, a0, a1, a1, a2, g2, a3, g3)
		   alpha = _mm_shufflehi_epi16(alpha, 0xF5);

		   // (a0, a0, a1, a1, a2, a2, a3, a3)
		   alpha = _mm_shufflelo_epi16(alpha, 0xF5);

		   // Subtract alphas from 256, to get 1..256
		   alpha = _mm_sub_epi16(c_256, alpha);

		   // Multiply by red and blue by src alpha.
		   dst_rb = _mm_mullo_epi16(dst_rb, alpha);
		   // Multiply by alpha and green by src alpha.
		   dst_ag = _mm_mullo_epi16(dst_ag, alpha);

		   // Divide by 256.
		   dst_rb = _mm_srli_epi16(dst_rb, 8);

		   // Mask out high bits (already in the right place)
		   dst_ag = _mm_andnot_si128(rb_mask, dst_ag);

		   // Combine back into RGBA.
		   dst_pixel = _mm_or_si128(dst_rb, dst_ag);

		   // Add result
		   __m128i result = _mm_add_epi8(src_pixel, dst_pixel);
		   _mm_store_si128(d, result);
		   s++;
		   d++;
		   count -= 4;
	   }
#endif
	   src = reinterpret_cast<const SkPMColor*>(s);
	   dst = reinterpret_cast<SkPMColor*>(d);
   }

   while (count > 0) {
	   *dst = SkPMSrcOver(*src, *dst);
	   src++;
	   dst++;
	   count--;
   }
}

/////////////////////////////////////////////////////////////////////////////////////
class  SkMatrix {
public:
	enum {
		kMScaleX,
		kMSkewX,
		kMTransX,
		kMSkewY,
		kMScaleY,
		kMTransY,
		kMPersp0,
		kMPersp1,
		kMPersp2
	};
	bool setRectToRect(const SkRect& src, const SkRect& dst);
	void reset();
	static void ScaleTrans_xy(const SkMatrix&, SkScalar, SkScalar, SkScalar&, SkScalar&);
	bool invert(SkMatrix* inverse);
	SkScalar         fMat[9];

};
///////////////////////////////////////////////////////////////////////////////
#define SK_Scalar1   (1.0f)
#define kMatrix22Elem   SK_Scalar1
#define SkScalarDiv(a, b)       ((float)(a) / (b))
#define SkScalarMul(a, b)       ((float)(a) * (b))
static inline void sk_bzero(void* buffer, size_t size) {
	memset(buffer, 0, size);
}

#define sk_float_abs(x)         fabsf(x)
#define SkScalarAbs(x)          sk_float_abs(x)
#define SK_ScalarNearlyZero         (SK_Scalar1 / (1 << 12))

static inline bool SkScalarNearlyZero(SkScalar x,
									  SkScalar tolerance = SK_ScalarNearlyZero) {
	  SkASSERT(tolerance >= 0);
	  return SkScalarAbs(x) <= tolerance;
}

typedef double SkDetScalar;
#define SkPerspMul(a, b)            SkScalarMul(a, b)
#define SkScalarMulShift(a, b, s)   SkDoubleToFloat((a) * (b))
static double sk_inv_determinant(const float mat[9], int isPerspective,
								 int* /* (only used in Fixed case) */) {
									 double det;

									 if (isPerspective) {
										 det =   mat[SkMatrix::kMScaleX] * ((double)mat[SkMatrix::kMScaleY] * mat[SkMatrix::kMPersp2] - (double)mat[SkMatrix::kMTransY] * mat[SkMatrix::kMPersp1]) +
											 mat[SkMatrix::kMSkewX] * ((double)mat[SkMatrix::kMTransY] * mat[SkMatrix::kMPersp0] - (double)mat[SkMatrix::kMSkewY] * mat[SkMatrix::kMPersp2]) +
											 mat[SkMatrix::kMTransX] * ((double)mat[SkMatrix::kMSkewY] * mat[SkMatrix::kMPersp1] - (double)mat[SkMatrix::kMScaleY] * mat[SkMatrix::kMPersp0]);
									 } else {
										 det =   (double)mat[SkMatrix::kMScaleX] * mat[SkMatrix::kMScaleY] - (double)mat[SkMatrix::kMSkewX] * mat[SkMatrix::kMSkewY];
									 }

									 // Since the determinant is on the order of the cube of the matrix members,
									 // compare to the cube of the default nearly-zero constant (although an
									 // estimate of the condition number would be better if it wasn't so expensive).
									 if (SkScalarNearlyZero((float)det, SK_ScalarNearlyZero * SK_ScalarNearlyZero * SK_ScalarNearlyZero)) {
										 return 0;
									 }
									 return 1.0 / det;
}

bool SkMatrix::setRectToRect(const SkRect& src, const SkRect& dst)
{
	if (src.isEmpty()) {
		this->reset();
		return false;
	}

	if (dst.isEmpty()) {
		sk_bzero(fMat, 8 * sizeof(SkScalar));
	/*	this->setTypeMask(kScale_Mask | kRectStaysRect_Mask);*/
	} else {
		SkScalar    tx, sx = SkScalarDiv(dst.width(), src.width());
		SkScalar    ty, sy = SkScalarDiv(dst.height(), src.height());
		//bool        xLarger = false;

		//if (align != kFill_ScaleToFit) {
		//	if (sx > sy) {
		//		xLarger = true;
		//		sx = sy;
		//	} else {
		//		sy = sx;
		//	}
		//}

		tx = dst.fLeft - SkScalarMul(src.fLeft, sx);
		ty = dst.fTop - SkScalarMul(src.fTop, sy);
		/*if (align == kCenter_ScaleToFit || align == kEnd_ScaleToFit) {
			SkScalar diff;

			if (xLarger) {
				diff = dst.width() - SkScalarMul(src.width(), sy);
			} else {
				diff = dst.height() - SkScalarMul(src.height(), sy);
			}

			if (align == kCenter_ScaleToFit) {
				diff = SkScalarHalf(diff);
			}

			if (xLarger) {
				tx += diff;
			} else {
				ty += diff;
			}
		}*/

		fMat[kMScaleX] = sx;
		fMat[kMScaleY] = sy;
		fMat[kMTransX] = tx;
		fMat[kMTransY] = ty;
		fMat[kMSkewX]  = fMat[kMSkewY] = 
		fMat[kMPersp0] = fMat[kMPersp1] = 0;

		/*this->setTypeMask(kScale_Mask | kTranslate_Mask | kRectStaysRect_Mask);*/
	}
	// shared cleanup
	fMat[kMPersp2] = kMatrix22Elem;
	return true;

}
void  SkMatrix::reset() {
	fMat[kMScaleX] = fMat[kMScaleY] = SK_Scalar1;
	fMat[kMSkewX]  = fMat[kMSkewY] = 
		fMat[kMTransX] = fMat[kMTransY] =
		fMat[kMPersp0] = fMat[kMPersp1] = 0;
	fMat[kMPersp2] = kMatrix22Elem;

}

 #define SkScalarMulAdd(a, b, c) ((float)(a) * (b) + (c))
void SkMatrix::ScaleTrans_xy(const SkMatrix& m, SkScalar sx, SkScalar sy,
							SkScalar& fx, SkScalar& fy) {
	 fx = SkScalarMulAdd(sx, m.fMat[kMScaleX], m.fMat[kMTransX]);
	 fy = SkScalarMulAdd(sy, m.fMat[kMScaleY], m.fMat[kMTransY]);
}

static inline float SkDoubleToFloat(double x) {
	return static_cast<float>(x);
}
static float inline mul_diff_scale(double a, double b, double c, double d,
								   double scale) {
	return SkDoubleToFloat((a * b - c * d) * scale);
}
bool SkMatrix::invert(SkMatrix* inv)
{
	int         isPersp = 0;
	int         shift;
	SkDetScalar scale = sk_inv_determinant(fMat, isPersp, &shift);

	if (fabs(scale) < 0.000001) { // underflow
		return false;
	}

	if (inv) {
		SkMatrix tmp;
		if (inv == this) {
			inv = &tmp;
		}
		if (isPersp) {
			shift = 61 - shift;
			inv->fMat[kMScaleX] = SkScalarMulShift(SkPerspMul(fMat[kMScaleY], fMat[kMPersp2]) - SkPerspMul(fMat[kMTransY], fMat[kMPersp1]), scale, shift);
			inv->fMat[kMSkewX]  = SkScalarMulShift(SkPerspMul(fMat[kMTransX], fMat[kMPersp1]) - SkPerspMul(fMat[kMSkewX],  fMat[kMPersp2]), scale, shift);
			inv->fMat[kMTransX] = SkScalarMulShift(SkScalarMul(fMat[kMSkewX], fMat[kMTransY]) - SkScalarMul(fMat[kMTransX], fMat[kMScaleY]), scale, shift);

			inv->fMat[kMSkewY]  = SkScalarMulShift(SkPerspMul(fMat[kMTransY], fMat[kMPersp0]) - SkPerspMul(fMat[kMSkewY],   fMat[kMPersp2]), scale, shift);
			inv->fMat[kMScaleY] = SkScalarMulShift(SkPerspMul(fMat[kMScaleX], fMat[kMPersp2]) - SkPerspMul(fMat[kMTransX],  fMat[kMPersp0]), scale, shift);
			inv->fMat[kMTransY] = SkScalarMulShift(SkScalarMul(fMat[kMTransX], fMat[kMSkewY]) - SkScalarMul(fMat[kMScaleX], fMat[kMTransY]), scale, shift);

			inv->fMat[kMPersp0] = SkScalarMulShift(SkScalarMul(fMat[kMSkewY], fMat[kMPersp1]) - SkScalarMul(fMat[kMScaleY], fMat[kMPersp0]), scale, shift);             
			inv->fMat[kMPersp1] = SkScalarMulShift(SkScalarMul(fMat[kMSkewX], fMat[kMPersp0]) - SkScalarMul(fMat[kMScaleX], fMat[kMPersp1]), scale, shift);
			inv->fMat[kMPersp2] = SkScalarMulShift(SkScalarMul(fMat[kMScaleX], fMat[kMScaleY]) - SkScalarMul(fMat[kMSkewX], fMat[kMSkewY]), scale, shift);
#ifdef SK_SCALAR_IS_FIXED
			if (SkAbs32(inv->fMat[kMPersp2]) > SK_Fixed1) {
				Sk64    tmp;

				tmp.set(SK_Fract1);
				tmp.shiftLeft(16);
				tmp.div(inv->fMat[kMPersp2], Sk64::kRound_DivOption);

				SkFract scale = tmp.get32();

				for (int i = 0; i < 9; i++) {
					inv->fMat[i] = SkFractMul(inv->fMat[i], scale);
				}
			}
			inv->fMat[kMPersp2] = SkFixedToFract(inv->fMat[kMPersp2]);
#endif
		} else {   // not perspective
#ifdef SK_SCALAR_IS_FIXED
			Sk64    tx, ty;
			int     clzNumer;

			// check the 2x2 for overflow
			{
				int32_t value = SkAbs32(fMat[kMScaleY]);
				value |= SkAbs32(fMat[kMSkewX]);
				value |= SkAbs32(fMat[kMScaleX]);
				value |= SkAbs32(fMat[kMSkewY]);
				clzNumer = SkCLZ(value);
				if (shift - clzNumer > 31)
					return false;   // overflow
			}

			set_muladdmul(&tx, fMat[kMSkewX], fMat[kMTransY], -fMat[kMScaleY], fMat[kMTransX]);
			set_muladdmul(&ty, fMat[kMSkewY], fMat[kMTransX], -fMat[kMScaleX], fMat[kMTransY]);
			// check tx,ty for overflow
			clzNumer = SkCLZ(SkAbs32(tx.fHi) | SkAbs32(ty.fHi));
			if (shift - clzNumer > 14) {
				return false;   // overflow
			}

			int fixedShift = 61 - shift;
			int sk64shift = 44 - shift + clzNumer;

			inv->fMat[kMScaleX] = SkMulShift(fMat[kMScaleY], scale, fixedShift);
			inv->fMat[kMSkewX]  = SkMulShift(-fMat[kMSkewX], scale, fixedShift);
			inv->fMat[kMTransX] = SkMulShift(tx.getShiftRight(33 - clzNumer), scale, sk64shift);

			inv->fMat[kMSkewY]  = SkMulShift(-fMat[kMSkewY], scale, fixedShift);
			inv->fMat[kMScaleY] = SkMulShift(fMat[kMScaleX], scale, fixedShift);
			inv->fMat[kMTransY] = SkMulShift(ty.getShiftRight(33 - clzNumer), scale, sk64shift);
#else
			inv->fMat[kMScaleX] = SkDoubleToFloat(fMat[kMScaleY] * scale);
			inv->fMat[kMSkewX] = SkDoubleToFloat(-fMat[kMSkewX] * scale);
			inv->fMat[kMTransX] = mul_diff_scale(fMat[kMSkewX], fMat[kMTransY],
				fMat[kMScaleY], fMat[kMTransX], scale);

			inv->fMat[kMSkewY] = SkDoubleToFloat(-fMat[kMSkewY] * scale);
			inv->fMat[kMScaleY] = SkDoubleToFloat(fMat[kMScaleX] * scale);
			inv->fMat[kMTransY] = mul_diff_scale(fMat[kMSkewY], fMat[kMTransX],
				fMat[kMScaleX], fMat[kMTransY], scale);
#endif
			inv->fMat[kMPersp0] = 0;
			inv->fMat[kMPersp1] = 0;
			inv->fMat[kMPersp2] = kMatrix22Elem;
		}

		if (inv == &tmp) {
			*(this) = tmp;
		}
	}
	return true;
}


class SkBitmap {
public:
	SkBitmap();
	~SkBitmap();
	void SetPixels(void* pixels,  unsigned int   width, unsigned int   height,  size_t   pixelRefOffset = 0,   unsigned int   rowBytes = 0);
public:
	// 必填参数
	mutable void*   fPixels;
	unsigned int    fWidth;
	unsigned int    fHeight;
	bool fRef;
	// 不允许更改
	unsigned int    fRowBytes;
	mutable size_t  fPixelRefOffset;
};

///////////////////////////////////////////////////
SkBitmap::SkBitmap()
{
	fPixelRefOffset = 0;
	fPixels = NULL;
	fRowBytes = 0;
	fWidth = 0;
	fHeight = 0;
	fRef = true;

}
SkBitmap::~SkBitmap()
{
	if (!fRef)
	{
		delete []((BYTE*)fPixels);
		fPixels = NULL;
	}

}
void SkBitmap::SetPixels(void* pixels,  unsigned int   width, unsigned int   height,  size_t   pixelRefOffset,   unsigned int   rowBytes)
{
	if (rowBytes == 0)
		fRowBytes =  width * 4;
	else
		fRowBytes = rowBytes;
	fWidth = width;
	fHeight = height;
	fPixels =pixels;
	fPixelRefOffset = pixelRefOffset;
}

//////////////////////////////////////////////////////////
typedef void (*sk_memset32_Proc)(uint32_t dst[], uint32_t value, int count);
typedef void (*ClampX_ClampY_nofilter_scale_Proc)(SkMatrix* matrix, SkBitmap*fBitmap, uint32_t xy[], int count, int x, int y);
typedef void (*S32A_Opaque_BlitRow32_Proc)(uint32_t* dst,const SkPMColor* src, int count, U8CPU alpha);

sk_memset32_Proc sk_memset32_proc = NULL;
ClampX_ClampY_nofilter_scale_Proc ClampX_ClampY_nofilter_scale_proc = NULL;
S32A_Opaque_BlitRow32_Proc S32A_Opaque_BlitRow32_proc = NULL;

/////////////////////////////////////////////////////////////
#define SK_Fixed1           (1 << 16)
#define SkFloatToFixed(x)   ((SkFixed)((x) * SK_Fixed1))
#define SkScalarToFixed(x)      SkFloatToFixed(x)
#define SK_ScalarHalf           (0.5f)

static inline int SkClampMax(int value, int max) {
	// ensure that max is positive
	SkASSERT(max >= 0);
	if (value < 0) {
		value = 0;
	}
	if (value > max) {
		value = max;
	}
	return value;
}


static inline uint32_t ClampX_ClampY_pack_filter(SkFixed f, unsigned max,
												 SkFixed one) {
	 unsigned i = SkClampMax(f >> 16, max);
	 i = (i << 4) | ((f >> 12) & 0xF);
	 return (i << 14) | SkClampMax((f + one) >> 16, max);
}


//void ClampX_ClampY_filter_scale_SSE2(SkMatrix* matrix, SkBitmap*fBitmap, uint32_t xy[],
//									 int count, int x, int y) {
//
//	 const unsigned maxX = fBitmap->fWidth - 1;
//	 const SkFixed one = (1 << 16);
//	 const SkFixed dx = SkScalarToFixed(matrix->fMat[SkMatrix::kMScaleX]);
//	 SkFixed fx;
//	 SkFixed  fFilterOneY = SK_Fixed1;
//
//	 SkScalar ptfx, ptfy;//SkPoint pt; 
//
//	 SkMatrix::ScaleTrans_xy(*matrix, SkIntToScalar(x) + SK_ScalarHalf,
//		 SkIntToScalar(y) + SK_ScalarHalf, ptfx, ptfy);
//	 const SkFixed fy = SkScalarToFixed(ptfy) - (fFilterOneY >> 1);
//	 const unsigned maxY = fBitmap->fHeight - 1;
//	 // compute our two Y values up front
//	 *xy++ = ClampX_ClampY_pack_filter(fy, maxY, fFilterOneY);
//	 // now initialize fx
//	 fx = SkScalarToFixed(ptfx) - (one >> 1);
//
//	 // test if we don't need to apply the tile proc
//	 if (dx > 0 && (unsigned)(fx >> 16) <= maxX &&
//		 (unsigned)((fx + dx * (count - 1)) >> 16) < maxX) {
//			 if (count >= 4) {
//				 // SSE version of decal_filter_scale
//				 while ((size_t(xy) & 0x0F) != 0) {
//					 SkASSERT((fx >> (16 + 14)) == 0);
//					 *xy++ = (fx >> 12 << 14) | ((fx >> 16) + 1);
//					 fx += dx;
//					 count--;
//				 }
//
//				 __m128i wide_1    = _mm_set1_epi32(1);
//				 __m128i wide_dx4  = _mm_set1_epi32(dx * 4);
//				 __m128i wide_fx   = _mm_set_epi32(fx + dx * 3, fx + dx * 2,
//					 fx + dx, fx);
//
//				 while (count >= 4) {
//					 __m128i wide_out; 
//
//					 wide_out = _mm_slli_epi32(_mm_srai_epi32(wide_fx, 12), 14);
//					 wide_out = _mm_or_si128(wide_out, _mm_add_epi32(
//						 _mm_srai_epi32(wide_fx, 16), wide_1)); 
//
//					 _mm_store_si128(reinterpret_cast<__m128i*>(xy), wide_out);
//
//					 xy += 4;
//					 fx += dx * 4;
//					 wide_fx  = _mm_add_epi32(wide_fx, wide_dx4);
//					 count -= 4;
//				 } // while count >= 4
//			 } // if count >= 4
//
//			 while (count-- > 0) {
//				 SkASSERT((fx >> (16 + 14)) == 0);
//				 *xy++ = (fx >> 12 << 14) | ((fx >> 16) + 1);
//				 fx += dx;
//			 }
//	 } else {
//		 // SSE2 only support 16bit interger max & min, so only process the case
//		 // maxX less than the max 16bit interger. Actually maxX is the bitmap's
//		 // height, there should be rare bitmap whose height will be greater 
//		 // than max 16bit interger in the real world.
//		 if ((count >= 4) && (maxX <= 0xFFFF)) {
//			 while (((size_t)xy & 0x0F) != 0) {
//				 *xy++ = ClampX_ClampY_pack_filter(fx, maxX, one);
//				 fx += dx;
//				 count--;
//			 }
//
//			 __m128i wide_fx   = _mm_set_epi32(fx + dx * 3, fx + dx * 2,
//				 fx + dx, fx);
//			 __m128i wide_dx4  = _mm_set1_epi32(dx * 4);
//			 __m128i wide_one  = _mm_set1_epi32(one);
//			 __m128i wide_maxX = _mm_set1_epi32(maxX); 
//			 __m128i wide_mask = _mm_set1_epi32(0xF);
//
//			 while (count >= 4) {
//				 __m128i wide_i;
//				 __m128i wide_lo;
//				 __m128i wide_fx1;
//
//				 // i = SkClampMax(f>>16,maxX)
//				 wide_i = _mm_max_epi16(_mm_srli_epi32(wide_fx, 16), 
//					 _mm_setzero_si128());
//				 wide_i = _mm_min_epi16(wide_i, wide_maxX);
//
//				 // i<<4 | TILEX_LOW_BITS(fx)
//				 wide_lo = _mm_srli_epi32(wide_fx, 12);
//				 wide_lo = _mm_and_si128(wide_lo, wide_mask);
//				 wide_i  = _mm_slli_epi32(wide_i, 4);         
//				 wide_i  = _mm_or_si128(wide_i, wide_lo);     
//
//				 // i<<14
//				 wide_i = _mm_slli_epi32(wide_i, 14);
//
//				 // SkClampMax(((f+one))>>16,max)
//				 wide_fx1 = _mm_add_epi32(wide_fx, wide_one);
//				 wide_fx1 = _mm_max_epi16(_mm_srli_epi32(wide_fx1, 16), 
//					 _mm_setzero_si128());
//				 wide_fx1 = _mm_min_epi16(wide_fx1, wide_maxX);
//
//				 // final combination
//				 wide_i = _mm_or_si128(wide_i, wide_fx1);
//				 _mm_store_si128(reinterpret_cast<__m128i*>(xy), wide_i); 
//
//				 wide_fx = _mm_add_epi32(wide_fx, wide_dx4);
//				 fx += dx * 4;   
//				 xy += 4;
//				 count -= 4;
//			 } // while count >= 4
//		 } // if count >= 4
//
//		 while (count-- > 0) {
//			 *xy++ = ClampX_ClampY_pack_filter(fx, maxX, one);
//			 fx += dx;
//		 }
//	 }
//}

 #define PACK_TWO_SHORTS(pri, sec) ((pri) | ((sec) << 16))
 #define pack_two_shorts(pri, sec)   PACK_TWO_SHORTS(pri, sec)
 #define SkToU16(x)  ((uint16_t)(x))
void ClampX_ClampY_nofilter_scale_SSE2(SkMatrix* matrix, SkBitmap*fBitmap,
									   uint32_t xy[], int count, int x, int y) {
										   /*SkASSERT((s.fInvType & ~(SkMatrix::kTranslate_Mask |
										   SkMatrix::kScale_Mask)) == 0);*/

										   // we store y, x, x, x, x, x
   const unsigned maxX = fBitmap->fWidth - 1;
   SkFixed fx;
   SkScalar ptfx, ptfy;//SkPoint pt; 
   SkMatrix::ScaleTrans_xy(*matrix, SkIntToScalar(x) + SK_ScalarHalf,
	   SkIntToScalar(y) + SK_ScalarHalf, ptfx, ptfy);
   fx = SkScalarToFixed(ptfy);
   const unsigned maxY = fBitmap->fHeight - 1;
   *xy++ = SkClampMax(fx >> 16, maxY);
   fx = SkScalarToFixed(ptfx);

   if (0 == maxX) {
	   // all of the following X values must be 0
	   memset(xy, 0, count * sizeof(uint16_t));
	   return;
   }

   SkFixed fInvSx  = SkScalarToFixed(matrix->fMat[SkMatrix::kMScaleX]);
   const SkFixed dx = fInvSx;

   // test if we don't need to apply the tile proc
   if ((unsigned)(fx >> 16) <= maxX &&
	   (unsigned)((fx + dx * (count - 1)) >> 16) <= maxX) {
		   // SSE version of decal_nofilter_scale
		   if (count >= 8) {
			   while (((size_t)xy & 0x0F) != 0) {
				   *xy++ = pack_two_shorts(fx >> 16, (fx + dx) >> 16);
				   fx += 2 * dx;
				   count -= 2;
			   }

			   __m128i wide_dx4 = _mm_set1_epi32(dx * 4);
			   __m128i wide_dx8 = _mm_add_epi32(wide_dx4, wide_dx4);

			   __m128i wide_low = _mm_set_epi32(fx + dx * 3, fx + dx * 2,
				   fx + dx, fx);
			   __m128i wide_high = _mm_add_epi32(wide_low, wide_dx4);

			   while (count >= 8) {
				   __m128i wide_out_low = _mm_srli_epi32(wide_low, 16);
				   __m128i wide_out_high = _mm_srli_epi32(wide_high, 16);

				   __m128i wide_result = _mm_packs_epi32(wide_out_low,
					   wide_out_high);
				   _mm_store_si128(reinterpret_cast<__m128i*>(xy), wide_result);

				   wide_low = _mm_add_epi32(wide_low, wide_dx8);
				   wide_high = _mm_add_epi32(wide_high, wide_dx8);

				   xy += 4;
				   fx += dx * 8;
				   count -= 8;
			   }
		   } // if count >= 8

		   uint16_t* xx = reinterpret_cast<uint16_t*>(xy);
		   while (count-- > 0) {
			   *xx++ = SkToU16(fx >> 16);
			   fx += dx;
		   }
   } else {
	   // SSE2 only support 16bit interger max & min, so only process the case
	   // maxX less than the max 16bit interger. Actually maxX is the bitmap's
	   // height, there should be rare bitmap whose height will be greater 
	   // than max 16bit interger in the real world.
	   if ((count >= 8) && (maxX <= 0xFFFF)) {
		   while (((size_t)xy & 0x0F) != 0) {
			   *xy++ = pack_two_shorts(SkClampMax((fx + dx) >> 16, maxX),
				   SkClampMax(fx >> 16, maxX));
			   fx += 2 * dx;
			   count -= 2;
		   }

		   __m128i wide_dx4 = _mm_set1_epi32(dx * 4);
		   __m128i wide_dx8 = _mm_add_epi32(wide_dx4, wide_dx4);

		   __m128i wide_low = _mm_set_epi32(fx + dx * 3, fx + dx * 2,
			   fx + dx, fx);
		   __m128i wide_high = _mm_add_epi32(wide_low, wide_dx4);
		   __m128i wide_maxX = _mm_set1_epi32(maxX);

		   while (count >= 8) {
			   __m128i wide_out_low = _mm_srli_epi32(wide_low, 16);
			   __m128i wide_out_high = _mm_srli_epi32(wide_high, 16);

			   wide_out_low  = _mm_max_epi16(wide_out_low, 
				   _mm_setzero_si128());
			   wide_out_low  = _mm_min_epi16(wide_out_low, wide_maxX);
			   wide_out_high = _mm_max_epi16(wide_out_high,
				   _mm_setzero_si128());
			   wide_out_high = _mm_min_epi16(wide_out_high, wide_maxX);

			   __m128i wide_result = _mm_packs_epi32(wide_out_low,
				   wide_out_high);
			   _mm_store_si128(reinterpret_cast<__m128i*>(xy), wide_result);

			   wide_low  = _mm_add_epi32(wide_low, wide_dx8);
			   wide_high = _mm_add_epi32(wide_high, wide_dx8);

			   xy += 4;
			   fx += dx * 8;
			   count -= 8;
		   }
	   } // if count >= 8

	   uint16_t* xx = reinterpret_cast<uint16_t*>(xy);
	   while (count-- > 0) {
		   *xx++ = SkClampMax(fx >> 16, maxX);
		   fx += dx;
	   }
   }
}


typedef long long int64_t;
typedef int64_t SkFixed48;
#define SkIntToFixed48(x)       ((SkFixed48)(x) << 48)
#define SkFixed48ToInt(x)       ((int)((x) >> 48))
#define SkFixedToFixed48(x)     ((SkFixed48)(x) << 32)
#define SkFixed48ToFixed(x)     ((SkFixed)((x) >> 32))
#define SkFloatToFixed48(x)     ((SkFixed48)((x) * (65536.0f * 65536.0f * 65536.0f)))
#define SkScalarToFixed48(x)    SkFloatToFixed48(x)
#define SkScalarToFractionalInt(x)  SkScalarToFixed48(x)
#define SkFractionalIntToFixed(x)   SkFixed48ToFixed(x)
typedef SkFixed48    SkFractionalInt;
//#define TILEY_PROCF(fy, max)    (((fy) & 0xFFFF) * ((max) + 1) >> 16)
//#define TILEX_PROCF(fx, max)    (((fx) & 0xFFFF) * ((max) + 1) >> 16)
#define TILEY_PROCF(fy, max)    SkClampMax((fy) >> 16, max)
#define TILEX_PROCF(fx, max)    SkClampMax((fx) >> 16, max)

void decal_nofilter_scale(uint32_t dst[], SkFixed fx, SkFixed dx, int count)
{
	int i;

#if	defined(__ARM_HAVE_NEON)
	if (count >= 8) {
		/* SkFixed is 16.16 fixed point */
		SkFixed dx2 = dx+dx;
		SkFixed dx4 = dx2+dx2;
		SkFixed dx8 = dx4+dx4;

		/* now build fx/fx+dx/fx+2dx/fx+3dx */
		SkFixed fx1, fx2, fx3;
		int32x2_t lower, upper;
		int32x4_t lbase, hbase;
		uint16_t *dst16 = (uint16_t *)dst;

		fx1 = fx+dx;
		fx2 = fx1+dx;
		fx3 = fx2+dx;

		/* avoid an 'lbase unitialized' warning */
		lbase = vdupq_n_s32(fx);
		lbase = vsetq_lane_s32(fx1, lbase, 1);
		lbase = vsetq_lane_s32(fx2, lbase, 2);
		lbase = vsetq_lane_s32(fx3, lbase, 3);
		hbase = vaddq_s32(lbase, vdupq_n_s32(dx4));

		/* take upper 16 of each, store, and bump everything */
		do {
			int32x4_t lout, hout;
			uint16x8_t hi16;

			lout = lbase;
			hout = hbase;
			/* gets hi's of all louts then hi's of all houts */
			asm ("vuzpq.16 %q0, %q1" : "+w" (lout), "+w" (hout));
			hi16 = vreinterpretq_u16_s32(hout);
			vst1q_u16(dst16, hi16);

			/* on to the next */
			lbase = vaddq_s32 (lbase, vdupq_n_s32(dx8));
			hbase = vaddq_s32 (hbase, vdupq_n_s32(dx8));
			dst16 += 8;
			count -= 8;
			fx += dx8;
		} while (count >= 8);
		dst = (uint32_t *) dst16;
	}
#else
	for (i = (count >> 2); i > 0; --i)
	{
		*dst++ = pack_two_shorts(fx >> 16, (fx + dx) >> 16);
		fx += dx+dx;
		*dst++ = pack_two_shorts(fx >> 16, (fx + dx) >> 16);
		fx += dx+dx;
	}
	count &= 3;
#endif

	uint16_t* xx = (uint16_t*)dst;
	for (i = count; i > 0; --i) {
		*xx++ = SkToU16(fx >> 16); fx += dx;
	}
}

#define CHECK_FOR_DECAL
void ClampX_ClampY_nofilter_scale(SkMatrix* matrix, SkBitmap*fBitmap,
						 uint32_t xy[], int count, int x, int y) {

		 const unsigned maxX = fBitmap->fWidth - 1;
		 SkFractionalInt fx;
		 {
			 

			 SkScalar ptfx, ptfy;//SkPoint pt; 
			SkMatrix::ScaleTrans_xy(*matrix, SkIntToScalar(x) + SK_ScalarHalf,
				 SkIntToScalar(y) + SK_ScalarHalf,  ptfx, ptfy);
			 fx = SkScalarToFractionalInt(ptfy);
			 const unsigned maxY =fBitmap->fHeight - 1;
			 *xy++ = TILEY_PROCF(SkFractionalIntToFixed(fx), maxY);
			 fx = SkScalarToFractionalInt(ptfx);
		 }

		 if (0 == maxX) {
			 // all of the following X values must be 0
			 memset(xy, 0, count * sizeof(uint16_t));
			 return;
		 }

		 const SkFractionalInt dx = SkScalarToFractionalInt(matrix->fMat[0]);

#ifdef CHECK_FOR_DECAL
		 // test if we don't need to apply the tile proc
		 SkFixed tmpFx = SkFractionalIntToFixed(fx);
		 SkFixed tmpDx = SkFractionalIntToFixed(dx);
		 if ((unsigned)(tmpFx >> 16) <= maxX &&
			 (unsigned)((tmpFx + tmpDx * (count - 1)) >> 16) <= maxX) {
				 decal_nofilter_scale(xy, tmpFx, tmpDx, count);
		 } else
#endif
		 {
			 int i;
			 for (i = (count >> 2); i > 0; --i) {
				 unsigned a, b;
				 a = TILEX_PROCF(SkFractionalIntToFixed(fx), maxX); fx += dx;
				 b = TILEX_PROCF(SkFractionalIntToFixed(fx), maxX); fx += dx;
#ifdef SK_CPU_BENDIAN
				 *xy++ = (a << 16) | b;
#else
				 *xy++ = (b << 16) | a;
#endif
				 a = TILEX_PROCF(SkFractionalIntToFixed(fx), maxX); fx += dx;
				 b = TILEX_PROCF(SkFractionalIntToFixed(fx), maxX); fx += dx;
#ifdef SK_CPU_BENDIAN
				 *xy++ = (a << 16) | b;
#else
				 *xy++ = (b << 16) | a;
#endif
			 }
			 uint16_t* xx = (uint16_t*)xy;
			 for (i = (count & 3); i > 0; --i) {
				 *xx++ = TILEX_PROCF(SkFractionalIntToFixed(fx), maxX); fx += dx;
			 }
		 }
}
///////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
#define BUF_MAX     128

int maxCountForBufferSize(size_t bufferSize){
	int32_t size = static_cast<int32_t>(bufferSize);
	size &= ~3; // only care about 4-byte aligned chunks
	/*if (fInvType <= (SkMatrix::kTranslate_Mask | SkMatrix::kScale_Mask))*/ {
		size -= 4;   // the shared Y (or YY) coordinate
		if (size < 0) {
			size = 0;
		}
		size >>= 1;
	}
	/*else {
		size >>= 2;
	}*/

	//if (fDoFilter) {
	//	size >>= 1;
	//}

	return size;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
#define assign_16_longs(dst, value)             \
	do {                                        \
	*(dst)++ = value;   *(dst)++ = value;   \
	*(dst)++ = value;   *(dst)++ = value;   \
	*(dst)++ = value;   *(dst)++ = value;   \
	*(dst)++ = value;   *(dst)++ = value;   \
	*(dst)++ = value;   *(dst)++ = value;   \
	*(dst)++ = value;   *(dst)++ = value;   \
	*(dst)++ = value;   *(dst)++ = value;   \
	*(dst)++ = value;   *(dst)++ = value;   \
	} while (0)
void sk_memset32(uint32_t dst[], uint32_t value, int count) {
	SkASSERT(dst != NULL && count >= 0);

	int sixteenlongs = count >> 4;
	if (sixteenlongs) {
		do {
			assign_16_longs(dst, value);
		} while (--sixteenlongs != 0);
		count &= 15;
	}

	if (count) {
		do {
			*dst++ = value;
		} while (--count != 0);
	}
}

void sk_memset32_SSE2(uint32_t *dst, uint32_t value, int count)
{
	SkASSERT(dst != NULL && count >= 0);

	// dst must be 4-byte aligned.
	SkASSERT((((size_t) dst) & 0x03) == 0);

	if (count >= 16) {
		while (((size_t)dst) & 0x0F) {
			*dst++ = value;
			--count;
		}
		__m128i *d = reinterpret_cast<__m128i*>(dst);
		__m128i value_wide = _mm_set1_epi32(value);
		while (count >= 16) {
			_mm_store_si128(d++, value_wide);
			_mm_store_si128(d++, value_wide);
			_mm_store_si128(d++, value_wide);
			_mm_store_si128(d++, value_wide);
			count -= 16;
		}
		dst = reinterpret_cast<uint32_t*>(d);
	}
	while (count > 0) {
		*dst++ = value;
		--count;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
#define DSTSIZE                 32
#define SRCTYPE                 SkPMColor
//#define PREAMBLE(state)         unsigned alphaScale = state.fAlphaScale
#define RETURNDST(src)          SkAlphaMulQ(src, alphaScale)
#define DSTTYPE                 uint32_t
//#define BITMAPPROC_MEMSET(ptr, value, n) sk_memset32_SSE2(ptr, value, n)
#define PACK_TWO_SHORTS(pri, sec) ((pri) | ((sec) << 16))
#define UNPACK_PRIMARY_SHORT(packed)    ((packed) & 0xFFFF)
#define UNPACK_SECONDARY_SHORT(packed)  ((uint32_t)(packed) >> 16)

void S32_alpha_D32__nofilter_DX(SkBitmap*fBitmap,unsigned fAlphaScale,
							const uint32_t* SK_RESTRICT xy,
							int count, DSTTYPE* SK_RESTRICT colors) {

	unsigned alphaScale = fAlphaScale;
	const SRCTYPE* SK_RESTRICT srcAddr = (const SRCTYPE*)fBitmap->fPixels;

	// buffer is y32, x16, x16, x16, x16, x16
	// bump srcAddr to the proper row, since we're told Y never changes
	SkASSERT((unsigned)xy[0] < (unsigned)fBitmap->fHeight);
	srcAddr = (const SRCTYPE*)((const char*)srcAddr +
		xy[0] * fBitmap->fRowBytes);
	xy += 1;

	SRCTYPE src;

	if (1 == fBitmap->fWidth) {
		src = srcAddr[0];
		DSTTYPE dstValue = RETURNDST(src);
		sk_memset32_proc(colors, dstValue, count);
	} else {
		int i;
		for (i = (count >> 2); i > 0; --i) {
			uint32_t xx0 = *xy++;
			uint32_t xx1 = *xy++;
			SRCTYPE x0 = srcAddr[UNPACK_PRIMARY_SHORT(xx0)];
			SRCTYPE x1 = srcAddr[UNPACK_SECONDARY_SHORT(xx0)];
			SRCTYPE x2 = srcAddr[UNPACK_PRIMARY_SHORT(xx1)];
			SRCTYPE x3 = srcAddr[UNPACK_SECONDARY_SHORT(xx1)];

			*colors++ = RETURNDST(x0);
			*colors++ = RETURNDST(x1);
			*colors++ = RETURNDST(x2);
			*colors++ = RETURNDST(x3);
		}
		const uint16_t* SK_RESTRICT xx = (const uint16_t*)(xy);
		for (i = (count & 3); i > 0; --i) {
			SkASSERT(*xx < (unsigned)s.fBitmap->width());
			src = srcAddr[*xx++]; *colors++ = RETURNDST(src);
		}
	}

#ifdef POSTAMBLE
	POSTAMBLE(s);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////
#define RETURNDST1(src)          src
/////////////////////////////////////////////////////////////////////////////////////////
void S32_opaque_D32_nofilter_DX(SkBitmap*fBitmap, unsigned fAlphaScale,
							const uint32_t* SK_RESTRICT xy,
							int count, DSTTYPE* SK_RESTRICT colors) {

//#ifdef PREAMBLE
//	PREAMBLE(s);
//#endif
	const SRCTYPE* SK_RESTRICT srcAddr = (const SRCTYPE*)fBitmap->fPixels;

	// buffer is y32, x16, x16, x16, x16, x16
	// bump srcAddr to the proper row, since we're told Y never changes
	SkASSERT((unsigned)xy[0] < (unsigned)fBitmap->fHeight);
	srcAddr = (const SRCTYPE*)((const char*)srcAddr +
		xy[0] * fBitmap->fRowBytes);
	xy += 1;

	SRCTYPE src;

	if (1 == fBitmap->fWidth) {
		src = srcAddr[0];
		DSTTYPE dstValue = RETURNDST1(src);
		sk_memset32_proc(colors, dstValue, count);
	} else {
		int i;
		for (i = (count >> 2); i > 0; --i) {
			uint32_t xx0 = *xy++;
			uint32_t xx1 = *xy++;
			SRCTYPE x0 = srcAddr[UNPACK_PRIMARY_SHORT(xx0)];
			SRCTYPE x1 = srcAddr[UNPACK_SECONDARY_SHORT(xx0)];
			SRCTYPE x2 = srcAddr[UNPACK_PRIMARY_SHORT(xx1)];
			SRCTYPE x3 = srcAddr[UNPACK_SECONDARY_SHORT(xx1)];

			*colors++ = RETURNDST1(x0);
			*colors++ = RETURNDST1(x1);
			*colors++ = RETURNDST1(x2);
			*colors++ = RETURNDST1(x3);
		}
		const uint16_t* SK_RESTRICT xx = (const uint16_t*)(xy);
		for (i = (count & 3); i > 0; --i) {
			SkASSERT(*xx < (unsigned)s.fBitmap->width());
			src = srcAddr[*xx++]; *colors++ = RETURNDST1(src);
		}
	}

#ifdef POSTAMBLE
	POSTAMBLE(s);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////


inline void shadeSpan(SkBitmap* pDrawDest, SkBitmap* pBitmapSrc,  SkMatrix* matrix, U8CPU alpha, int x, int y, SkPMColor dstC[], int count)
{
	uint32_t buffer[BUF_MAX];
	int max =  maxCountForBufferSize(sizeof(buffer[0]) * BUF_MAX);

	
	for (;;) {
		int n = count;
		if (n > max) {
			n = max;
		}
		
		ClampX_ClampY_nofilter_scale_proc(matrix, pBitmapSrc, buffer, n, x, y);
		if (alpha < 255)
			S32_alpha_D32__nofilter_DX(pBitmapSrc, alpha + 1, buffer, n, dstC);
		else
			S32_opaque_D32_nofilter_DX(pBitmapSrc, alpha + 1, buffer, n, dstC);
	//	S32_generic_D32_filter_DX_SSSE3(pBitmapSrc,256, buffer, n, dstC);
		//mproc(state, buffer, n, x, y);

		//sproc(state, buffer, n, dstC);

		if ((count -= n) == 0) {
			break;
		}
		x += n;
		dstC += n;
	}
}
inline void blitH(SkBitmap* pDrawDest, SkBitmap* pBitmapSrc,  SkMatrix* matrix,  U8CPU alpha, SkPMColor*  fBuffer, int x, int y, int width)
{
	uint32_t*  device =  (uint32_t*)((char*)pDrawDest->fPixels + y * pDrawDest->fRowBytes + (x << 2));

	SkPMColor*  span = fBuffer;
	
	shadeSpan(pDrawDest, pBitmapSrc, matrix, alpha, x, y, span, width);

    S32A_Opaque_BlitRow32_proc(device, fBuffer, width, 255);

}


void AlphaBlendEx(__in  ImageData* pBitmapDest, __in int xoriginDest, __in int yoriginDest, __in int wDest, __in int hDest, __in ImageData* pBitmapSrc, __in int xoriginSrc, __in int yoriginSrc, __in int wSrc, __in int hSrc, __in unsigned int alpha, __in RECT* clipBounds, __in char* pbuffer)
{
	if (sk_memset32_proc == NULL)
	{
		if (cachedHasSSE2())
		{
			sk_memset32_proc = sk_memset32_SSE2;
			ClampX_ClampY_nofilter_scale_proc = ClampX_ClampY_nofilter_scale_SSE2;
			S32A_Opaque_BlitRow32_proc =  S32A_Opaque_BlitRow32_SSE2;

		}
		else
		{
			sk_memset32_proc = sk_memset32;
			ClampX_ClampY_nofilter_scale_proc = ClampX_ClampY_nofilter_scale;
			S32A_Opaque_BlitRow32_proc =  S32A_Opaque_BlitRow32;

		}
	}

	 SkRect tmpSrc;
	 tmpSrc.fLeft = SkIntToScalar(0);
	 tmpSrc.fTop = SkIntToScalar(0);
	 tmpSrc.fRight = SkIntToScalar(wSrc);
	 tmpSrc.fBottom = SkIntToScalar(hSrc);

	  SkRect dst;
	  dst.fLeft = SkIntToScalar(xoriginDest);
	  dst.fTop = SkIntToScalar(yoriginDest);
	  dst.fRight = SkIntToScalar(xoriginDest + wDest);
	  dst.fBottom = SkIntToScalar(yoriginDest + hDest);

	  SkMatrix matrix;
	  matrix.setRectToRect(tmpSrc, dst);

	  SkMatrix matrixInverse;

	  if (!matrix.invert(&matrixInverse))
		  return;


	 RECT tmpRectSrc = {0, 0, pBitmapSrc->fWidth, pBitmapSrc->fHeight};
	 RECT tmpRectSrc1 = {xoriginSrc, yoriginSrc,xoriginSrc + wSrc, yoriginSrc + hSrc};
	  if (!IntersectRect(&tmpRectSrc, &tmpRectSrc, &tmpRectSrc1))
		  return;

	 SkBitmap dstBitmap, srcBitmap;
	 dstBitmap.SetPixels(pBitmapDest->fPixels, pBitmapDest->fWidth, pBitmapDest->fHeight);
	 // 计算偏移量
	 int offset = tmpRectSrc.left;
	 offset <<= 2;
	 unsigned int    fRowBytes = pBitmapSrc->fWidth * 4;
	 size_t   pixelRefOffset = tmpRectSrc.top * fRowBytes + offset;

	 srcBitmap.SetPixels(pBitmapSrc->fPixels, tmpRectSrc.right-tmpRectSrc.left, tmpRectSrc.bottom - tmpRectSrc.top,  pixelRefOffset, fRowBytes);
	 srcBitmap.fPixels = (char*)srcBitmap.fPixels + srcBitmap.fPixelRefOffset;


	 RECT tmpRectDst = {xoriginDest, yoriginDest, xoriginDest + wDest, yoriginDest + hDest};
	 if (clipBounds)
	 {
		 if (!IntersectRect(&tmpRectDst, &tmpRectDst, clipBounds))
			 return;
	 }
	 RECT tmpRectDst1 = {0, 0, pBitmapDest->fWidth, pBitmapDest->fHeight};
	 if (!IntersectRect(&tmpRectDst, &tmpRectDst, &tmpRectDst1))
		 return;

	 SkPMColor*     fBuffer =  (SkPMColor*)pbuffer;
	 if (!fBuffer) fBuffer = (SkPMColor*)malloc(pBitmapDest->fWidth * sizeof(SkPMColor));
	 if (!fBuffer) return;

	 int x = tmpRectDst.left;
	 int y = tmpRectDst.top;
	 int width = tmpRectDst.right - tmpRectDst.left;
	 int height = tmpRectDst.bottom -  tmpRectDst.top;
	 while (--height >= 0) {
		 blitH(&dstBitmap, &srcBitmap, &matrixInverse, alpha, fBuffer,  x, y++, width);
	 }
	
	 if (!pbuffer)
	 {
		 if (fBuffer) free(fBuffer);
		 fBuffer = NULL;
	 }
}