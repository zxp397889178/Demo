/*********************************************************************
*            Copyright (C) 2010, 网龙天晴程序应用软件部
**********************************************************************
*   Date        Name        Description
*   9/17/2010   hanzp		Create.
*   9/20/2010   hanzp		Add Class CEditUI.
*   10/27/2010  hanzp		Add MultiLineEdit
*********************************************************************/
#pragma once

//{{
//#ifndef 1DIRECTUI_LAYOUT_RTL
#if 1
//}}

///////////////////////////////////////////////////////////
/*
此多行编辑框实现的自动换行功能，以及一些必要的操作如复制黏贴拖动等等功能
一、在多行编辑框控件中，任何一个字符有3种方法表示其所在位置:
1.  行位置， 列位置 在CMultiLineEditUI定义CharPos结构体表示。
2.  在字符串中索引号
3.  实际在编辑框中的位置。
二、成员变量说明:
1.  CharPos m_cpSelBegin;               
	CharPos m_cpSelEnd;
两个变量表示当前选定文本的开始位置和结束位置用CharPos结构体表示。默认的,你未拖动文本时m_cpSelBegin 与 m_cpSelEnd是相等的
且m_cpSelEnd也表示当前光标的位置, 若要修改光标的位置只需更改这个值即可.  特别注意
2. CharPos m_cpPrevSelEnd;
表示上次选定的结束位置 (定义此变量是为了拖动文本选定时，防止出现闪烁问题)
3. CLineInfoArray m_lineInfoArray;
其中CLineInfoArray是由vector<LPLINEINFO>定义的:
typedef vector<LPLINEINFO>  CLineInfoArray;
typedef LINEINFO *LPLINEINFO;
struct LINEINFO   //记录每一行信息的结构体
{
	unsigned CharsCount;  //一行多少个字符
	MyBool bParaEnd;    //是否有回车
	LINEINFO(unsigned nCount = 0, MyBool bFlag = MYFALSE)  //默认构造函数
	{
	CharsCount = nCount;
	bParaEnd = bFlag;
	}
};
(1).此成员变量及其重要, 记录着每行字符个数,是否有回车. 而判断是否有回车，也计入一行有多少个字符(有则加2个字符)。
(2).只要有对文本的插入删除操作都会导致此变量里元素的重设
4. int m_nFirstVisibleLine;
随着文本增多，行数也随着增多，此变量表示的是 当文本行数超出文本区域时，显示第一行可见行的行号。
重新调整m_nFirstVisiblePixel大小，以使光标位置可见
三、成员函数说明，见类中注释。
*/
///////////////////////////////////////////////////////////
//{{
typedef	  BYTE	MyBool;
#ifdef    UNICODE
#define	  MYTRUE	2
#define	  MYFALSE	0
#else
#define	  MYBOOL	BYTE
#define	  MYTRUE	1
#define	  MYFALSE	0
#endif
//}}

typedef struct tagCHARPOS
{
	unsigned  LineIndex;		//行号
	unsigned  CharIndex;		//列号
}CharPos;

struct LINEINFO
{
	unsigned CharsCount;		//一行多少个字符
	MyBool bParaEnd;			//是否有回车
	LINEINFO(unsigned nCount = 0, MyBool bFlag = MYFALSE)
	{
		CharsCount = nCount;
		bParaEnd = bFlag;
	}
};
typedef LINEINFO *LPLINEINFO;

enum ADJUSTMODE
{
	INSERT_ONLY,				//不删除，直接追加
	DEL_AND_INSERT				//删除并追加
};

class MultiLineEditDataImpl;

/*!
    \brief    多行编辑框控件
	\note     不具备滚动条
*****************************************/
class DUI_API CMultiLineEditUI:public CLabelUI, public INotifyUI
{
	UI_OBJECT_DECLARE(CMultiLineEditUI, _T("MultiEdit"))
public:
	CMultiLineEditUI();
	virtual ~CMultiLineEditUI();

	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	void SetText(LPCTSTR lpszText);
	void SetTextColor(DWORD dwClrText);
	void SetFontName(LPCTSTR lpszFontName);
	void SetFontSize(int nFontSize);
	void SetBold(bool bBold);
	void SetItalic(bool bItalic);
	void SetUnderline(bool bUnderline);
	void SetAlign(UINT uAlignStyle);
	bool IsKillFocus();
	bool IsReadOnly(){return m_bReadOnly;};
	void SetReadOnly(bool bReadOnly);
	void SetClientWidth(unsigned width);
	void SetFirstVisibleLine(unsigned uLineNum);

protected:
	UIBEGIN_MSG_MAP
		UI_EVENT_MENU_ID_HANDLER(UINOTIFY_CLICK, IDR_MENU_UIEDIT, OnMenuItemClick)
	UIEND_MSG_MAP

	bool OnMenuItemClick(TNotifyUI* pNotify);

