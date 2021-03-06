/*********************************************************************
 *       Copyright (C) 2010,应用软件开发
*********************************************************************/
#include "stdafx.h"
#include "Utility.h"


#pragma warning( disable : 4244 )
CStdPtrArray::CStdPtrArray(int iPreallocSize) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(iPreallocSize)
{
	if (iPreallocSize > 0) m_ppVoid = static_cast<LPVOID*>(malloc(iPreallocSize * sizeof(LPVOID)));
}

CStdPtrArray::~CStdPtrArray()
{
	Empty();
}

void CStdPtrArray::Empty()
{
	if (m_ppVoid != NULL) free(m_ppVoid);
	m_ppVoid = NULL;
	m_nCount = m_nAllocated = 0;
}

void CStdPtrArray::Resize(int iSize)
{
	Empty();
	m_ppVoid = static_cast<LPVOID*>(malloc(iSize * sizeof(LPVOID)));
	::ZeroMemory(m_ppVoid, iSize * sizeof(LPVOID));
	m_nAllocated = iSize;
	m_nCount = iSize;
}

bool CStdPtrArray::IsEmpty() const
{
	return m_nCount == 0;
}

bool CStdPtrArray::Add(LPVOID pData)
{
	if (++m_nCount >= m_nAllocated) {
		m_nAllocated *= 2;
		if (m_nAllocated == 0)
			m_nAllocated = 11;
		LPVOID* temp = static_cast<LPVOID*>(realloc(m_ppVoid, m_nAllocated * sizeof(LPVOID)));
		if (temp == NULL)
			return false;
		m_ppVoid = temp;
	}
	m_ppVoid[m_nCount - 1] = pData;
	return true;
}

bool CStdPtrArray::InsertAt(int iIndex, LPVOID pData)
{
	if (iIndex == m_nCount) return Add(pData);
	if (iIndex < 0 || iIndex > m_nCount) return false;
	if (++m_nCount >= m_nAllocated) {
		m_nAllocated *= 2;
		if (m_nAllocated == 0) m_nAllocated = 11;
		LPVOID* temp = static_cast<LPVOID*>(realloc(m_ppVoid, m_nAllocated * sizeof(LPVOID)));
		if (temp == NULL)
			return false;
		m_ppVoid = temp;
	}
	memmove(&m_ppVoid[iIndex + 1], &m_ppVoid[iIndex], (m_nCount - iIndex - 1) * sizeof(LPVOID));
	m_ppVoid[iIndex] = pData;
	return true;
}

bool CStdPtrArray::SetAt(int iIndex, LPVOID pData)
{
	if (iIndex < 0 || iIndex >= m_nCount) return false;
	m_ppVoid[iIndex] = pData;
	return true;
}

bool CStdPtrArray::Remove(int iIndex)
{
	if (iIndex < 0 || iIndex >= m_nCount) return false;
	if (iIndex < --m_nCount) ::CopyMemory(m_ppVoid + iIndex, m_ppVoid + iIndex + 1, (m_nCount - iIndex) * sizeof(LPVOID));
	return true;
}

bool  CStdPtrArray::Remove(LPVOID iIndex)
{
	int i = Find(iIndex);
	return Remove(i);
}

int CStdPtrArray::Find(LPVOID pData) const
{
	for (int i = 0; i < m_nCount; i++) if (m_ppVoid[i] == pData) return i;
	return -1;
}

int CStdPtrArray::GetSize() const
{
	return m_nCount;
}

LPVOID* CStdPtrArray::GetData()
{
	return m_ppVoid;
}

LPVOID CStdPtrArray::GetAt(int iIndex) const
{
	if (iIndex < 0 || iIndex >= m_nCount) return NULL;
	return m_ppVoid[iIndex];
}

LPVOID CStdPtrArray::operator[] (int iIndex) const
{
	if (iIndex < 0 || iIndex >= m_nCount) return NULL;
	return m_ppVoid[iIndex];
}

void  CStdPtrArray::Swap(int nStart, int nEnd)
{
	LPVOID  lptmp = m_ppVoid[nStart];
	m_ppVoid[nStart] = m_ppVoid[nEnd];
	m_ppVoid[nEnd] = lptmp;
}

