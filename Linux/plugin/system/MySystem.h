/*
 * =====================================================================================
 *
 *       Filename:  MySystem.h
 *
 *    Description:  系统操作
 *
 *        Version:  1.0
 *        Created:  11/20/2019 08:52:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#pragma once

#include "BaseCode.h"
#include "MyNet.h"
#include "MyDB.h"
#include "userv_mt/Config.h"
#include "userv_mt/IPlugin.h"

class CMySystem
{
public:
    CMySystem();
    ~CMySystem();

    int32_t Init(const CMyString &xmlFile, const CMyString &sect);
    void doMaintance(void);
    void doUpdate(void);

    ENTRY_CGI_DEFINE(doUpload);
    ENTRY_CGI_DEFINE(doDownload);
    ENTRY_CGI_DEFINE(doUrlTransfer);
    ENTRY_CGI_DEFINE(doEcho);
    ENTRY_CGI_DEFINE(doGetFile);

private:
    int32_t dealMultiPartFile(http::CHttpRequest &req, CMsgBuffer &respBuff );

    // 采用gzip压缩模式，分块发送文件
    int32_t gzip_chunk_send(FILE *fp, uint32_t fileSize, bool bTrailer, ILogic* pLogic, int64_t clientId);

    // 采用分块发送文件，bTrailer指示是否加md5校验
    void chunk_send(FILE *fp, uint32_t fileSize, bool bTrailer, ILogic* pLogic, int64_t clientId);

private:
    http::CHttpClient   m_http;

    bool        m_bTrailer;   // 使用Trailer: Content-MD5
};
