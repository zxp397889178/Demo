/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  分流器
 *
 *        Version:  1.0
 *        Created:  2021年09月07日 11时00分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */



#include "Shunt.h"

static CPluginShunt* g_pShunt(NULL);
////////////////////////////////////////////////////////////////////
// Common Define
extern "C" int32_t Init(const CMyString &xmlFile, const CMyString &sect, struct PluginFunc& funcs)
{
    if (NULL == g_pShunt)
    {
        g_pShunt = new CPluginShunt();
    }
    if (NULL == g_pShunt)
    {
        LogFatal("[%s(%d):%s]insufficient memory when create shunt object.", 
                __FILE__, __LINE__, __func__);
        return -1;
    }

    int32_t ret = g_pShunt->Init(xmlFile, sect);
    if (ret <0)
    {
        LogFatal("[%s(%d):%s]initialize shunt object failure",
                __FILE__, __LINE__, __func__);
        return -3;
    }

    funcs.strUpdate = "Update";
    funcs.strGetEntry = "GetEntry";
    funcs.strMaintance = "Maintance";
    funcs.strFinial = "Release";

    LogMsg("initialize shunt object succeed.");
    return 0;
}

extern "C" void Update(void)
{
    if (!g_pShunt)
    {
        return;
    }
    g_pShunt->doUpdate();
}

extern "C" void GetEntry(VEC_ENTRY &vecEntry)
{
    if (NULL == g_pShunt)
    {
        return;
    }

    REG_CGI(vecEntry, "shunt/baiducallback", "doBaiduShunt");
    REG_CGI(vecEntry, "shunt/quickgamecallback", "doQuickGameShunt");
    REG_CGI(vecEntry, "shunt/quicksdkcallback", "doQuickSdkShunt");
    REG_CGI(vecEntry, "shunt/yybcallback", "doYybShunt");
    REG_CGI(vecEntry, "shunt/youyicallback", "doYouYiShunt");
    REG_CGI(vecEntry, "shunt/shunyoucallback", "doShunYouShunt");
    REG_CGI(vecEntry, "shunt/game8ucallback", "doGame8UShunt");
}

extern "C" void Maintance(void)
{
    if (NULL == g_pShunt)
    {
        return;
    }
    g_pShunt->doMaintance();
}

extern "C" void Release(void)
{
    SAFE_DELETE(g_pShunt);
}

ENTRY_CGI_IMPLEMENT(g_pShunt, doBaiduShunt)
ENTRY_CGI_IMPLEMENT(g_pShunt, doQuickGameShunt)
ENTRY_CGI_IMPLEMENT(g_pShunt, doQuickSdkShunt)
ENTRY_CGI_IMPLEMENT(g_pShunt, doYybShunt)
ENTRY_CGI_IMPLEMENT(g_pShunt, doYouYiShunt)
ENTRY_CGI_IMPLEMENT(g_pShunt, doShunYouShunt)
ENTRY_CGI_IMPLEMENT(g_pShunt, doGame8UShunt)