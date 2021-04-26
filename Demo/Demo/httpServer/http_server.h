#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include "common/mongoose.h"

// ����http����callback
typedef void OnRspCallback(mg_connection *c, const char*, std::string);
// ����http����handler
using ReqHandler = std::function<bool (std::string, std::string, mg_connection *c, OnRspCallback)>;

class HttpServer
{
public:
	HttpServer();
	~HttpServer();
    void Init(const std::string &port); // ��ʼ������
    bool Start(); // ����httpserver
    bool Close(); // �ر�
    void AddHandler(const std::string &url, ReqHandler req_handler); // ע���¼�������
    void RemoveHandler(const std::string &url); // �Ƴ��¼�������

public:
    static std::string s_web_dir; // ��ҳ��Ŀ¼
    static mg_serve_http_opts* s_server_option; // web������ѡ��
    static std::unordered_map<std::string, ReqHandler> s_handler_map; // �ص�����ӳ���

private:
    // ��̬�¼���Ӧ����
    static void OnHttpEvent(mg_connection *connection, int event_type, void *event_data);
    static void HandleEvent(mg_connection *connection, http_message *http_req);
    static void SendRsp(mg_connection *connection, const char *status, std::string rsp); // status����Ӧ״̬��

private:
    std::string m_port;    // �˿�
    mg_mgr m_mgr;          // ���ӹ�����
};
