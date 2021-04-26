#include "stdafx.h"
#include "Utils/Macros.h"
#include "MainViewWnd.h"

#include "View\UIEditLayout.h"
#include "Event\LoginEvent.hpp"
#include "Event\UserCenterEventModel.hpp"
#include "UserCenter/LoginWnd.h"
#include "Util/Common.h"

#include <WinSock2.h>
#include <winsock.h>
#include <thread>
#include "ShowText/ShowTextWnd.h"
#include <shellapi.h>

//#include "httpServer/http_server.h"

CMainViewWnd::CMainViewWnd()
{

}

CMainViewWnd::~CMainViewWnd()
{
}

HWND CMainViewWnd::CreateWnd(HWND hParent)
{
	SetLayerWindow(true);
	HWND hWnd = Create(hParent, _T("MainViewWindow"));

	return hWnd;
}

void CMainViewWnd::OnCreate()
{

}

void CMainViewWnd::OnClose()
{
	__super::OnClose();
	::PostQuitMessage(-1);
}

bool CMainViewWnd::OnBtnClose(TNotifyUI* msg)
{
	CloseWindow();
	return true;
}

bool CMainViewWnd::OnBtnLoginPanel(TNotifyUI* msg)
{
	CLoginWnd* loginWnd = new CLoginWnd();
	if (loginWnd)
	{
		HWND hwnd = loginWnd->CreateWnd(GetHWND());
		loginWnd->SetAutoDel(false);
		loginWnd->CenterWindow();
		loginWnd->ShowWindow(SW_SHOW);
	}

	return true;
}

bool CMainViewWnd::OnBtnReg(TNotifyUI* msg)
{
	//��file:///�����ļ����������ַ�ʽ   https://www.it1352.com/2123677.html
	//����Ĭ�������������ҳ  
	{

		// �����\HKEY_CURRENT_USER\Software\Microsoft\Windows\Shell\Associations\UrlAssociations\http
		tstring wstrBrowserPath = _T("");
		tstring wstrBrowserCmd = _T("");
		CRegKey reg;
		if (reg.Open(HKEY_CURRENT_USER,
			_T("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\http\\UserChoice"),
			KEY_READ) == ERROR_SUCCESS)
		{
			TCHAR szValue[512] = { 0 };
			DWORD dwSize = 512;
			if (reg.QueryStringValue(_T("ProgId"), szValue, &dwSize) == ERROR_SUCCESS)
			{
				// �����\HKEY_LOCAL_MACHINE\Software\Classes\ ���� �����\HKEY_CURRENT_USER\Software\Classes\ ��
				TCHAR szTemp[MAX_PATH];
				_stprintf_s(szTemp, _T("SOFTWARE\\Classes\\%s\\shell\\open\\command"), szValue);
				wstrBrowserCmd = szTemp;
				reg.Close();

				TCHAR szValue2[512] = { 0 };
				DWORD dwSize2 = 512;
				if (reg.Open(HKEY_LOCAL_MACHINE, wstrBrowserCmd.c_str(), KEY_READ) == ERROR_SUCCESS)
				{
					if (reg.QueryStringValue(_T(""), szValue2, &dwSize2) == ERROR_SUCCESS)
					{
						wstrBrowserCmd = szValue2;
					}
				}
				else if (reg.Open(HKEY_CURRENT_USER, wstrBrowserCmd.c_str(), KEY_READ) == ERROR_SUCCESS)
				{
					if (reg.QueryStringValue(_T(""), szValue2, &dwSize2) == ERROR_SUCCESS)
					{
						wstrBrowserCmd = szValue2;
					}
				}
			}
		}

		if (!wstrBrowserCmd.empty())
		{
			int index = wstrBrowserCmd.rfind(_T("."));
			if (index != wstring::npos)
			{
				wstrBrowserPath = wstrBrowserCmd.substr(1, index + 3);
			}
		}

		tstring wstrCmd = _T("\"") + wstrBrowserPath;
		wstrCmd += _T("\" \"");
		wstrCmd += _T("www.baidu.com");
		wstrCmd += _T("\"");
		PROCESS_INFORMATION pi = { 0 };
		STARTUPINFO si = { 0 };
		si.cb = sizeof(STARTUPINFO);
		::CreateProcess(wstrBrowserPath.c_str(), (LPWSTR)wstrCmd.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	}

	return true;
}


#pragma comment(lib,"ws2_32.lib") 
bool CMainViewWnd::OnBtnSocketServer(TNotifyUI* msg)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		return true;
	}

	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return true;

	}

	//�����׽���  
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}

	//��IP�Ͷ˿�  
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8888);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (::bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("listen error !");
		return 0;
	}

	//��ʼ����  
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		printf("listen error !");
		return 0;
	}

	//ѭ����������  
	SOCKET sClient;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	char revData[255];
	while (true)
	{
		printf("�ȴ�����...\n");
		sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET)
		{
			printf("accept error !");
			continue;
		}
		printf("���ܵ�һ�����ӣ�%s \r\n", inet_ntoa(remoteAddr.sin_addr));

		//��������  
		int ret = recv(sClient, revData, 255, 0);
		if (ret > 0)
		{
			revData[ret] = 0x00;
			printf(revData);
		}

		//��������  
		const char * sendData = "��ã�TCP�ͻ��ˣ�\n";
		send(sClient, sendData, strlen(sendData), 0);
		closesocket(sClient);
	}

	closesocket(slisten);
	WSACleanup();

	return true;
}

