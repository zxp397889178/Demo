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
};