	virtual void Render(IRenderDC* pRenderDC, RECT &rcPaint);
	virtual bool Event(TEventUI& event);
	virtual void Init();
	virtual void SetRect(RECT& rectRegion);
	virtual void OnKeyDown(UINT nChar, UINT nFlags);
	virtual void OnChar(UINT nChar, UINT nFlags);
	virtual void OnLButtonDown(UINT nFlags, POINT pt);
	virtual void OnRButtonUp(UINT nFlags, POINT pt);
	virtual void OnLButtonUp(UINT nFlags, POINT pt);
	virtual void OnLButtonDblClk(UINT nFlags, POINT pt);
	virtual void OnMouseMove(UINT nFlags, POINT pt);
	virtual void OnPaste(){}

	TextStyle* GetEditStyle();
	TextStyle* GetEmptyEditStyle();

private:
	void GetSel(tstring &str);
	void Copy();
	void Paste();
	void Clear();

	void CharPos2Point(const CharPos &pos, POINT &pt);
	void Point2CharPos(const POINT &pt, CharPos &pos);   
	void CharPos2Index(const CharPos &pos, unsigned &nIndex);
	void Index2CharPos(const unsigned &nIndex, CharPos &pos);
	
	void LeftSelEnd();
	void RightSelEnd();
	void UpSelEnd();
	void DownSelEnd();
	bool DelSelRange();
	void Insert(LPCTSTR lpszText);
	bool EnsureSelEndVisible();
	
	void CalcTextMetrics();
	int GetTextHeight();
	void SetStyleObjRect(const RECT& rc);

	RECT GetSelectReDrawRect();
	RECT GetInsertReDrawRect(const CharPos &beforeChangePos, const CharPos &afterChangePos, const bool bAddLine);
	RECT GetDeleteReDrawRect(const CharPos &beforeChangePos, const CharPos &afterChangePos, const bool bSubLine);
	void RemoveLineInfo(unsigned nStartLine, unsigned nCount);
	void RemoveLineInfo(unsigned nStartLine = 0);
	void AdjustFromLineToParaEnd(const unsigned LineNum, const ADJUSTMODE mode = DEL_AND_INSERT);
	void AdjustAllLineInfo();//慎用，重设m_lineInfoArray元素信息

	int FindPrevCrlfLine(unsigned nStartLine);
	unsigned FindNextCrlfLine(unsigned nStartLine);
	int CutStringForOneLine(HDC hDC, const TCHAR *lpszText);

	void TextOut(IRenderDC* pRenderDC, int x, int y, LPCTSTR lpszText, int c);
	void DrawRange(IRenderDC* pRenderDC, const CharPos &pos1, const CharPos &pos2);
	void InvalidateRect(LPRECT lpRect, bool bFlag = false);
	
protected:
	void CreateCaret();
	void SetCaretPos();
	void DestroyCaret();

	bool m_bInit;
	int	m_nFirstVisibleLine;
	int	m_nLineHeight;  //每行的高度
	int	m_nAveCharWidth; //字符的平均加权宽带
	int	m_nMaxchar;   //设置最大容纳字符个数
	int	m_nMaxUTF8Char;
	int	m_nCurUTF8Char;
	CharPos	m_cpSelBegin;
	CharPos	m_cpSelEnd;
	CharPos	m_cpPrevSelEnd;
	unsigned m_uClientWidth;//当前编辑框的宽度
	unsigned m_uClientHeight;//当前编辑框的高度
	MultiLineEditDataImpl* m_pEditDataImpl;

	bool m_bReadOnly;
	bool m_bKillFocus;
	int m_nTransparent;  //是否背景透明 	//0 不透明 1 透明 2 失焦时透明
	bool m_bAutoSize;  //是否是大小可变的编辑框
	bool m_bMenuEnable;
	bool m_bhasCaret;
	BOOL m_bMaxSize;
	SIZE m_cxyMaxSize;
	RECT m_rcPrevItem;
	int	 m_nPreCaretHeight;

	TextStyle* m_pTextStyle;
	TextStyle* m_pEmptyTextStyle;
	CScrollbarUI *m_pVerticalScrollbar;
	HCURSOR m_hCursor;
	HWND m_hParentWnd;
	tstring m_strEmptyTipsText;
};

//////////////////////////////////////////////////////////////////////////
/*
此为单行编辑框
一、在单行编辑框控件中，任何一个字符有2种方法表示其所在位置:
1. 实际在编辑框中的位置
2. 该字符在字符串中的索引位置
二、成员变量说明
1. 	unsigned m_uSelBegin;
	unsigned m_uSelEnd;
同多行编辑框一样也定义有选定范围变量。不过其类型含义表示是在字符串中的索引。
m_uSelEnd也有控制光标位置作用
2.	unsigned m_uPrevSelEnd;
	int m_nPrevSelEndPixel;
表示上次选定末尾字符索引位置和上次选定末尾字符的像素位置，设置此2个变量用于对编辑框效率优化:
由于计算每个字符的像素位置通常都是从第一个字符所占像素的大小一直累加到你指定字符的位置。但是
当文本框的内容过多，光标的频繁变换会导致计算的卡死，因此设置2个变量每次计算都是从上次光标所在
的位置进行开始计算。
3. int m_nFirstVisiblePixel;
表示第一个可见像素的位置，注意这与多行表示的第一个可见行号有所不同。
*/
//////////////////////////////////////////////////////////////////////////
/*!
    \brief    单行编辑框控件
*****************************************/

