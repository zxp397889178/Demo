/*
 * =====================================================================================
 *
 *       Filename:  Shunt.cpp
 *
 *    Description:  分流器
 *
 *        Version:  1.0
 *        Created:  2021年09月07日 11时01分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Shunt.h"
#include <thread>
#include <semaphore.h>
#include "Security.h"

sem_t g_BaiduSem;
static vector<CMyString> vecBaiduResp;

sem_t g_QuickGameSem;
static vector<CMyString> vecQuickGameResp;

sem_t g_QuickSdkSem;
static vector<CMyString> vecQuickSdkResp;

sem_t g_YybSem;
static vector<CMyString> vecYybResp;

sem_t g_YouYiSem;
static vector<CMyString> vecYouYiResp;

sem_t g_ShunYouSem;
static vector<CMyString> vecShunYouResp;

sem_t g_Game8USem;
static vector<CMyString> vecGame8UResp;

CPluginShunt::CPluginShunt()
{

}

CPluginShunt::~CPluginShunt()
{

}

int32_t CPluginShunt::Init(const CMyString &xmlFile, const CMyString &sect)
{
    LogDebug("[%s(%d):%s]Init, xmlFile:%s, sect:%s", __FILE__, __LINE__, __func__, xmlFile.c_str(), sect.c_str());

    int32_t nRet = 0;
    CXmlFile xml;
    xml.Load(xmlFile.c_str());
    void *pRoot(NULL);
    CMyString strSect("%s/item", sect.c_str());
    try
    {
        pRoot = xml.GetElement(strSect);
        while(pRoot != NULL)
        {
            CMyString strUrl = xml.GetValue(pRoot, "url");
            CMyString strType = xml.GetValue(pRoot, "type");
            if (strType == "baidu")
            {
                m_vecBaiduTransferUrl.push_back(strUrl);
            }
            else if (strType == "quickgame")
            {
                m_vecQuickGameTransferUrl.push_back(strUrl);
            }
            else if (strType == "quicksdk")
            {
                m_vecQuickSdkTransferUrl.push_back(strUrl);
            }
            else if (strType == "yyb")
            {
                m_vecYybTransferUrl.push_back(strUrl);
            }
            else if (strType == "youyi")
            {
                m_vecYouYiTransferUrl.push_back(strUrl);
            }
            else if (strType == "shunyou")
            {
                m_vecShunYouTransferUrl.push_back(strUrl);
            }
            else if (strType == "game8u")
            {
                m_vecGame8UTransferUrl.push_back(strUrl);
            }
            
            LogDebug("[%s:%s:%d]get transfer url, strUrl:%s", __FILE__, __func__, __LINE__, strUrl.c_str());
            pRoot = xml.GetNextSibling(pRoot, "item");
        }
    }
    catch(CException &ex)
    {
        LogErr("get transfer url failure, err:%s", ex.what());
        nRet = -103;
    }
    catch(std::exception &ex)
    {
        LogErr("get transfer url failure, err:%s", ex.what());
        nRet = -105;
    }
    catch(...)
    {
        LogErr("get transfer url failure, unknown err");
        nRet = -107;
    }

    return nRet;
}

void CPluginShunt::doMaintance(void)
{
    //LogDebug("[%s(%d):%s]maintance", __FILE__, __LINE__, __func__);
}

void CPluginShunt::doUpdate(void)
{
    LogDebug("[%s(%d):%s]update", __FILE__, __LINE__, __func__);
}

void TransferUrl(CMyString strUrl, CMyString strParams, sem_t* sem, vector<CMyString>* vecResp)
{
    http::VEC_STRING header_list;
    CMyString strRespHeader;
    CMyString strResp;
    http::CHttpClient net;

    CMyString strFullUrl = strUrl + "?" + strParams;
    LogDebug("[%s:%s:%d] ,TransferUrl start strFullUrl:%s", __FILE__, __func__, __LINE__, strFullUrl.c_str());

    int32_t ret = net.HttpGetWithHeaders(strFullUrl.c_str(), strResp, strRespHeader, header_list);
    if (ret != 0)
    {
        CMyString strErr;
        net.getError(ret, strErr);  
        LogWarn("[%s:%s:%d] HttpGetWithHeaders failed, fullurl:%s,  err:%d, msg:%s", __FILE__, __func__, __LINE__,
                strFullUrl.c_str(),  ret, strErr.c_str());
    }
    if (0 == ret)
    {
        sem_wait(sem);
        vecResp->push_back(strResp);
        LogDebug("[%s:%s:%d]TransferUrl, strFullUrl:%s, strResp[%s]", __FILE__, __func__, __LINE__, strFullUrl.c_str(), strResp.c_str());
        sem_post(sem);
    }

}

int32_t ENTRY_CGI_DECLARE(CPluginShunt, doBaiduShunt)
{
    vecBaiduResp.clear();
    if (m_vecBaiduTransferUrl.empty())
    {
        safe_strcpy(szResp, cbResp, "fail");
        cbResp = strlen(szResp);
        return 0;
    }

    CByteStream buff;
    req.getPost(buff);
    CMyString strPost;
    buff.Read(strPost);
    CMyString headers = req.getFullUrl();
    CMyString strParams;
    req.getFullParamStr(strParams);
    LogDebug("do shunt, header[%s], param[%s] post[%s]",
            headers.c_str(), strParams.c_str(), strPost.c_str());

    sem_init(&g_BaiduSem, 0, 1);
    for (size_t i = 0; i < m_vecBaiduTransferUrl.size(); ++i)
    {
        CMyString strUrl = m_vecBaiduTransferUrl[i];
        LogDebug("[%s:%s:%d]strUrl: %s", __FILE__, __func__, __LINE__, strUrl.c_str());

        std::thread t1(TransferUrl, strUrl, strParams, &g_BaiduSem, &vecBaiduResp);
        t1.detach();
    }

    int nCount = 0;
    while (vecBaiduResp.size() < m_vecBaiduTransferUrl.size())
    {
        sleep(1);
        nCount ++;
        if (nCount > 10)
        {
            LogDebug("[%s:%s:%d ncount > 10]", __FILE__, __func__, __LINE__);
            break;
        }
    }

    CMyString strResp;
    for (size_t i =0 ; i < vecBaiduResp.size(); ++i)
    {
        strResp = vecBaiduResp[i];
        if (strResp == "success" )
        {
            break;
        }
    }
    sem_destroy(&g_BaiduSem);

    if (strResp != "success")
    {
        strResp = "fail";
    }

    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
    LogDebug("[%s:%s:%d]do shunt, szResp[%s]", __FILE__, __func__, __LINE__, szResp);
    return 0;
}

int32_t ENTRY_CGI_DECLARE(CPluginShunt, doQuickGameShunt)
{
    vecQuickGameResp.clear();
    if (m_vecQuickGameTransferUrl.empty())
    {
        safe_strcpy(szResp, cbResp, "FAILED");
        cbResp = strlen(szResp);
        return 0;
    }

    CByteStream buff;
    req.getPost(buff);
    CMyString strPost;
    buff.Read(strPost);
    CMyString headers = req.getFullUrl();
    CMyString strParams;
    req.getFullParamStr(strParams);
    LogDebug("do shunt, header[%s], param[%s] post[%s]",
            headers.c_str(), strParams.c_str(), strPost.c_str());

    sem_init(&g_QuickGameSem, 0, 1);
    for (size_t i = 0; i < m_vecQuickGameTransferUrl.size(); ++i)
    {
        CMyString strUrl = m_vecQuickGameTransferUrl[i];
        LogDebug("[%s:%s:%d]strUrl: %s", __FILE__, __func__, __LINE__, strUrl.c_str());

        std::thread t1(TransferUrl, strUrl, strParams, &g_QuickGameSem, &vecQuickGameResp);
        t1.detach();
    }

    int nCount = 0;
    while (vecQuickGameResp.size() < m_vecQuickGameTransferUrl.size())
    {
        sleep(1);
        nCount ++;
        if (nCount > 10)
        {
            LogDebug("[%s:%s:%d ncount > 10]", __FILE__, __func__, __LINE__);
            break;
        }
    }

    CMyString strResp;
    for (size_t i =0 ; i < vecQuickGameResp.size(); ++i)
    {
        strResp = vecQuickGameResp[i];
        if (strResp == "SUCCESS" )
        {
            break;
        }
    }
    sem_destroy(&g_QuickGameSem);

    if (strResp != "SUCCESS")
    {
        strResp = "FAILED";
    }

    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
    LogDebug("[%s:%s:%d]do shunt, szResp[%s]", __FILE__, __func__, __LINE__, szResp);
    return 0;
}

int32_t ENTRY_CGI_DECLARE(CPluginShunt, doQuickSdkShunt)
{
    vecQuickSdkResp.clear();
    if (m_vecQuickSdkTransferUrl.empty())
    {
        safe_strcpy(szResp, cbResp, "FAILED");
        cbResp = strlen(szResp);
        return 0;
    }

    CByteStream buff;
    req.getPost(buff);
    CMyString strPost;
    buff.Read(strPost);
    CMyString headers = req.getFullUrl();
    CMyString strParams;
    req.getFullParamStr(strParams);
    LogDebug("do shunt, header[%s], param[%s] post[%s]",
            headers.c_str(), strParams.c_str(), strPost.c_str());

    sem_init(&g_QuickSdkSem, 0, 1);
    for (size_t i = 0; i < m_vecQuickSdkTransferUrl.size(); ++i)
    {
        CMyString strUrl = m_vecQuickSdkTransferUrl[i];
        LogDebug("[%s:%s:%d]strUrl: %s", __FILE__, __func__, __LINE__, strUrl.c_str());

        std::thread t1(TransferUrl, strUrl, strParams, &g_QuickSdkSem, &vecQuickSdkResp);
        t1.detach();
    }

    int nCount = 0;
    while (vecQuickSdkResp.size() < m_vecQuickSdkTransferUrl.size())
    {
        sleep(1);
        nCount ++;
        if (nCount > 10)
        {
            LogDebug("[%s:%s:%d ncount > 10]", __FILE__, __func__, __LINE__);
            break;
        }
    }

    CMyString strResp;
    for (size_t i =0 ; i < vecQuickSdkResp.size(); ++i)
    {
        strResp = vecQuickSdkResp[i];
        if (strResp == "SUCCESS" )
        {
            break;
        }
    }
    sem_destroy(&g_QuickSdkSem);

    if (strResp != "SUCCESS")
    {
        strResp = "FAILED";
    }

    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
    LogDebug("[%s:%s:%d]do shunt, szResp[%s]", __FILE__, __func__, __LINE__, szResp);
    return 0;
}

int32_t ENTRY_CGI_DECLARE(CPluginShunt, doYybShunt)
{
    vecYybResp.clear();
    if (m_vecYybTransferUrl.empty())
    {
        Json::Value resp_data;
        resp_data["ret"] = -1;
        resp_data["msg"] = "FAIL";
        CJsonWrapper wr;
        CMyString strResp = wr.write(resp_data);

        safe_strcpy(szResp, cbResp, strResp.c_str());
        cbResp = strlen(szResp);
        return 0;
    }

    CMyString strParams;
    http::CHttpData::NodeVector vecNode = req.getParams();
    for (size_t i = 0; i < vecNode.size(); ++i)
    {
        http::CHttpData::Node& node = vecNode[i];
       
        if (!strParams.empty())
        {
            strParams += "&";
        }
        strParams += node.name;
        strParams += "=";

        if ("sig" == node.name)
        {
            strParams += CHttpWrapper::encodeURIComponent(node.value);
        }
        else
        {
            strParams += node.value;
        }
    }
    LogMsg("[%s:%s:%d] doYybShunt strParams:%s", __FILE__, __func__, __LINE__, strParams.c_str());

    sem_init(&g_YybSem, 0, 1);
    for (size_t i = 0; i < m_vecYybTransferUrl.size(); ++i)
    {
        CMyString strUrl = m_vecYybTransferUrl[i];
        LogDebug("[%s:%s:%d]strUrl: %s", __FILE__, __func__, __LINE__, strUrl.c_str());

        std::thread t1(TransferUrl, strUrl, strParams, &g_YybSem, &vecYybResp);
        t1.detach();
    }

    int nCount = 0;
    while (vecYybResp.size() < m_vecYybTransferUrl.size())
    {
        sleep(1);
        nCount ++;
        if (nCount > 10)
        {
            LogDebug("[%s:%s:%d ncount > 10]", __FILE__, __func__, __LINE__);
            break;
        }
    }

    int32_t nRet = -1;
    CMyString strResp;
    for (size_t i =0 ; i < vecYybResp.size(); ++i)
    {
        strResp = vecYybResp[i];
        CJsonWrapper jr;
        if (jr.Load(strResp))
        {
            nRet = jr.getInt("ret");
            if (nRet == 0)
            {
                break;
            }
        }
    }
    sem_destroy(&g_YybSem);

    if (nRet != 0)
    {
        Json::Value resp_data;
        resp_data["ret"] = -1;
        resp_data["msg"] = "FAIL";
        CJsonWrapper wr;
        strResp = wr.write(resp_data);
    }

    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
    LogDebug("[%s:%s:%d]do shunt, szResp[%s]", __FILE__, __func__, __LINE__, szResp);
    return 0;
}


int32_t ENTRY_CGI_DECLARE(CPluginShunt, doYouYiShunt)
{
    vecYouYiResp.clear();
    if (m_vecYouYiTransferUrl.empty())
    {
        safe_strcpy(szResp, cbResp, "fail");
        cbResp = strlen(szResp);
        return 0;
    }

    CByteStream buff;
    req.getPost(buff);
    CMyString strPost;
    buff.Read(strPost);
    CMyString headers = req.getFullUrl();
    CMyString strParams;
    req.getFullParamStr(strParams);
    LogDebug("do shunt, header[%s], param[%s] post[%s]",
            headers.c_str(), strParams.c_str(), strPost.c_str());

    sem_init(&g_YouYiSem, 0, 1);
    for (size_t i = 0; i < m_vecYouYiTransferUrl.size(); ++i)
    {
        CMyString strUrl = m_vecYouYiTransferUrl[i];
        LogDebug("[%s:%s:%d]strUrl: %s", __FILE__, __func__, __LINE__, strUrl.c_str());

        std::thread t1(TransferUrl, strUrl, strParams, &g_YouYiSem, &vecYouYiResp);
        t1.detach();
    }

    int nCount = 0;
    while (vecYouYiResp.size() < m_vecYouYiTransferUrl.size())
    {
        sleep(1);
        nCount ++;
        if (nCount > 10)
        {
            LogDebug("[%s:%s:%d ncount > 10]", __FILE__, __func__, __LINE__);
            break;
        }
    }

    CMyString strResp;
    for (size_t i =0 ; i < vecYouYiResp.size(); ++i)
    {
        strResp = vecYouYiResp[i];
        if (strResp == "success" )
        {
            break;
        }
    }
    sem_destroy(&g_YouYiSem);

    if (strResp != "success")
    {
        strResp = "fail";
    }

    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
    LogDebug("[%s:%s:%d]do shunt, szResp[%s]", __FILE__, __func__, __LINE__, szResp);
    return 0;
}

int32_t ENTRY_CGI_DECLARE(CPluginShunt, doShunYouShunt)
{
    vecShunYouResp.clear();
    if (m_vecShunYouTransferUrl.empty())
    {
        safe_strcpy(szResp, cbResp, "fail");
        cbResp = strlen(szResp);
        return 0;
    }

    CByteStream buff;
    req.getPost(buff);
    CMyString strPost;
    buff.Read(strPost);
    CMyString headers = req.getFullUrl();
    CMyString strParams;
    req.getFullParamStr(strParams);
    LogDebug("do shunt, header[%s], param[%s] post[%s]",
            headers.c_str(), strParams.c_str(), strPost.c_str());

    sem_init(&g_ShunYouSem, 0, 1);
    for (size_t i = 0; i < m_vecShunYouTransferUrl.size(); ++i)
    {
        CMyString strUrl = m_vecShunYouTransferUrl[i];
        LogDebug("[%s:%s:%d]strUrl: %s", __FILE__, __func__, __LINE__, strUrl.c_str());

        std::thread t1(TransferUrl, strUrl, strParams, &g_ShunYouSem, &vecShunYouResp);
        t1.detach();
    }

    int nCount = 0;
    while (vecShunYouResp.size() < m_vecShunYouTransferUrl.size())
    {
        sleep(1);
        nCount ++;
        if (nCount > 10)
        {
            LogDebug("[%s:%s:%d ncount > 10]", __FILE__, __func__, __LINE__);
            break;
        }
    }

    CMyString strResp;
    for (size_t i =0 ; i < vecShunYouResp.size(); ++i)
    {
        strResp = vecShunYouResp[i];
        if (strResp == "success" )
        {
            break;
        }
    }
    sem_destroy(&g_ShunYouSem);

    if (strResp != "success")
    {
        strResp = "error";
    }

    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
    LogDebug("[%s:%s:%d]do shunt, szResp[%s]", __FILE__, __func__, __LINE__, szResp);
    return 0;
}

int32_t ENTRY_CGI_DECLARE(CPluginShunt, doGame8UShunt)
{
    vecGame8UResp.clear();
    if (m_vecGame8UTransferUrl.empty())
    {
        safe_strcpy(szResp, cbResp, "fail");
        cbResp = strlen(szResp);
        return 0;
    }

    CByteStream buff;
    req.getPost(buff);
    CMyString strPost;
    buff.Read(strPost);
    CMyString headers = req.getFullUrl();
    CMyString strParams;
    req.getFullParamStr(strParams);
    LogDebug("doGame8UShunt, header[%s], param[%s] post[%s]",
            headers.c_str(), strParams.c_str(), strPost.c_str());

    sem_init(&g_Game8USem, 0, 1);
    for (size_t i = 0; i < m_vecGame8UTransferUrl.size(); ++i)
    {
        CMyString strUrl = m_vecGame8UTransferUrl[i];
        LogDebug("[%s:%s:%d]strUrl: %s", __FILE__, __func__, __LINE__, strUrl.c_str());

        std::thread t1(TransferUrl, strUrl, strParams, &g_Game8USem, &vecGame8UResp);
        t1.detach();
    }

    int nCount = 0;
    while (vecGame8UResp.size() < m_vecGame8UTransferUrl.size())
    {
        sleep(1);
        nCount ++;
        if (nCount > 10)
        {
            LogDebug("[%s:%s:%d ncount > 10]", __FILE__, __func__, __LINE__);
            break;
        }
    }

    CMyString strResp;
    for (size_t i =0 ; i < vecGame8UResp.size(); ++i)
    {
        strResp = vecGame8UResp[i];
        if (strResp == "success" )
        {
            break;
        }
    }
    sem_destroy(&g_Game8USem);

    if (strResp != "success")
    {
        strResp = "fail";
    }

    safe_strcpy(szResp, cbResp, strResp.c_str());
    cbResp = strlen(szResp);
    LogDebug("[%s:%s:%d]doGame8UShunt, szResp[%s]", __FILE__, __func__, __LINE__, szResp);
    return 0;
}