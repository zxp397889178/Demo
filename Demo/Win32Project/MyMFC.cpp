#include "MyMFC.h"

MyMFC myMfc;  //ȫ��Ӧ�ó���������ҽ���һ��

BOOL MyMFC::InitInstance()
{
	//��������
	MyFrame* frame = new MyFrame;

	//��ʾ�͸���
	frame->ShowWindow(SW_SHOW);
	frame->UpdateWindow();

	m_pMainWnd = frame;  //����ָ��Ӧ�ó���������ڵ�ָ��

	return TRUE;
}


//�ֽ��
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

	//MFC�е��ַ��� CString
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

	dc.Ellipse(100, 100, 400, 500);  //��Բ

	//char* -> CString
	char* p3 = "ccc";
	CString str = CString(p3);

	//CString -> char*
	CStringA tmp;
	tmp = str;
	char* pp = tmp.GetBuffer();
}