class DUI_API CEditUI:public CLabelUI, public INotifyUI
{
	UI_OBJECT_DECLARE(CEditUI, _T("Edit"))
public:
	CEditUI();
	virtual ~CEditUI();
	void SetText(LPCTSTR lpszText);
	tstring GetPasswordText();
	void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
	void SetTextWithoutNotify(LPCTSTR lpszText);
	bool IsReadOnly();
	void SetReadOnly(bool bReadOnly = true);
	void SelAllText();
	bool SetSelPos(int pos = -1);

protected:
	UIBEGIN_MSG_MAP
		UI_EVENT_MENU_ID_HANDLER(UINOTIFY_CLICK, IDR_MENU_UIEDIT,  OnMenuItemClick)
	UIEND_MSG_MAP

	bool OnMenuItemClick(TNotifyUI* pNotify);

	virtual void Init();
	virtual void SetRect(RECT& rectRegion);
	virtual bool Event(TEventUI& event);
	virtual void Render(IRenderDC* pRenderDC, RECT &rcPaint);
	virtual void OnSetFocus();
	virtual void OnKillFocus();
	virtual void OnChar(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnKeyDown(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnLButtonDown(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnLButtonUp(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnLButtonDblick(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnRButtonUp(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnMouseMove(WPARAM wParam, LPARAM lParam, LPPOINT ppt = NULL);
	virtual void OnPaste();
	virtual bool OnLimitChar(UINT nChar);

	TextStyle* GetEditStyle();	
	TextStyle* GetEmptyEditStyle();
	int Point2Index(LPPOINT ppt, bool bAdjust=false);
	POINT Index2Point(int index);
	int GetTextPixel();
	int	DelSelText();
	bool IsCaretEnable();
	void SetFirstVisiblePixel(int nPixel);
	void SetPreSelEndPixel(int nPixel);

	void SetPrevSelEndInfo();//当对光标进行定位时调用 设置m_uPrevSelEnd; m_nPrevSelEndPixel;变量
	BOOL EnsureSelEndVisible();//重新调整m_nFirstVisiblePixel大小，以使光标位置可见
	RECT GetSelectReDrawRect(unsigned beforeMovePos); 

	unsigned m_uSelBegin;
	unsigned m_uSelEnd;
	unsigned m_uPrevSelEnd;

protected:
	void Copy();
	void Paste();
	void HideToolTip();
	void SetStyleObjRect(const RECT& rc);

	void CreateCaret();
	void SetCaretPos();
	void DestroyCaret();

	tstring GetSelText();
	int Insert(LPCTSTR lpszText);
	void DrawRange(IRenderDC* pRenderDC, const unsigned &index1, const unsigned &index2);

	int GetCharsPixels(HDC hDC, int nCounts);
	int GetCharsCounts(HDC hDC, int nPixels, bool bAdjust=false);
	int GetLeftClientWidthIndex(HDC hDC, int nPos);
	int GetRightClientWidthIndex(HDC hDC, int nPos);

	void SetPrevSelEndInfo(int offsetPixels); //当进行插入或者删除时, 对m_uPrevSelEnd; m_nPrevSelEndPixel;设置传递的参数是调用insert和DelSelRange的返回值。
	RECT GetInsertReDrawRect(unsigned beforeInsertPos);
	RECT GetDeleteReDrawRect();

	bool m_bReadOnly;
	bool m_bAutoSize;
	bool m_bMenuEnable;
	bool m_bCaretEnable;
	bool m_bTransparent;

	int m_nAveCharWidth;
	int m_nLineHeight;
	int m_cy;
	int m_nMaxchar;
	int m_nMaxUTF8Char;
	int m_nCurUTF8Char;
	SIZE m_cxyMaxSize;
	RECT m_rcPrevItem;
	bool m_bPassword;
	bool m_bInit;
	bool m_bMaxSize;
	int m_nTextPixel;//总的文本字符所占的像素大小之和
	int m_nFirstVisiblePixel;
	int m_nPrevSelEndPixel;

	TCHAR m_tcPassChar;//若是密码框，密码框密码的字符
	tstring m_strPasswdText;//密码掩膜，字符个数与真正的密码一直不过里面存放的是m_tcPassChar字符
	int m_nPasswdCharWidth;

	HWND m_hParentWnd;
	HWND m_hToolTipWnd;
	TextStyle* m_pTextStyle;
	TextStyle* m_pEmptyTextStyle;
	tstring m_strEmptyTipsText;
};

#endif 

