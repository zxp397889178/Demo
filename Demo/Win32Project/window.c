/*
#include <Windows.h>

LRESULT CALLBACK WindowProc(
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		//所有以xxxWindow为结尾的API，都不会进入消息队列，直接执行
		DestroyWindow(hwnd);  //发送另一个消息
		break;
	case WM_DESTROY:
		PostQuitMessage(-1);
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		TextOut(hdc, 100, 100, TEXT("1234"), strlen("1234"));
		EndPaint(hwnd, &ps);
		break;
	}
	default:
		break;
	}

	//默认处理方式
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,			//应用程序实例句柄
	_In_opt_ HINSTANCE hPrevInstance,	//上一个应用程序的实例句柄，win32环境下，参数一般为NULL，不起作用
	_In_ LPSTR lpCmdLine,				//char* argv[]
	_In_ int nShowCmd)					//显示命令，最大化、最小化、正常
{
	//1.设计窗口
	//2.注册窗口
	//3.创建窗口
	//4.显示和更新
	//5.通过循环取消息
	//6.处理消息（窗口过程）

	//1.设计窗口
	WNDCLASS wc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);		//设置背景
	wc.hCursor = LoadCursor(NULL, IDC_HAND);					//设置光标，如果第一个参数是NULL，则代表使用系统提供的光标
	wc.hIcon = LoadIcon(NULL, IDI_ERROR);						//图标 如果第一个参数为NULL，代表使用系统提供个图标
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WindowProc;								//回调函数 窗口过程
	wc.lpszClassName = TEXT("test");
	wc.lpszMenuName = NULL;								//菜单名称
	wc.style = 0;

	//2.注册窗口
	RegisterClass(&wc);

	//3.创建窗口
	HWND hWnd = CreateWindow(wc.lpszClassName, TEXT("window"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	//4.显示和更新
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);

	//5.通过循环取消息

	//HWND        hwnd;
	//UINT        message;
	//WPARAM      wParam;	附加消息 键盘消息
	//LPARAM      lParam;	附加消息 鼠标消息
	//DWORD       time;		消息产生时间
	//POINT       pt;		附加消息 鼠标消息 鼠标位置
	MSG msg;
	while (1)
	{
//		__out LPMSG lpMsg,
//		__in_opt HWND hWnd,			捕获窗口，NULL代表捕获所有的窗口
//		__in UINT wMsgFilterMin,	最小和最大的过滤消息 一般填0
//		__in UINT wMsgFilterMax		0代表捕获所有消息
		
		if (GetMessage(&msg, NULL, 0, 0) == FALSE)
		{
			break;
		}

		//翻译消息
		TranslateMessage(&msg);

		//不为false，分发消息
		DispatchMessage(&msg);
	}

	//6.处理消息（窗口过程）

	return 0;
}
*/