bool CMainViewWnd::OnBtnSocketClient(TNotifyUI* msg)
{
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA data;
	if (WSAStartup(sockVersion, &data) != 0)
	{
		return 0;
	}

	while (true)
	{
		SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sclient == INVALID_SOCKET)
		{
			printf("invalid socket!");
			return 0;
		}

		sockaddr_in serAddr;
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(8888);
		serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
		{  //����ʧ�� 
			printf("connect error !");
			closesocket(sclient);
			return 0;
		}

// 		string data;
// 		cin >> data;
// 		const char * sendData;
// 		sendData = data.c_str();   //stringתconst char* 
		char * sendData = "��ã�TCP����ˣ����ǿͻ���\n";
		send(sclient, sendData, strlen(sendData), 0);
		//send()������������ָ����socket�����Է�����
		//int send(int s, const void * msg, int len, unsigned int flags)
		//sΪ�ѽ��������ӵ�socket��msgָ���������ݣ�len��Ϊ���ݳ��ȣ�����flagsһ����0
		//�ɹ��򷵻�ʵ�ʴ��ͳ�ȥ���ַ�����ʧ�ܷ���-1������ԭ�����error 

		char recData[255];
		int ret = recv(sclient, recData, 255, 0);
		if (ret > 0){
			recData[ret] = 0x00;
			printf(recData);
		}
		closesocket(sclient);
	}


	WSACleanup();

	return true;
}

bool CMainViewWnd::OnBtnUDPServer(TNotifyUI* msg)
{
	WSAData wsd;           //��ʼ����Ϣ
	SOCKET soRecv;              //����SOCKET
	char * pszRecv = NULL; //�������ݵ����ݻ�����ָ��
	int nRet = 0;
	//int i = 0;
	int dwSendSize = 0;
	SOCKADDR_IN siRemote, siLocal;    //Զ�̷��ͻ���ַ�ͱ������ջ���ַ

	//����Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) 
	{
		return 0;
	}

	//����socket
	soRecv = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (soRecv == SOCKET_ERROR) 
	{
		return 1;
	}

	//���ö˿ں�
	int nPort = 5150;

	//int nPort = 1234;
	siLocal.sin_family = AF_INET;
	siLocal.sin_port = htons(nPort);
	siLocal.sin_addr.s_addr = inet_addr("127.0.0.1");

	//�󶨱��ص�ַ��socket
	if (::bind(soRecv, (SOCKADDR*)&siLocal, sizeof(siLocal)) == SOCKET_ERROR) 
	{
		return 1;
	}

	//�����ڴ�
	pszRecv = new char[4096];
	if (pszRecv == NULL) 
	{
		return 0;
	}

	for (int i = 0; i < 30; i++){
		dwSendSize = sizeof(siRemote);
		//��ʼ��������
		nRet = recvfrom(soRecv, pszRecv, 4096, 0, (SOCKADDR*)&siRemote, &dwSendSize);
		if (nRet == SOCKET_ERROR) 
		{
			break;
		}
		else if (nRet == 0) {
			break;
		}
		else{
			pszRecv[nRet] = '\0';
			string str = pszRecv;
		}

	}
	//�ر�socket����
	closesocket(soRecv);
	delete[] pszRecv;

	//����
	WSACleanup();

	return true;
}

