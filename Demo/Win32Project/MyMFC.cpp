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




MyFrame::MyFrame()
{
	Create(NULL, TEXT("mfc"));
}
