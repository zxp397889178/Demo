#include <iostream>
#include <memory>
#include "http_server.h"

bool Fun1Handler(std::string body, std::string query_string, mg_connection *c, OnRspCallback reply_callback)
{
    std::cout << "FUNC: " << __FUNCTION__ << "  "
              << "body: " << body << std::endl;
    std::cout << "FUNC: " << __FUNCTION__ << "  "
              << "query_string: " << query_string << std::endl;

    reply_callback(c, "200 OK", "success");

	return true;
}

bool Fun2Handler(std::string body, std::string query_string, mg_connection *c, OnRspCallback reply_callback)
{
    std::cout << "FUNC: " << __FUNCTION__ << "  "
              << "body: " << body << std::endl;
    std::cout << "FUNC: " << __FUNCTION__ << "  "
              << "query_string: " << query_string << std::endl;

    char n1[100], n2[100];
    char res[100];
    double result;

    /* Get form variables */
    /* string: n1=x&n2=y  */
    if(!body.empty()){
        struct mg_str http_body;
        http_body.p   = body.c_str();
        http_body.len = body.length();
        mg_get_http_var(&http_body, "n1", n1, sizeof(n1));
        mg_get_http_var(&http_body, "n2", n2, sizeof(n2));

    }
    else if(!query_string.empty()){
        struct mg_str http_body;
        http_body.p   = query_string.c_str();
        http_body.len = query_string.length();
        mg_get_http_var(&http_body, "n1", n1, sizeof(n1));
        mg_get_http_var(&http_body, "n2", n2, sizeof(n2));
    }

    /* Compute the result and send it back as a JSON object */
    result = strtod(n1, NULL) + strtod(n2, NULL);
    sprintf(res, "%0.5f", result);
    reply_callback(c, "200 OK", res);

    return true;
}

// int main(int argc, char *argv[]) 
// {
//     printf("\tusage notes: %s [port] [webpath]\n"
//            "\t - [port]:\tYou can modify the port that webserver uses, through appoint [port], default 80.\n"
//            "\t - [webpath]:\tYou can modify the web/html path, through appoint [webpath], default '../web'.\n", argv[0]);
// 
//     std::string port;
//     switch (argc) {
//         case 1:{
//             port = "80"; //默认使用80端口
//             break;
//         }
//         case 2:{
//             port = argv[1];
//             break;
//         }
//         case 3:{
//             port = argv[1];
//             HttpServer::s_web_dir = argv[2];
//         }
//         default:
//             break;
//     }
// 
//     // 启动http server
// 	auto http_server = std::shared_ptr<HttpServer>(new HttpServer);
// 	http_server->Init(port);
// 	// add handler
//     http_server->AddHandler("/api/fun1", Fun1Handler);
//     http_server->AddHandler("/api/sum",  Fun2Handler);
// 	http_server->Start();
// 
// 	
// 	return 0;
// }
