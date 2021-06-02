#include "MyMFC.h"

MyMFC myMfc;  //全局应用程序对象，有且仅有一个

BOOL MyMFC::InitInstance()
{
	//创建窗口
	MyFrame* frame = new MyFrame;

	//显示和更新
	frame->ShowWindow(SW_SHOW);
	frame->UpdateWindow();

	m_pMainWnd = frame;  //保存指向应用程序的主窗口的指针

	return TRUE;
}


//分界宏
BEGIN_MESSAGE_MAP(MyFrame, CFrameWnd)
	ON_WM_LBUTTONDOWN()  
	ON_WM_CHAR()
	ON_WM_PAINT()
END_MESSAGE_MAP()

MyFrame::MyFrame()
{
	Create(NULL, TEXT("mfc"));
}

void MyFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	/*TCHAR buf[256];
	wsprintf(buf, TEXT("x = %d, y = %d"), point.x, point.y);

	MessageBox(buf);*/

	//MFC中的字符串 CString
	CString str;
	str.Format(TEXT("x = %d, y = %d"), point.x, point.y);

	MessageBox(str);
}

void MyFrame::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString str;
	str.Format(TEXT("%c"), nChar);

	MessageBox(str);
}

void MyFrame::OnPaint()
{
	CPaintDC dc(this);

	dc.TextOutW(100, 400, TEXT("1234dlajfa"));

	dc.Ellipse(100, 100, 400, 500);  //椭圆

	//char* -> CString
	char* p3 = "ccc";
	CString str = CString(p3);

	//CString -> char*
	CStringA tmp;
	tmp = str;
	char* pp = tmp.GetBuffer();
}