bool CMainViewWnd::OnBtnUDPClient(TNotifyUI* msg)
{
	WSAData wsd;           //��ʼ����Ϣ
	SOCKET soSend;         //����SOCKET
	int nRet = 0;
	//int i = 0;
	int dwSendSize = 0;
	SOCKADDR_IN siLocal;    //Զ�̷��ͻ���ַ�ͱ������ջ���ַ

	//����Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		/*����WinSocket�ĳ�ʼ��,windows ��ʼ��socket����⣬����2��2�İ汾��windows socket��̱����ȳ�ʼ����*/
		return 0;
	}

	//����socket

	//AF_INET Э����:������Ҫ��ipv4��ַ��32λ�ģ���˿ںţ�16λ�ģ������
	//SOCK_DGRAM --  UDP���ͣ�����֤���ݽ��յ�˳�򣬷ǿɿ����ӣ�
	soSend = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (soSend == SOCKET_ERROR)
	{
		return 1;
	}

	//���ö˿ں�
	int nPort = 5150;
	siLocal.sin_family = AF_INET;
	siLocal.sin_port = htons(nPort);
	siLocal.sin_addr.s_addr = inet_addr("127.0.0.1");

	for (;;){
		//��ʼ��������
		//�������ݵ�ָ����IP��ַ�Ͷ˿�
		nRet = sendto(soSend, "123 mutouren", strlen("123 mutouren"), 0, (SOCKADDR*)&siLocal, sizeof(SOCKADDR));
		if (nRet == SOCKET_ERROR)
		{
			break;
		}
	}
	//�ر�socket����
	closesocket(soSend);
	//����
	WSACleanup();
	return true;
}

#include <functional>
bool CMainViewWnd::OnBtnfun1(TNotifyUI* msg)
{
	//ȡ��
	negate<int> n;
	int a = n(50);

	//�ӷ�
	plus<int> p;
	int b = p(50, 10);

	minus<int> m; //����
	multiplies<int> m2; //�˷�
	divides<int> d; //����
	modulus<int> m3; //ȡģ��

	return true;
}

class MyCmp
{
public:
	bool operator()(int& a, int&b)
	{
		return a > b;
	}
};

bool CMainViewWnd::OnBtnfun2(TNotifyUI* msg)
{
	equal_to<int> eq;			//����
	not_equal_to<int> neq;		//������
	greater<int> g;				//����
	greater_equal<int> geq;		//���ڵ���
	less<int> l;				//С��
	less_equal<int> leq;		//С�ڵ���
	bool b = leq(10, 20);

// 	vector<int, MyCmp()> vec;
// 	vec.push_back(1);

	return true;
}

#include <numeric>
bool CMainViewWnd::OnBtnfun3(TNotifyUI* msg)
{
	logical_and<int> la;		//�߼���
	logical_or<int> lo;			//�߼���
	logical_not<int> ln;		//�߼���

	vector<bool> vec1;
	vec1.push_back(true);
	vec1.push_back(false);

	vector<bool> vec2;
	vec2.resize(vec1.size());//Ŀ�����������ȿ��ٿռ䣬��Ȼtransform��ʧ��
	transform(vec1.begin(), vec1.end(), vec2.begin(), logical_not<bool>());

	//���������е��ظ�����Ԫ��
	adjacent_find(vec1.begin(), vec1.end());

	//���ֲ��ҷ�����������ſ���,������������У�����δ֪
	binary_search(vec1.begin(), vec1.end(), true);

	//���������
	srand((unsigned int)time(NULL));
	random_shuffle(vec1.begin(), vec1.end()); //����˳��

	reverse(vec1.begin(), vec1.end());  //��ת��������



	vector<int> v;
	vector<int> vec;
	for (int i = 0; i <= 100; i++)
	{
		v.push_back(i);
		vec.push_back(i + 5);
	}

	//����ͷ�ļ� numeric   ����3����ʼ�ۼ�ֵ
	int total = accumulate(v.begin(), v.end(), 0);   //�ۼ�
	fill(v.begin(), v.end(), 100);   //�������


	//���ü����㷨
	vector<int> vv;

	//set_intersection ȡ����
	vv.resize(min(v.size(), vec.size()));
	vector<int>::iterator itEnd = set_intersection(v.begin(), v.end(), vec.begin(), vec.end(), vv.begin());

	//set_union ȡ����
	vv.resize(v.size() + vec.size());
	itEnd = set_union(v.begin(), v.end(), vec.begin(), vec.end(), vv.begin());

	//set_difference ȡ�
	vv.resize(max(v.size(), vec.size()));
	itEnd = set_difference(v.begin(), v.end(), vec.begin(), vec.end(), vv.begin());

	return true;
}

