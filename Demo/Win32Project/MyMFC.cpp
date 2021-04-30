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




MyFrame::MyFrame()
{
	Create(NULL, TEXT("mfc"));
}
