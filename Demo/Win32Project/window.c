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
		//������xxxWindowΪ��β��API�������������Ϣ���У�ֱ��ִ��
		DestroyWindow(hwnd);  //������һ����Ϣ
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

	//Ĭ�ϴ���ʽ
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,			//Ӧ�ó���ʵ�����
	_In_opt_ HINSTANCE hPrevInstance,	//��һ��Ӧ�ó����ʵ�������win32�����£�����һ��ΪNULL����������
	_In_ LPSTR lpCmdLine,				//char* argv[]
	_In_ int nShowCmd)					//��ʾ�����󻯡���С��������
{
	//1.��ƴ���
	//2.ע�ᴰ��
	//3.��������
	//4.��ʾ�͸���
	//5.ͨ��ѭ��ȡ��Ϣ
	//6.������Ϣ�����ڹ��̣�

	//1.��ƴ���
	WNDCLASS wc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);		//���ñ���
	wc.hCursor = LoadCursor(NULL, IDC_HAND);					//���ù�꣬�����һ��������NULL�������ʹ��ϵͳ�ṩ�Ĺ��
	wc.hIcon = LoadIcon(NULL, IDI_ERROR);						//ͼ�� �����һ������ΪNULL������ʹ��ϵͳ�ṩ��ͼ��
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WindowProc;								//�ص����� ���ڹ���
	wc.lpszClassName = TEXT("test");
	wc.lpszMenuName = NULL;								//�˵�����
	wc.style = 0;

	//2.ע�ᴰ��
	RegisterClass(&wc);

	//3.��������
	HWND hWnd = CreateWindow(wc.lpszClassName, TEXT("window"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	//4.��ʾ�͸���
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);

	//5.ͨ��ѭ��ȡ��Ϣ

	//HWND        hwnd;
	//UINT        message;
	//WPARAM      wParam;	������Ϣ ������Ϣ
	//LPARAM      lParam;	������Ϣ �����Ϣ
	//DWORD       time;		��Ϣ����ʱ��
	//POINT       pt;		������Ϣ �����Ϣ ���λ��
	MSG msg;
	while (1)
	{
//		__out LPMSG lpMsg,
//		__in_opt HWND hWnd,			���񴰿ڣ�NULL���������еĴ���
//		__in UINT wMsgFilterMin,	��С�����Ĺ�����Ϣ һ����0
//		__in UINT wMsgFilterMax		0������������Ϣ
		
		if (GetMessage(&msg, NULL, 0, 0) == FALSE)
		{
			break;
		}

		//������Ϣ
		TranslateMessage(&msg);

		//��Ϊfalse���ַ���Ϣ
		DispatchMessage(&msg);
	}

	//6.������Ϣ�����ڹ��̣�

	return 0;
}
*/