bool CMainViewWnd::OnBtntest(TNotifyUI* msg)
{
// 	CShowTextWnd* pShowTextWnd = new CShowTextWnd();
// 	if (pShowTextWnd)
// 	{
// 		pShowTextWnd->CreateWnd(GetHWND());
// 		pShowTextWnd->CenterWindow();
// 		pShowTextWnd->ShowWindow();
// 	}

	NOTIFYICONDATA NotifyIconData;
	memset(&NotifyIconData, 0, sizeof(NotifyIconData));

	NotifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	NotifyIconData.hIcon = GetUIRes()->LoadImage(_T("#bk_close36"), false, UIIAMGE_HICON)->GetHIcon();
	NotifyIconData.hWnd = GetHWND();
	NotifyIconData.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;

	tstring wstrTip = _T("101����PPT");
	_stprintf_s(NotifyIconData.szTip, _T("%s"), wstrTip.c_str());

	NotifyIconData.uID = 1;
	NotifyIconData.uCallbackMessage = 1223;

	Shell_NotifyIcon(NIM_ADD, &NotifyIconData);

	return true;
}

// bool Fun1Handler(std::string body, std::string query_string, mg_connection *c, OnRspCallback reply_callback)
// {
// 	reply_callback(c, "200 OK", "success");
// 
// 	return true;
// }
// 
// bool Fun2Handler(std::string body, std::string query_string, mg_connection *c, OnRspCallback reply_callback)
// {
// 	char n1[100], n2[100];
// 	char res[100];
// 	double result;
// 
// 	/* Get form variables */
// 	/* string: n1=x&n2=y  */
// 	if (!body.empty()){
// 		struct mg_str http_body;
// 		http_body.p = body.c_str();
// 		http_body.len = body.length();
// 		mg_get_http_var(&http_body, "n1", n1, sizeof(n1));
// 		mg_get_http_var(&http_body, "n2", n2, sizeof(n2));
// 
// 	}
// 	else if (!query_string.empty()){
// 		struct mg_str http_body;
// 		http_body.p = query_string.c_str();
// 		http_body.len = query_string.length();
// 		mg_get_http_var(&http_body, "n1", n1, sizeof(n1));
// 		mg_get_http_var(&http_body, "n2", n2, sizeof(n2));
// 	}
// 
// 	/* Compute the result and send it back as a JSON object */
// 	result = strtod(n1, NULL) + strtod(n2, NULL);
// 	sprintf(res, "%0.5f", result);
// 	reply_callback(c, "200 OK", res);
// 
// 	return true;
// }

bool CMainViewWnd::OnBtntestHttpServer(TNotifyUI* msg)
{
	// ����http server
// 	string strPort = "80";
// 	auto http_server = std::make_shared<HttpServer>();
// 	http_server->Init(strPort);
// 	// add handler
// 	http_server->AddHandler("/api/fun1", Fun1Handler);
// 	http_server->AddHandler("/api/sum", Fun2Handler);
// 	http_server->Start();

	NOTIFYICONDATA NotifyIconData;
	memset(&NotifyIconData, 0, sizeof(NotifyIconData));

	NotifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	NotifyIconData.hWnd = GetHWND();
	NotifyIconData.uID = 1;
	NotifyIconData.uCallbackMessage = 1223;

	Shell_NotifyIcon(NIM_DELETE, &NotifyIconData);

	return true;
}
