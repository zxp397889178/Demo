#include <afxwin.h>

class MyMFC : public CWinApp
{
public:
	virtual BOOL InitInstance();

};

class MyFrame : public CFrameWnd  //���ڿ����
{
public:
	MyFrame();

	//������
	DECLARE_MESSAGE_MAP();

private:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnPaint();
};