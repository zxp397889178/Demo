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
	//打开file:///本地文件必须用这种方式   https://www.it1352.com/2123677.html
	//查找默认浏览器并打开网页  
	{

		// 计算机\HKEY_CURRENT_USER\Software\Microsoft\Windows\Shell\Associations\UrlAssociations\http
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
				// 计算机\HKEY_LOCAL_MACHINE\Software\Classes\ 或者 计算机\HKEY_CURRENT_USER\Software\Classes\ 下
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

	//创建套接字  
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}

	//绑定IP和端口  
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8888);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (::bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("listen error !");
		return 0;
	}

	//开始监听  
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		printf("listen error !");
		return 0;
	}

	//循环接收数据  
	SOCKET sClient;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	char revData[255];
	while (true)
	{
		printf("等待连接...\n");
		sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET)
		{
			printf("accept error !");
			continue;
		}
		printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));

		//接收数据  
		int ret = recv(sClient, revData, 255, 0);
		if (ret > 0)
		{
			revData[ret] = 0x00;
			printf(revData);
		}

		//发送数据  
		const char * sendData = "你好，TCP客户端！\n";
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
		{  //连接失败 
			printf("connect error !");
			closesocket(sclient);
			return 0;
		}

// 		string data;
// 		cin >> data;
// 		const char * sendData;
// 		sendData = data.c_str();   //string转const char* 
		char * sendData = "你好，TCP服务端，我是客户端\n";
		send(sclient, sendData, strlen(sendData), 0);
		//send()用来将数据由指定的socket传给对方主机
		//int send(int s, const void * msg, int len, unsigned int flags)
		//s为已建立好连接的socket，msg指向数据内容，len则为数据长度，参数flags一般设0
		//成功则返回实际传送出去的字符数，失败返回-1，错误原因存于error 

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
	WSAData wsd;           //初始化信息
	SOCKET soRecv;              //接收SOCKET
	char * pszRecv = NULL; //接收数据的数据缓冲区指针
	int nRet = 0;
	//int i = 0;
	int dwSendSize = 0;
	SOCKADDR_IN siRemote, siLocal;    //远程发送机地址和本机接收机地址

	//启动Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) 
	{
		return 0;
	}

	//创建socket
	soRecv = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (soRecv == SOCKET_ERROR) 
	{
		return 1;
	}

	//设置端口号
	int nPort = 5150;

	//int nPort = 1234;
	siLocal.sin_family = AF_INET;
	siLocal.sin_port = htons(nPort);
	siLocal.sin_addr.s_addr = inet_addr("127.0.0.1");

	//绑定本地地址到socket
	if (::bind(soRecv, (SOCKADDR*)&siLocal, sizeof(siLocal)) == SOCKET_ERROR) 
	{
		return 1;
	}

	//申请内存
	pszRecv = new char[4096];
	if (pszRecv == NULL) 
	{
		return 0;
	}

	for (int i = 0; i < 30; i++){
		dwSendSize = sizeof(siRemote);
		//开始接受数据
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
	//关闭socket连接
	closesocket(soRecv);
	delete[] pszRecv;

	//清理
	WSACleanup();

	return true;
}

bool CMainViewWnd::OnBtnUDPClient(TNotifyUI* msg)
{
	WSAData wsd;           //初始化信息
	SOCKET soSend;         //发送SOCKET
	int nRet = 0;
	//int i = 0;
	int dwSendSize = 0;
	SOCKADDR_IN siLocal;    //远程发送机地址和本机接收机地址

	//启动Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		/*进行WinSocket的初始化,windows 初始化socket网络库，申请2，2的版本，windows socket编程必须先初始化。*/
		return 0;
	}

	//创建socket

	//AF_INET 协议族:决定了要用ipv4地址（32位的）与端口号（16位的）的组合
	//SOCK_DGRAM --  UDP类型，不保证数据接收的顺序，非可靠连接；
	soSend = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (soSend == SOCKET_ERROR)
	{
		return 1;
	}

	//设置端口号
	int nPort = 5150;
	siLocal.sin_family = AF_INET;
	siLocal.sin_port = htons(nPort);
	siLocal.sin_addr.s_addr = inet_addr("127.0.0.1");

	for (;;){
		//开始发送数据
		//发送数据到指定的IP地址和端口
		nRet = sendto(soSend, "123 mutouren", strlen("123 mutouren"), 0, (SOCKADDR*)&siLocal, sizeof(SOCKADDR));
		if (nRet == SOCKET_ERROR)
		{
			break;
		}
	}
	//关闭socket连接
	closesocket(soSend);
	//清理
	WSACleanup();
	return true;
}

#include <functional>
bool CMainViewWnd::OnBtnfun1(TNotifyUI* msg)
{
	//取反
	negate<int> n;
	int a = n(50);

	//加法
	plus<int> p;
	int b = p(50, 10);

	minus<int> m; //减法
	multiplies<int> m2; //乘法
	divides<int> d; //除法
	modulus<int> m3; //取模；

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
	equal_to<int> eq;			//等于
	not_equal_to<int> neq;		//不等于
	greater<int> g;				//大于
	greater_equal<int> geq;		//大于等于
	less<int> l;				//小于
	less_equal<int> leq;		//小于等于
	bool b = leq(10, 20);

// 	vector<int, MyCmp()> vec;
// 	vec.push_back(1);

	return true;
}

#include <numeric>
bool CMainViewWnd::OnBtnfun3(TNotifyUI* msg)
{
	logical_and<int> la;		//逻辑与
	logical_or<int> lo;			//逻辑或
	logical_not<int> ln;		//逻辑非

	vector<bool> vec1;
	vec1.push_back(true);
	vec1.push_back(false);

	vector<bool> vec2;
	vec2.resize(vec1.size());//目标容器必须先开辟空间，不然transform会失败
	transform(vec1.begin(), vec1.end(), vec2.begin(), logical_not<bool>());

	//查找容器中的重复相邻元素
	adjacent_find(vec1.begin(), vec1.end());

	//二分查找法，必须有序才可以,如果是无序序列，则结果未知
	binary_search(vec1.begin(), vec1.end(), true);

	//随机数种子
	srand((unsigned int)time(NULL));
	random_shuffle(vec1.begin(), vec1.end()); //打乱顺序

	reverse(vec1.begin(), vec1.end());  //反转容器数据



	vector<int> v;
	vector<int> vec;
	for (int i = 0; i <= 100; i++)
	{
		v.push_back(i);
		vec.push_back(i + 5);
	}

	//包含头文件 numeric   参数3是起始累加值
	int total = accumulate(v.begin(), v.end(), 0);   //累加
	fill(v.begin(), v.end(), 100);   //填充数据


	//常用集合算法
	vector<int> vv;

	//set_intersection 取交集
	vv.resize(min(v.size(), vec.size()));
	vector<int>::iterator itEnd = set_intersection(v.begin(), v.end(), vec.begin(), vec.end(), vv.begin());

	//set_union 取并集
	vv.resize(v.size() + vec.size());
	itEnd = set_union(v.begin(), v.end(), vec.begin(), vec.end(), vv.begin());

	//set_difference 取差集
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

	tstring wstrTip = _T("101教育PPT");
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
	// 启动http server
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
