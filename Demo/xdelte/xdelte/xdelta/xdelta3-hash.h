/* xdelta 3 - delta compression tools and library
 * Copyright (C) 2001, 2003, 2004, 2005, 2006, 2007.  Joshua P. MacDonald
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* To know more about Xdelta, start by reading xdelta3.c.  If you are
 * ready to use the API, continue reading here.  There are two
 * interfaces -- xd3_encode_input and xd3_decode_input -- plus a dozen
 * or so related calls.  This interface is styled after Zlib. */

#ifndef _XDELTA3_HASH_H_
#define _XDELTA3_HASH_H_

#if XD3_DEBUG
#define SMALL_HASH_DEBUG1(s,inp)                                  \
  usize_t debug_hval = xd3_checksum_hash (& (s)->small_hash,       \
         xd3_scksum ((inp), (s)->smatcher.small_look))
#define SMALL_HASH_DEBUG2(s,inp)                                  \
  XD3_ASSERT (debug_hval == xd3_checksum_hash (& (s)->small_hash, \
	 xd3_scksum ((inp), (s)->smatcher.small_look)))
#else
#define SMALL_HASH_DEBUG1(s,inp)
#define SMALL_HASH_DEBUG2(s,inp)
#endif /* XD3_DEBUG */

/* Update the run-length state */
#define NEXTRUN(c) do { if ((c) == run_c) { run_l += 1; } \
  else { run_c = (c); run_l = 1; } } while (0)

/* Update the checksum state. */
#if ADLER_LARGE_CKSUM
#define LARGE_CKSUM_UPDATE(cksum,base,look)                              \
  do {                                                                   \
    uint32_t old_c = PERMUTE((base)[0]);                                    \
    uint32_t new_c = PERMUTE((base)[(look)]);                               \
    uint32_t low   = (((cksum) & 0xffff) - old_c + new_c) & 0xffff;         \
    uint32_t high  = (((cksum) >> 16) - (old_c * (look)) + low) & 0xffff;   \
    (cksum) = (high << 16) | low;                                        \
  } while (0)
#else

// This is a table of powers of 1597334677 (a good MLCG multiplier for 32-bit
// modular arithmetic).  TODO: add citation for "linear congruential
// generators of different sizes and good lattice structure"
static const uint32_t hash_multiplier = 1597334677U;
static const uint32_t hash_multiplier_powers[64] = {
  1U,          1597334677U, 1664532153U, 1152009645U, 
  438510001U,  2554380293U, 2226829033U, 2237430173U, 
  785718369U,  1614047349U, 2189918233U, 3004453517U, 
  2504269841U, 1873956325U, 3298675273U, 184041597U, 
  412048577U,  513892437U,  1512523129U, 420197229U, 
  3195470449U, 3365769157U, 782672297U,  1830755165U, 
  3607904545U, 2926038069U, 2592473U,    2764040269U, 
  2237745361U, 1146488229U, 3329146121U, 975735357U, 
  4047063425U, 3720555541U, 992599097U,  2284876077U, 
  1371042609U, 2776575877U, 857709673U,  1933315357U, 
  18085345U,   3544121333U, 2019536281U, 657247757U, 
  3376546193U, 235703653U,  3581067209U, 1515267069U, 
  2578040385U, 3954624469U, 25854713U,   2236907245U, 
  3587404785U, 2101452613U, 498181929U,  2150584029U, 
  1830304417U, 3629515701U, 1929108569U, 4095149005U, 
  3948038737U, 2377245989U, 565564041U,  309202365U, 
};

#define LARGE_CKSUM_UPDATE(cksum,base,look) \
  do { \
    cksum = (cksum * hash_multiplier) - \
      (hash_multiplier_powers[look] * PERMUTE(base[0])) + \
      PERMUTE(base[look]); \
  } while (0)
#endif

#if ARITH_SMALL_CKSUM
#define SMALL_CKSUM_UPDATE(cksum,base,look) \
  (cksum) = ((*(uint32_t*)(base+1)) * 1597334677U)
#else
#define SMALL_CKSUM_UPDATE LARGE_CKSUM_UPDATE
#endif

/***********************************************************************
 Permute stuff
 ***********************************************************************/

#if HASH_PERMUTE == 0
#define PERMUTE(x) (x)
#else
#define PERMUTE(x) (__single_hash[(uint)x])

