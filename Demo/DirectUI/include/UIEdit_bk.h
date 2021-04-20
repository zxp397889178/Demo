/*********************************************************************
*            Copyright (C) 2010, �����������Ӧ�������
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
�˶��б༭��ʵ�ֵ��Զ����й��ܣ��Լ�һЩ��Ҫ�Ĳ����縴������϶��ȵȹ���
һ���ڶ��б༭��ؼ��У��κ�һ���ַ���3�ַ�����ʾ������λ��:
1.  ��λ�ã� ��λ�� ��CMultiLineEditUI����CharPos�ṹ���ʾ��
2.  ���ַ�����������
3.  ʵ���ڱ༭���е�λ�á�
������Ա����˵��:
1.  CharPos m_cpSelBegin;               
	CharPos m_cpSelEnd;
����������ʾ��ǰѡ���ı��Ŀ�ʼλ�úͽ���λ����CharPos�ṹ���ʾ��Ĭ�ϵ�,��δ�϶��ı�ʱm_cpSelBegin �� m_cpSelEnd����ȵ�
��m_cpSelEndҲ��ʾ��ǰ����λ��, ��Ҫ�޸Ĺ���λ��ֻ��������ֵ����.  �ر�ע��
2. CharPos m_cpPrevSelEnd;
��ʾ�ϴ�ѡ���Ľ���λ�� (����˱�����Ϊ���϶��ı�ѡ��ʱ����ֹ������˸����)
3. CLineInfoArray m_lineInfoArray;
����CLineInfoArray����vector<LPLINEINFO>�����:
typedef vector<LPLINEINFO>  CLineInfoArray;
typedef LINEINFO *LPLINEINFO;
struct LINEINFO   //��¼ÿһ����Ϣ�Ľṹ��
{
	unsigned CharsCount;  //һ�ж��ٸ��ַ�
	MyBool bParaEnd;    //�Ƿ��лس�
	LINEINFO(unsigned nCount = 0, MyBool bFlag = MYFALSE)  //Ĭ�Ϲ��캯��
	{
	CharsCount = nCount;
	bParaEnd = bFlag;
	}
};
(1).�˳�Ա����������Ҫ, ��¼��ÿ���ַ�����,�Ƿ��лس�. ���ж��Ƿ��лس���Ҳ����һ���ж��ٸ��ַ�(�����2���ַ�)��
(2).ֻҪ�ж��ı��Ĳ���ɾ���������ᵼ�´˱�����Ԫ�ص�����
4. int m_nFirstVisibleLine;
�����ı����࣬����Ҳ�������࣬�˱�����ʾ���� ���ı����������ı�����ʱ����ʾ��һ�пɼ��е��кš�
���µ���m_nFirstVisiblePixel��С����ʹ���λ�ÿɼ�
������Ա����˵����������ע�͡�
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
	unsigned  LineIndex;		//�к�
	unsigned  CharIndex;		//�к�
}CharPos;

struct LINEINFO
{
	unsigned CharsCount;		//һ�ж��ٸ��ַ�
	MyBool bParaEnd;			//�Ƿ��лس�
	LINEINFO(unsigned nCount = 0, MyBool bFlag = MYFALSE)
	{
		CharsCount = nCount;
		bParaEnd = bFlag;
	}
};
typedef LINEINFO *LPLINEINFO;

enum ADJUSTMODE
{
	INSERT_ONLY,				//��ɾ����ֱ��׷��
	DEL_AND_INSERT				//ɾ����׷��
};

class MultiLineEditDataImpl;

/*!
    \brief    ���б༭��ؼ�
	\note     ���߱�������
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
	void AdjustAllLineInfo();//���ã�����m_lineInfoArrayԪ����Ϣ

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
	int	m_nLineHeight;  //ÿ�еĸ߶�
	int	m_nAveCharWidth; //�ַ���ƽ����Ȩ���
	int	m_nMaxchar;   //������������ַ�����
	int	m_nMaxUTF8Char;
	int	m_nCurUTF8Char;
	CharPos	m_cpSelBegin;
	CharPos	m_cpSelEnd;
	CharPos	m_cpPrevSelEnd;
	unsigned m_uClientWidth;//��ǰ�༭��Ŀ��
	unsigned m_uClientHeight;//��ǰ�༭��ĸ߶�
	MultiLineEditDataImpl* m_pEditDataImpl;

	bool m_bReadOnly;
	bool m_bKillFocus;
	int m_nTransparent;  //�Ƿ񱳾�͸�� 	//0 ��͸�� 1 ͸�� 2 ʧ��ʱ͸��
	bool m_bAutoSize;  //�Ƿ��Ǵ�С�ɱ�ı༭��
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
��Ϊ���б༭��
һ���ڵ��б༭��ؼ��У��κ�һ���ַ���2�ַ�����ʾ������λ��:
1. ʵ���ڱ༭���е�λ��
2. ���ַ����ַ����е�����λ��
������Ա����˵��
1. 	unsigned m_uSelBegin;
	unsigned m_uSelEnd;
ͬ���б༭��һ��Ҳ������ѡ����Χ���������������ͺ����ʾ�����ַ����е�������
m_uSelEndҲ�п��ƹ��λ������
2.	unsigned m_uPrevSelEnd;
	int m_nPrevSelEndPixel;
��ʾ�ϴ�ѡ��ĩβ�ַ�����λ�ú��ϴ�ѡ��ĩβ�ַ�������λ�ã����ô�2���������ڶԱ༭��Ч���Ż�:
���ڼ���ÿ���ַ�������λ��ͨ�����Ǵӵ�һ���ַ���ռ���صĴ�Сһֱ�ۼӵ���ָ���ַ���λ�á�����
���ı�������ݹ��࣬����Ƶ���任�ᵼ�¼���Ŀ������������2������ÿ�μ��㶼�Ǵ��ϴι������
��λ�ý��п�ʼ���㡣
3. int m_nFirstVisiblePixel;
��ʾ��һ���ɼ����ص�λ�ã�ע��������б�ʾ�ĵ�һ���ɼ��к�������ͬ��
*/
//////////////////////////////////////////////////////////////////////////
/*!
    \brief    ���б༭��ؼ�
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

	void SetPrevSelEndInfo();//���Թ����ж�λʱ���� ����m_uPrevSelEnd; m_nPrevSelEndPixel;����
	BOOL EnsureSelEndVisible();//���µ���m_nFirstVisiblePixel��С����ʹ���λ�ÿɼ�
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

	void SetPrevSelEndInfo(int offsetPixels); //�����в������ɾ��ʱ, ��m_uPrevSelEnd; m_nPrevSelEndPixel;���ô��ݵĲ����ǵ���insert��DelSelRange�ķ���ֵ��
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
	int m_nTextPixel;//�ܵ��ı��ַ���ռ�����ش�С֮��
	int m_nFirstVisiblePixel;
	int m_nPrevSelEndPixel;

	TCHAR m_tcPassChar;//��������������������ַ�
	tstring m_strPasswdText;//������Ĥ���ַ�����������������һֱ���������ŵ���m_tcPassChar�ַ�
	int m_nPasswdCharWidth;

	HWND m_hParentWnd;
	HWND m_hToolTipWnd;
	TextStyle* m_pTextStyle;
	TextStyle* m_pEmptyTextStyle;
	tstring m_strEmptyTipsText;
};

#endif 