void CStdPtrArray::Swap(int nStart1, int nEnd1, int nStart2, int nEnd2)
{
	if (nStart1 >= nEnd2)
	{
		int tmp = nStart1;
		nStart1 = nStart2;
		nStart2 = tmp;
		tmp = nEnd1;
		nEnd1 = nEnd2;
		nEnd2 = tmp;
	}
	if (nEnd1 >= nStart2)
		return;

	int nCount1 = nEnd1 - nStart1 + 1;
	int nCount2 = nEnd2 - nStart2 + 1;
	if (nCount1 >= nCount2)
	{
		LPVOID lpTmp = static_cast<LPVOID*>(malloc(nCount1 * sizeof(LPVOID)));
		::CopyMemory(lpTmp, m_ppVoid + nStart1, nCount1 * sizeof(LPVOID));
		::CopyMemory(m_ppVoid + nStart1 + nCount2, m_ppVoid + nEnd1 + 1, (nStart2 - nEnd1 - 1) * sizeof(LPVOID));
		::CopyMemory(m_ppVoid + nStart1, m_ppVoid + nStart2, nCount2 * sizeof(LPVOID));
		::CopyMemory(m_ppVoid + nEnd2 - nCount1 + 1, lpTmp, nCount1 * sizeof(LPVOID));
		free(lpTmp);
	}
	else
	{
		LPVOID lpTmp = static_cast<LPVOID*>(malloc(nCount2 * sizeof(LPVOID)));
		::CopyMemory(lpTmp, m_ppVoid + nStart2, nCount2 * sizeof(LPVOID));
		::CopyMemory(m_ppVoid + nEnd1 + 1 + nCount2 - nCount1, m_ppVoid + nEnd1 + 1, (nStart2 - nEnd1 - 1) * sizeof(LPVOID));
		::CopyMemory(m_ppVoid + nStart2 + nCount2 - nCount1, m_ppVoid + nStart1, nCount1 * sizeof(LPVOID));
		::CopyMemory(m_ppVoid + nStart1, lpTmp, nCount2 * sizeof(LPVOID));
		free(lpTmp);
	}
}

bool CStdPtrArray::Move(int nStart, int nEnd, int nBoundryTo)
{
	if (nBoundryTo > m_nCount)
		nBoundryTo = m_nCount;
	if (nBoundryTo < 0)
		nBoundryTo = 0;
	if (nBoundryTo >= nStart && nBoundryTo <= nEnd)
		return false;
	if (nStart < 0 || nStart >= m_nCount)
		return false;
	if (nEnd < 0 || nEnd >= m_nCount)
		return false;
	int nCount = nEnd - nStart + 1;
	LPVOID lpTmp = static_cast<LPVOID*>(malloc(nCount * sizeof(LPVOID)));
	::CopyMemory(lpTmp, m_ppVoid + nStart, nCount * sizeof(LPVOID));
	if (nBoundryTo > nEnd)
	{
		::CopyMemory(m_ppVoid + nStart, m_ppVoid + nEnd + 1, (nBoundryTo - nEnd) * sizeof(LPVOID));
		::CopyMemory(m_ppVoid + nBoundryTo - nCount + 1, lpTmp, nCount * sizeof(LPVOID));
	}
	else if (nBoundryTo < nStart)
	{
		::CopyMemory(m_ppVoid + nBoundryTo + nCount, m_ppVoid + nBoundryTo, (nStart - nBoundryTo) * sizeof(LPVOID));
		::CopyMemory(m_ppVoid + nBoundryTo, lpTmp, nCount * sizeof(LPVOID));
	}
	free(lpTmp);
	return true;
}

void CStdPtrArray::Sort(array_compare_t cmp, int nBegin /*= 0*/, int nSize /*= -1*/)
{
	if (nSize < 0)
		nSize = GetSize();
	if (nSize > GetSize())
		nSize = GetSize();
	if (nSize > GetSize() - nBegin)
		nSize = GetSize() - nBegin;
	stable_sort(m_ppVoid + nBegin, m_ppVoid + (nBegin + nSize), cmp);
}
/////////////////////////////////////////////////////////////////////////////////////////
// // ANSIתΪUnicode
wstring Charset::AnsiToUnicode(const char* src)
{
	if (src == NULL || src[0] == '\0')
	{
		return L"";
	}
	string strSrc(src);
	int length = MultiByteToWideChar(CP_ACP, 0, strSrc.c_str(), -1, NULL, 0);
	wchar_t *buf = new wchar_t[length + 1];
	MultiByteToWideChar(CP_ACP, 0, strSrc.c_str(), -1, buf, length);
	buf[length] = L'\0';
	wstring dest = buf;
	delete[] buf;
	return dest;
}

