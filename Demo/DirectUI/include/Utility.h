#pragma once

#include <Windows.h>
//{{
typedef bool (*array_compare_t)(LPVOID ptrA, LPVOID ptrB);
//}}
/// ��������.
class DUI_API CStdPtrArray
{
	//{{
public:
	CStdPtrArray(int iPreallocSize = 0);
	 ~CStdPtrArray();
	//}}
	void Empty();
	void Resize(int iSize);
	bool IsEmpty() const;
	int Find(LPVOID iIndex) const;
	bool Add(LPVOID pData);
	bool SetAt(int iIndex, LPVOID pData);
	bool InsertAt(int iIndex, LPVOID pData);
	bool Remove(int iIndex);
	bool Remove(LPVOID iIndex);
	void Swap(int nStart, int nEnd);
	/// ���佻��
	void Swap(int nStart1, int nEnd1, int nStart2, int nEnd2);
	bool Move(int nStart, int nEnd, int nBoundryTo);
	int GetSize() const;
	LPVOID* GetData();

	LPVOID GetAt(int iIndex) const;
	LPVOID operator[] (int nIndex) const;
	//{{
	void Sort(array_compare_t cmp, int nBegin = 0, int nSize = -1);
protected:
	LPVOID* m_ppVoid;
	int m_nCount;
	int m_nAllocated;
	//}}
};

class  DUI_API Charset
{
public:
	// ANSIתΪUnicode
	static wstring AnsiToUnicode(const char* src);

	// ANSIתΪUTF8
	static string AnsiToUTF8(const char* src);
	// UTF8תΪANSI
	//{{
	static string UTF8ToAnsi(const char* src);
	//}}
	// UTF8תΪUNICODE
	static wstring UTF8ToUnicode(const char* src);
	// UNICODEתΪUTF8
	static string UnicodeToUTF8(const wchar_t*src);

	// UTF8תΪANSI��Unicode
	static tstring UTF8ToNative(const char* src);
	// ANSI��UnicodeתΪUTF8
	static string  NativeToUTF8(LPCTSTR src);

	static wstring  TCHARToUnicode(LPCTSTR src);

	static string UnicodeTochar(const wchar_t*src);
	
};


class DUI_API TweenAlgorithm
{
public:
	typedef enum{
		TweenEasein = 0,
		TweenEaseout = 1,
		TweenEaseinout = 2,
	}TweenEaseType;
	typedef enum{
		TweenLinear = 0,
		TweenQuadratic = 1,
		TweenCubic = 2,
		TweenQuartic = 3,
		TweenQuintic = 4,
		TweenSinusoidal = 5,
		TweenExponential = 6,
		TweenCircular = 7,
		TweenElastic = 8,
		TweenBack = 9,
		TweenBounce = 10,
	}TweenType;
	//{{
	static double Tween(TweenAlgorithm::TweenType type, double currentTime, double beginValue, double changeValue, double duration, TweenAlgorithm::TweenEaseType iTweenEaseType = TweenAlgorithm::TweenEasein, double s = 1.70158);

	static double Linear(	double currentTime, double beginValue, double changeValue, double duration );
	static double Quad(		double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0 ); 
	static double Cubic(	double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0 ); 
	static double Quart(	double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0 );
	static double Quint(	double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0 );
	static double Sine(		double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0 );
	static double Expo(		double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0 );
	static double Circ(		double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0 );
	static double Elastic(	double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0 );
	static double Back(		double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0, double s = 1.70158);
	static double Bounce(	double currentTime, double beginValue, double changeValue, double duration, int iTweenEaseType = 0 );
	//}}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CurveImpl;
class DUI_API CurveObj : public BaseObject
{
	//{{
public:
	friend class BezierAlgorithm;
	CurveObj();

	virtual ~CurveObj();
	void AddPoint(float tp, float sp);
	void Reset();
protected:
	CurveImpl* m_pCurveImpl;
	//}}
};
class DUI_API BezierAlgorithm
{
public:
	static long BezierValue(CurveObj* curve, long currentTime, long beginValue, long changeValue, long totalTime);
	static POINT BezierPoint(CurveObj* curve, long currentTime, const POINT& beginPoint, const POINT& destPoint, long totalTime);
	static RECT  BezierRect(CurveObj* curve, long currentTime, const RECT& beginRect, const RECT& destRect, long totalTime);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class DUI_API StrUtil
{
public:
	static tstring Replace(tstring &str, const TCHAR* string_to_replace, const TCHAR* new_string);
    static void ToLowerCase( tstring& str );
    static void ToUpperCase( tstring& str );
	static void Trim( tstring& str, bool left = true, bool right = true );
	static tstring ConvertBSTR(BSTR bstr);
	//{{
    static vector<tstring> Split( const tstring& str, const tstring& delims = _T("\t\n "), unsigned int maxSplits = 0);
	//}}
	static bool IsMailFormat(const tstring& str);
	static bool IsNumberFormat(const tstring& str);
	static bool IsLetterFormat(const tstring& str);
	static bool IsChineseFormat(const tstring& str);
};
//{{
extern "C" 
{
//}}
	DUI_API const char* GetDirectUIVersion();
	DUI_API void ShowDirectUIVersion();
//{{
}
//}}