static const uint16_t __single_hash[256] =
{
  /* Random numbers generated using SLIB's pseudo-random number generator.
   * This hashes the input alphabet. */
  0xbcd1, 0xbb65, 0x42c2, 0xdffe, 0x9666, 0x431b, 0x8504, 0xeb46,
  0x6379, 0xd460, 0xcf14, 0x53cf, 0xdb51, 0xdb08, 0x12c8, 0xf602,
  0xe766, 0x2394, 0x250d, 0xdcbb, 0xa678, 0x02af, 0xa5c6, 0x7ea6,
  0xb645, 0xcb4d, 0xc44b, 0xe5dc, 0x9fe6, 0x5b5c, 0x35f5, 0x701a,
  0x220f, 0x6c38, 0x1a56, 0x4ca3, 0xffc6, 0xb152, 0x8d61, 0x7a58,
  0x9025, 0x8b3d, 0xbf0f, 0x95a3, 0xe5f4, 0xc127, 0x3bed, 0x320b,
  0xb7f3, 0x6054, 0x333c, 0xd383, 0x8154, 0x5242, 0x4e0d, 0x0a94,
  0x7028, 0x8689, 0x3a22, 0x0980, 0x1847, 0xb0f1, 0x9b5c, 0x4176,
  0xb858, 0xd542, 0x1f6c, 0x2497, 0x6a5a, 0x9fa9, 0x8c5a, 0x7743,
  0xa8a9, 0x9a02, 0x4918, 0x438c, 0xc388, 0x9e2b, 0x4cad, 0x01b6,
  0xab19, 0xf777, 0x365f, 0x1eb2, 0x091e, 0x7bf8, 0x7a8e, 0x5227,
  0xeab1, 0x2074, 0x4523, 0xe781, 0x01a3, 0x163d, 0x3b2e, 0x287d,
  0x5e7f, 0xa063, 0xb134, 0x8fae, 0x5e8e, 0xb7b7, 0x4548, 0x1f5a,
  0xfa56, 0x7a24, 0x900f, 0x42dc, 0xcc69, 0x02a0, 0x0b22, 0xdb31,
  0x71fe, 0x0c7d, 0x1732, 0x1159, 0xcb09, 0xe1d2, 0x1351, 0x52e9,
  0xf536, 0x5a4f, 0xc316, 0x6bf9, 0x8994, 0xb774, 0x5f3e, 0xf6d6,
  0x3a61, 0xf82c, 0xcc22, 0x9d06, 0x299c, 0x09e5, 0x1eec, 0x514f,
  0x8d53, 0xa650, 0x5c6e, 0xc577, 0x7958, 0x71ac, 0x8916, 0x9b4f,
  0x2c09, 0x5211, 0xf6d8, 0xcaaa, 0xf7ef, 0x287f, 0x7a94, 0xab49,
  0xfa2c, 0x7222, 0xe457, 0xd71a, 0x00c3, 0x1a76, 0xe98c, 0xc037,
  0x8208, 0x5c2d, 0xdfda, 0xe5f5, 0x0b45, 0x15ce, 0x8a7e, 0xfcad,
  0xaa2d, 0x4b5c, 0xd42e, 0xb251, 0x907e, 0x9a47, 0xc9a6, 0xd93f,
  0x085e, 0x35ce, 0xa153, 0x7e7b, 0x9f0b, 0x25aa, 0x5d9f, 0xc04d,
  0x8a0e, 0x2875, 0x4a1c, 0x295f, 0x1393, 0xf760, 0x9178, 0x0f5b,
  0xfa7d, 0x83b4, 0x2082, 0x721d, 0x6462, 0x0368, 0x67e2, 0x8624,
  0x194d, 0x22f6, 0x78fb, 0x6791, 0xb238, 0xb332, 0x7276, 0xf272,
  0x47ec, 0x4504, 0xa961, 0x9fc8, 0x3fdc, 0xb413, 0x007a, 0x0806,
  0x7458, 0x95c6, 0xccaa, 0x18d6, 0xe2ae, 0x1b06, 0xf3f6, 0x5050,
  0xc8e8, 0xf4ac, 0xc04c, 0xf41c, 0x992f, 0xae44, 0x5f1b, 0x1113,
  0x1738, 0xd9a8, 0x19ea, 0x2d33, 0x9698, 0x2fe9, 0x323f, 0xcde2,
  0x6d71, 0xe37d, 0xb697, 0x2c4f, 0x4373, 0x9102, 0x075d, 0x8e25,
  0x1672, 0xec28, 0x6acb, 0x86cc, 0x186e, 0x9414, 0xd674, 0xd1a5
};
#endif

/***********************************************************************
 Ctable stuff
 ***********************************************************************/

static inline usize_t
xd3_checksum_hash (const xd3_hash_cfg *cfg, const usize_t cksum)
{
  return (cksum >> cfg->shift) ^ (cksum & cfg->mask);
}

/***********************************************************************
 Cksum function
 ***********************************************************************/

#if ADLER_LARGE_CKSUM
static inline uint32_t
xd3_lcksum (const uint8_t *seg, const int ln)
{
  int i = 0;
  uint32_t low  = 0;
  uint32_t high = 0;

  for (; i < ln; i += 1)
    {
      low  += PERMUTE(*seg++);
      high += low;
    }

  return ((high & 0xffff) << 16) | (low & 0xffff);
}
#else
static inline uint32_t
xd3_lcksum (const uint8_t *seg, const int ln)
{
  int i, j;
  uint32_t h = 0;
  for (i = 0, j = ln - 1; i < ln; ++i, --j) {
    h += PERMUTE(seg[i]) * hash_multiplier_powers[j];
  }
  return h;
}
#endif

#if ARITH_SMALL_CKSUM
static inline usize_t
xd3_scksum (const uint8_t *seg, const int ln)
{
  usize_t c;
  /* The -1 is because UPDATE operates on seg[1..ln] */
  SMALL_CKSUM_UPDATE (c,(seg-1),ln);
  return c;
}
#else
#define xd3_scksum(seg,ln) xd3_lcksum(seg,ln)
#endif

static usize_t
xd3_size_log2 (usize_t slots)
{
  int bits = 28; /* This should not be an unreasonable limit. */
  int i;

  for (i = 3; i <= bits; i += 1)
    {
      if (slots < (1U << i))
	{
	  /* TODO: this is compaction=1 in checksum_test.cc and maybe should
	   * not be fixed at -1. */
	  bits = i - 1; 
	  break;
	}
    }

  return bits;
}

static void
xd3_size_hashtable (xd3_stream    *stream,
		    usize_t        slots,
		    xd3_hash_cfg  *cfg)
{
  int bits = xd3_size_log2 (slots);

  /* TODO: there's a 32-bit assumption here */
  cfg->size  = (1 << bits);
  cfg->mask  = (cfg->size - 1);
  cfg->shift = 32 - bits;
}

#endif