// ANSIתΪUTF8
string Charset::AnsiToUTF8(const char* src)
{
	wstring unicode = AnsiToUnicode(src);
	return UnicodeToUTF8(unicode.c_str());
}

// UTF8תΪUNICODE
wstring Charset::UTF8ToUnicode(const char* src)
{
	wstring dest;
	int length = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
	if (length <= 0)
	{
		return dest;
	}

	dest.resize(length - 1);
	MultiByteToWideChar(CP_UTF8, 0, src, -1, &dest[0], length);

	return dest;
}
// UNICODEתΪUTF8
string Charset::UnicodeToUTF8(const wchar_t* src)
{
	string dest;
	int length = WideCharToMultiByte(CP_UTF8, 0, src,
		-1, NULL, 0, NULL, FALSE);
	if (length <= 0)
	{
		return dest;
	}
	
	dest.resize(length - 1);
	WideCharToMultiByte(CP_UTF8, 0, src, -1, &dest[0], length, NULL, FALSE);

	return dest;
}

tstring Charset::UTF8ToNative(const char* src)
{
#ifdef UNICODE	
	return UTF8ToUnicode(src);
#else
	return UTF8ToAnsi(src);
#endif
}

string  Charset::NativeToUTF8(LPCTSTR src)
{
#ifdef UNICODE	
	return UnicodeToUTF8(src);
#else
	return AnsiToUTF8(src);
#endif
}

wstring  Charset::TCHARToUnicode(LPCTSTR src)
{
	if (src == NULL || src[0] == '\0')
	{
		return L"";
	}
#ifdef UNICODE	
	return src;
#else
	return AnsiToUnicode(src);
#endif
}

