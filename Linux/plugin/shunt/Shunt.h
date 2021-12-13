/*
 * =====================================================================================
 *
 *       Filename:  Shunt.h
 *
 *    Description:  分流器
 *
 *        Version:  1.0
 *        Created:  2021年09月07日 11时01分08秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#pragma once

#include "BaseCode.h"
#include "MyNet.h"
#include <vector>
#include "IPlugin.h"

struct st_param  
{  
    CMyString strUrl;
    CMyString strParam;
    CMyString strResp;
}; 

class CPluginShunt
{
public:
    CPluginShunt();
    virtual ~CPluginShunt();

    int32_t Init(const CMyString &xmlFile, const CMyString &sect);
    void doMaintance(void);
    void doUpdate(void);

public:
    ENTRY_CGI_DEFINE(doBaiduShunt);
    ENTRY_CGI_DEFINE(doQuickGameShunt);
    ENTRY_CGI_DEFINE(doQuickSdkShunt);
    ENTRY_CGI_DEFINE(doYybShunt);
    ENTRY_CGI_DEFINE(doYouYiShunt);
    ENTRY_CGI_DEFINE(doShunYouShunt);
    ENTRY_CGI_DEFINE(doGame8UShunt);

private:
    vector<CMyString> m_vecBaiduTransferUrl;
    vector<CMyString> m_vecQuickGameTransferUrl;
    vector<CMyString> m_vecQuickSdkTransferUrl;
    vector<CMyString> m_vecYybTransferUrl;
    vector<CMyString> m_vecYouYiTransferUrl;
    vector<CMyString> m_vecShunYouTransferUrl;
    vector<CMyString> m_vecGame8UTransferUrl;
};


