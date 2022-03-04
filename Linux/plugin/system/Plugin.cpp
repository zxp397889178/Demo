/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  系统插件
 *
 *        Version:  1.0
 *        Created:  11/20/2019 08:49:59 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "MySystem.h"


static CMySystem *g_pSystemObj(NULL);
////////////////////////////////////////////////////////////////////
// Common Define
extern "C" int32_t Init(const CMyString &xmlFile, const CMyString &sect, struct PluginFunc& funcs)
{
    if (NULL == g_pSystemObj)
    {
        g_pSystemObj = new CMySystem();
    }
    if (NULL == g_pSystemObj)
    {
        LogFatal("[%s(%d):%s]insufficient memory when create system object.", 
                __FILE__, __LINE__, __func__);
        return -1;
    }

    int32_t ret = g_pSystemObj->Init(xmlFile, sect);
    if (ret <0)
    {
        LogFatal("[%s(%d):%s]initialize system object failure",
                __FILE__, __LINE__, __func__);
        return -3;
    }

    funcs.strUpdate = "Update";
    funcs.strGetEntry = "GetEntry";
    funcs.strMaintance = "Maintance";
    funcs.strFinial = "Release";

    LogMsg("initialize system object succeed.");
    return 0;
}

extern "C" void Update(void)
{
    if (!g_pSystemObj)
    {
        return;
    }
    g_pSystemObj->doUpdate();
}

extern "C" void GetEntry(VEC_ENTRY &vecEntry)
{
    if (NULL == g_pSystemObj)
    {
        return;
    }

    REG_CGI(vecEntry, "api/upload",   "doUpload");
    REG_CGI(vecEntry, "api/download", "doDownload");
    REG_CGI(vecEntry, "api/getfile",  "doGetFile");
    REG_CGI(vecEntry, "api/transfer", "doUrlTransfer");
    REG_CGI(vecEntry, "echo", "doEcho");
}

extern "C" void Maintance(void)
{
    if (NULL == g_pSystemObj)
    {
        return;
    }
    g_pSystemObj->doMaintance();
}

extern "C" void Release(void)
{
    SAFE_DELETE(g_pSystemObj);
}

ENTRY_CGI_IMPLEMENT(g_pSystemObj, doUpload)
ENTRY_CGI_IMPLEMENT(g_pSystemObj, doDownload)
ENTRY_CGI_IMPLEMENT(g_pSystemObj, doUrlTransfer)
ENTRY_CGI_IMPLEMENT(g_pSystemObj, doEcho)
ENTRY_CGI_IMPLEMENT(g_pSystemObj, doGetFile)