string Charset::UnicodeTochar(const wchar_t*src)
{
	if (!src)
		return "";

	int unicode_len = wcslen(src);
	int charcount = ::WideCharToMultiByte(CP_ACP, 0, src, unicode_len, NULL, 0, NULL, NULL);
	if (charcount == 0)
		return "";

	string mb;
	mb.resize(charcount);
	::WideCharToMultiByte(CP_ACP, 0, src, unicode_len, &mb[0], charcount, NULL, NULL);

	return mb;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////

double TweenAlgorithm::Tween(TweenType type, double currentTime, double beginValue, double changeValue, double duration, TweenEaseType iTweenEaseType /*= TweenEasein*/, double s /*= 1.70158*/)
{
	double retValue;
	switch (type)
	{
	case TweenLinear:
		retValue = Linear(currentTime, beginValue, changeValue, duration);
		break;
	case TweenQuadratic:
		retValue = Quad(currentTime, beginValue, changeValue, duration, iTweenEaseType);
		break;
	case TweenCubic:
		retValue = Cubic(currentTime, beginValue, changeValue, duration, iTweenEaseType);
		break;
	case TweenQuartic:
		retValue = Quart(currentTime, beginValue, changeValue, duration, iTweenEaseType);
		break;
	case TweenQuintic:
		retValue = Quint(currentTime, beginValue, changeValue, duration, iTweenEaseType);
		break;
	case TweenSinusoidal:
		retValue = Sine(currentTime, beginValue, changeValue, duration, iTweenEaseType);
		break;
	case TweenExponential:
		retValue = Expo(currentTime, beginValue, changeValue, duration, iTweenEaseType);
		break;
	case TweenCircular:
		retValue = Circ(currentTime, beginValue, changeValue, duration, iTweenEaseType);
		break;
	case TweenElastic:
		retValue = Elastic(currentTime, beginValue, changeValue, duration, iTweenEaseType);
		break;
	case TweenBack:
		retValue = Back(currentTime, beginValue, changeValue, duration, iTweenEaseType, s);
		break;
	case TweenBounce:
		retValue = Bounce(currentTime, beginValue, changeValue, duration, iTweenEaseType);
		break;
	default:
		retValue = Quad(currentTime, beginValue, changeValue, duration, iTweenEaseType);
		break;
	}
	return retValue;
}

double TweenAlgorithm::Linear(double t, double b, double c, double d)
{
	return c*t / d + b;
}

double TweenAlgorithm::Quad(double t, double b, double c, double d, int iEaseType)
{
	switch (iEaseType)
	{
	case TweenEasein:
		return c*(t /= d)*t + b;
		break;
	case TweenEaseout:
		return -c *(t /= d)*(t - 2) + b;
		break;
	case TweenEaseinout:
		if ((t /= d / 2) < 1) return ((c / 2)*(t*t)) + b;
		return -c / 2 * (((t - 2)*(--t)) - 1) + b;
		break;
	default:
		return c*(t /= d)*t + b;
		break;
	}
}

double TweenAlgorithm::Cubic(double t, double b, double c, double d, int iEaseType)
{
	switch (iEaseType)
	{
	case TweenEasein:
		return c*(t /= d)*t*t + b;
		break;
	case TweenEaseout:
		return c*((t = t / d - 1)*t*t + 1) + b;
		break;
	case TweenEaseinout:
		if ((t /= d / 2) < 1) return c / 2 * t*t*t + b;
		return c / 2 * ((t -= 2)*t*t + 2) + b;
		break;
	default:
		return c*(t /= d)*t*t + b;
		break;
	}
}

double TweenAlgorithm::Quart(double t, double b, double c, double d, int iEaseType)
{
	switch (iEaseType)
	{
	case TweenEasein:
		return c*(t /= d)*t*t*t + b;
		break;
	case TweenEaseout:
		return -c * ((t = t / d - 1)*t*t*t - 1) + b;
		break;
	case TweenEaseinout:
		if ((t /= d / 2) < 1) return c / 2 * t*t*t*t + b;
		return -c / 2 * ((t -= 2)*t*t*t - 2) + b;
		break;
	default:
		return c*(t /= d)*t*t*t + b;
		break;
	}
}

double TweenAlgorithm::Quint(double t, double b, double c, double d, int iEaseType)
{
	switch (iEaseType)
	{
	case TweenEasein:
		return c*(t /= d)*t*t*t*t + b;
		break;
	case TweenEaseout:
		return c*((t = t / d - 1)*t*t*t*t + 1) + b;
		break;
	case TweenEaseinout:
		if ((t /= d / 2) < 1) return c / 2 * t*t*t*t*t + b;
		return c / 2 * ((t -= 2)*t*t*t*t + 2) + b;
		break;
	default:
		return c*(t /= d)*t*t*t*t + b;
		break;
	}
}

double TweenAlgorithm::Sine(double t, double b, double c, double d, int iEaseType)
{
	switch (iEaseType)
	{
	case TweenEasein:
		return -c * cos(t / d * (M_PI / 2)) + c + b;
		break;
	case TweenEaseout:
		return c * sin(t / d * (M_PI / 2)) + b;
		break;
	case TweenEaseinout:
		return -c / 2 * (cos(M_PI*t / d) - 1) + b;
		break;
	default:
		return -c * cos(t / d * (M_PI / 2)) + c + b;
		break;
	}
}

double TweenAlgorithm::Expo(double t, double b, double c, double d, int iEaseType)
{
	switch (iEaseType)
	{
	case TweenEasein:
		return (t == 0) ? b : c * pow(2, 10 * (t / d - 1)) + b;
		break;
	case TweenEaseout:
		return (t == d) ? b + c : c * (-pow(2, -10 * t / d) + 1) + b;
		break;
	case TweenEaseinout:
		if (t == 0) return b;
		if (t == d) return b + c;
		if ((t /= d / 2) < 1) return c / 2 * pow(2, 10 * (t - 1)) + b;
		return c / 2 * (-pow(2, -10 * --t) + 2) + b;
		break;
	default:
		return (t == 0) ? b : c * pow(2, 10 * (t / d - 1)) + b;
		break;
	}
}

double TweenAlgorithm::Circ(double t, double b, double c, double d, int iEaseType)
{
	switch (iEaseType)
	{
	case TweenEasein:
		return -c * (sqrt(1 - (t /= d)*t) - 1) + b;
		break;
	case TweenEaseout:
		return c * sqrt(1 - (t = t / d - 1)*t) + b;
		break;
	case TweenEaseinout:
		if ((t /= d / 2) < 1) return -c / 2 * (sqrt(1 - t*t) - 1) + b;
		return c / 2 * (sqrt(1 - t*(t -= 2)) + 1) + b;
		break;
	default:
		return -c * (sqrt(1 - (t /= d)*t) - 1) + b;
		break;
	}
}

double TweenAlgorithm::Elastic(double t, double b, double c, double d, int iEaseType)
{
	switch (iEaseType)
	{
	case TweenEasein:
	{
		if (t == 0) return b;  if ((t /= d) == 1) return b + c;
		float p = d*.3f;
		float a = c;
		float s = p / 4;
		float postFix = a*pow(2, 10 * (t -= 1)); // this is a fix, again, with post-increment operators
		return -(postFix * sin((t*d - s)*(2 * M_PI) / p)) + b;
	}
	break;
	case TweenEaseout:
	{
		if (t == 0) return b;  if ((t /= d) == 1) return b + c;
		float p = d*.3f;
		float a = c;
		float s = p / 4;
		return (a*pow(2, -10 * t) * sin((t*d - s)*(2 * M_PI) / p) + c + b);
	}
	break;
	case TweenEaseinout:
	{
		if (t == 0) return b;  if ((t /= d / 2) == 2) return b + c;
		float p = d*(.3f*1.5f);
		float a = c;
		float s = p / 4;

		if (t < 1) {
			float postFix = a*pow(2, 10 * (t -= 1)); // postIncrement is evil
			return -.5f*(postFix* sin((t*d - s)*(2 * M_PI) / p)) + b;
		}
		float postFix = a*pow(2, -10 * (t -= 1)); // postIncrement is evil
		return postFix * sin((t*d - s)*(2 * M_PI) / p)*.5f + c + b;
	}
	break;
	default:
	{
		if (t == 0) return b;  if ((t /= d) == 1) return b + c;
		float p = d*.3f;
		float a = c;
		float s = p / 4;
		float postFix = a*pow(2, 10 * (t -= 1)); // this is a fix, again, with post-increment operators
		return -(postFix * sin((t*d - s)*(2 * M_PI) / p)) + b;
	}
	break;
	}
}

double TweenAlgorithm::Back(double t, double b, double c, double d, int iEaseType /*=0*/, double s /*=1.70158 */)
{
	switch (iEaseType)
	{
	case TweenEasein:
	{
		float postFix = t /= d;
		return c*(postFix)*t*((s + 1)*t - s) + b;
	}
	break;
	case TweenEaseout:
	{return c*((t = t / d - 1)*t*((s + 1)*t + s) + 1) + b; }
	break;
	case TweenEaseinout:
	{
		if ((t /= d / 2) < 1) return c / 2 * (t*t*(((s *= (1.525f)) + 1)*t - s)) + b;
		float postFix = t -= 2;
		return c / 2 * ((postFix)*t*(((s *= (1.525f)) + 1)*t + s) + 2) + b;
	}
	break;
	default:
	{
		float postFix = t /= d;
		return c*(postFix)*t*((s + 1)*t - s) + b;
	}
	break;
	}
	return 0;
}

double BounceEaseOut(double t, double b, double c, double d)
{
	if ((t /= d) < (1 / 2.75f)) {
		return c*(7.5625f*t*t) + b;
	}
	else if (t < (2 / 2.75f)) {
		float postFix = t -= (1.5f / 2.75f);
		return c*(7.5625f*(postFix)*t + .75f) + b;
	}
	else if (t < (2.5 / 2.75)) {
		float postFix = t -= (2.25f / 2.75f);
		return c*(7.5625f*(postFix)*t + .9375f) + b;
	}
	else {
		float postFix = t -= (2.625f / 2.75f);
		return c*(7.5625f*(postFix)*t + .984375f) + b;
	}
}

double BounceEaseIn(double t, double b, double c, double d)
{
	return c - BounceEaseOut(d - t, 0, c, d) + b;
}

double TweenAlgorithm::Bounce(double t, double b, double c, double d, int iEaseType)
{
	switch (iEaseType)
	{
	case TweenEasein:
		return BounceEaseIn(t, b, c, d);
		break;
	case TweenEaseout:
		return BounceEaseOut(t, b, c, d);
		break;
	case TweenEaseinout:
		if (t < d / 2) return BounceEaseIn(t * 2, 0, c, d) * .5f + b;
		else return BounceEaseOut(t * 2 - d, 0, c, d) * .5f + c*.5f + b;
		break;
	default:
		return BounceEaseIn(t, b, c, d);
		break;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
tstring StrUtil::Replace(tstring &str, const TCHAR* string_to_replace, const TCHAR* new_string)
{
	//   Find   the   first   string   to   replace   
	int index = str.find(string_to_replace);
	//   while   there   is   one   
	while (index != tstring::npos)
	{
		//   Replace   it   
		str.replace(index, _tcslen(string_to_replace), new_string);
		//   Find   the   next   one   
		index = str.find(string_to_replace, index + _tcslen(new_string));
	}
	return   str;
}

void StrUtil::ToLowerCase(tstring& str)
{
	std::transform(
		str.begin(),
		str.end(),
		str.begin(),
		tolower);
}

void  StrUtil::ToUpperCase(tstring& str)
{
	std::transform(
		str.begin(),
		str.end(),
		str.begin(),
		toupper);
}

void  StrUtil::Trim(tstring& str, bool left, bool right)
{
	static const tstring delims = _T(" \t\r");
	if (right)
		str.erase(str.find_last_not_of(delims) + 1);
	if (left)
		str.erase(0, str.find_first_not_of(delims));
}


vector<tstring> StrUtil::Split(const tstring& str, const tstring& delims, unsigned int maxSplits)
{
	vector<tstring> ret;
	unsigned int numSplits = 0;

	// Use STL methods 
	size_t start, pos;
	start = 0;
	do
	{
		pos = str.find_first_of(delims, start);
		if (pos == start)
		{
			// Do nothing
			start = pos + 1;
		}
		else if (pos == tstring::npos || (maxSplits && numSplits == maxSplits))
		{
			// Copy the rest of the tstring
			ret.push_back(str.substr(start));
			break;
		}
		else
		{
			// Copy up to delimiter
			ret.push_back(str.substr(start, pos - start));
			start = pos + 1;
		}
		// parse up to next real data
		start = str.find_first_not_of(delims, start);
		++numSplits;

	} while (pos != tstring::npos);
	return ret;
}

tstring StrUtil::ConvertBSTR(BSTR bstr)
{
	_bstr_t bstrText(bstr);
	return (LPCTSTR)(bstrText);
}

bool StrUtil::IsMailFormat(const tstring& str)
{
	tstring::size_type nLength = str.length();
	if (nLength == 0)
		return false;

	tstring::size_type nBegin;
	tstring::size_type nEnd;
	//必须包含一个并且只有一个符号“@” 
	nBegin = str.find_first_of('@');
	if (nBegin == tstring::npos)
		return false;
	else
	{
		//不允许出现“@.”或者.@ “＋@”
		if (nBegin > 0 && str[nBegin - 1] == '.')
			return false;
		if (nBegin > 0 && str[nBegin - 1] == '+')
			return false;
		if (/*nBegin >= 0 && */nBegin < nLength - 1 && str[nBegin + 1] == '.')
			return false;

		nEnd = str.find('.', nBegin);
		if (nEnd == tstring::npos)
			return false;

		nEnd = str.find('+', nBegin);
		if (nEnd != tstring::npos)
			return false;

		nEnd = str.find('@', nBegin + 1);
		if (nEnd != tstring::npos)
			return false;
	}

	//第一个字符不得是“@”或者“.”
	//不允许“＋”在最前面，
	if (str[0] == '@' || str[0] == '.' || str[0] == '+')
		return false;
	//结尾不得是字符“@”或者“.” 
	if (str[nLength - 1] == '@' || str[nLength - 1] == '.')
		return false;

	return true;
}

bool StrUtil::IsNumberFormat(const tstring& str)
{
	tstring strNumber = _T("0123456789");
	int nLength = str.length();

	for (int nPos = 0; nPos < nLength; nPos++)
	{
		if (strNumber.find_first_of(str[nPos]) == tstring::npos)
		{
			return false;
		}
	}
	return true;
}

bool StrUtil::IsLetterFormat(const tstring& str)
{
	TCHAR chA = 'A';
	TCHAR chZ = 'Z';
	TCHAR cha = 'a';
	TCHAR chz = 'z';
	//TCHAR ch;
	int nLength = str.length();
	for (int i = 0; i < nLength; i++)
	{
		TCHAR ch = str[i];
		if (!(ch >= chA && ch <= chZ || ch >= cha && ch <= chz))
		{
			return false;
		}
	}
	return true;
}

bool StrUtil::IsChineseFormat(const tstring& str)
{
	int nLength = str.length();
	for (int i = 0; i < nLength; i++)
	{
		TCHAR ch = str[i];
		if ((unsigned short)ch < 0x0391 || (unsigned short)ch > 0xFFE5)
		{
			return false;
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class Point2D
{
public:
	Point2D(float xx = 0, float yy = 0) : x(xx), y(yy) {}
	Point2D scale(float t) { return Point2D(x * t, y * t); }
	void add(const Point2D &other){ x += other.x; y += other.y; }
public:
	float x;
	float y;
};

class CurveImpl
{
public:
	typedef std::vector<Point2D> Point2DVector;

	CurveImpl()
	{

	}
	~CurveImpl()
	{
		Release();
	}
	void Release()
	{
		m_vecPoint.clear();
	}
	void AddPoint(float tp, float sp)
	{
		m_vecPoint.push_back(Point2D(tp, sp));
	}
	Point2DVector m_vecPoint;
};

CurveObj::CurveObj()
{
	m_pCurveImpl = new CurveImpl;
}

CurveObj::~CurveObj()
{
	if (m_pCurveImpl)
		delete m_pCurveImpl;
	m_pCurveImpl = NULL;
}

void CurveObj::AddPoint(float tp, float sp)
{
	if (m_pCurveImpl)
	{
		m_pCurveImpl->AddPoint(tp, sp);
	}
}

void CurveObj::Reset()
{
	if (m_pCurveImpl)
	{
		m_pCurveImpl->Release();
	}
}

void CalcCurve(CurveImpl::Point2DVector& vec, float currentTime, float totalTime)
{
	int n = vec.size() - 1;
	float u = currentTime / totalTime;
	for (int i = n; i >= 1; i--)
	{
		for (int j = 0; j < n; j++)
		{
			vec[j] = vec[j].scale(1 - u);
			vec[j].add(vec[j + 1].scale(u));
			//vec[j] = vec[j].scale(1-u) + vec[j+1].scale(u);
			//B(t) = P0 * (1 -t) + P1 * t;
		}
	}
}

long BezierAlgorithm::BezierValue(CurveObj* curve, long currentTime, long beginValue, long changeValue, long totalTime)
{
	if (curve && curve->m_pCurveImpl)
	{
		CurveImpl::Point2DVector temp = curve->m_pCurveImpl->m_vecPoint;
		CalcCurve(temp, currentTime, totalTime);
		if (temp.size() > 1)
			return beginValue + (long)(temp[0].y * changeValue);
	}
	return beginValue;
}

POINT BezierAlgorithm::BezierPoint(CurveObj* curve, long currentTime, const POINT&  beginPoint, const POINT&  destPoint, long totalTime)
{
	if (curve && curve->m_pCurveImpl)
	{
		CurveImpl::Point2DVector temp = curve->m_pCurveImpl->m_vecPoint;
		CalcCurve(temp, currentTime, totalTime);
		if (temp.size() > 1)
		{
			POINT pt;
			pt.x = beginPoint.x + temp[0].x * (destPoint.x - beginPoint.x);
			pt.y = beginPoint.y + temp[0].y * (destPoint.y - beginPoint.y);
			return pt;
		}
	}
	return beginPoint;
}

RECT BezierAlgorithm::BezierRect(CurveObj* curve, long currentTime, const RECT& beginRect, const RECT& destRect, long totalTime)
{
	if (curve && curve->m_pCurveImpl)
	{
		CurveImpl::Point2DVector temp = curve->m_pCurveImpl->m_vecPoint;
		CalcCurve(temp, currentTime, totalTime);
		if (temp.size() > 1)
		{
			RECT ret;
			ret.left = beginRect.left + temp[0].x * (destRect.left - beginRect.left);
			ret.right = beginRect.right + temp[0].x * (destRect.right - beginRect.right);
			ret.top = beginRect.top + temp[0].y * (destRect.top - beginRect.top);
			ret.bottom = beginRect.bottom + temp[0].y * (destRect.bottom - beginRect.bottom);
			return ret;
		}
	}
	return beginRect;
}