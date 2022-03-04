/*
 * =====================================================================================
 *
 *       Filename:  MySystem.cpp
 *
 *    Description:  系统操作定义
 *
 *        Version:  1.0
 *        Created:  11/20/2019 08:54:21 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "MySystem.h"
#include <openssl/md5.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../Common.h"
#include "MsgBuffer.h"

#define MIN_HTTPBUFFER_SIZE     1024

extern int32_t ImageResize(const CMyString &strFile, const CMyString &strThumb, const CMyString &strGeometry);
extern const char* md5str(byte_t *pData, size_t cbData, char *buffer);

CMyString getServerInfo(void)
{
    return "webmw-release/2.0.1";
}

/*
// 判断是否为文本类型
bool IsTextType(const CMyString &strContType)
{
    if (!strncasecmp(strContType.c_str(), "text/", 5))
    {
        return true;
    }
    if (strContType == "application/x-www-form-urlencoded"
        || strContType == "application/json" 
        || strContType == "multipart/form-data")
    {
        return true;
    }
    return false;
}
*/

////////////////////////////////////////////////////////////////////////
//CMySytem
CMySystem::CMySystem()
    : m_bTrailer(false)
{
    http::CHttpClient::GlobalInit();
}

CMySystem::~CMySystem()
{
    http::CHttpClient::GlobalFini();
}

int32_t CMySystem::Init(const CMyString &xmlFile, const CMyString &sect)
{
    mkdir("./upload", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    return 0;
}

void CMySystem::doUpdate(void)
{

}

void CMySystem::doMaintance(void)
{

}

void WriteResp(Json::Value &resp, CMsgBuffer &respBuff)
{
    CJsonWrapper writer;
    CMyString strResp = writer.write(resp);
    respBuff.Assign(strResp);
}

int32_t GetDispositionFileName(const CMyString &strDisposition, CMyString &strFile)
{
    size_t pos = strDisposition.find_first_of("filename");
    if (pos == std::string::npos)
    {
        return -1;
    }

    pos += 8;
    size_t pos1 = strDisposition.find_first_of('\"', pos);
    if (pos1 == std::string::npos)
    {
        // 解析是否有 '=';
        pos1 = strDisposition.find_first_of('=', pos);
        if (pos1 == std::string::npos)
        {
            return -3;
        }
        pos1 += 1;
        size_t pos2 = strDisposition.find_first_of(';', pos1);
        if (pos2 == std::string::npos)
        {
            strFile = strDisposition.substr(pos1);
            return 1;
        }
        else
        {
            strFile = strDisposition.substr(pos1, pos2-pos1);
            return 2;
        }
    }

    pos1 += 1;
    size_t pos2 = strDisposition.find_first_of('\"', pos1);
    if (pos2 == std::string::npos)
    {
        return -5;
    }
    strFile = strDisposition.substr(pos1, pos2-pos1);
    return 3;
}

int32_t ENTRY_CGI_DECLARE(CMySystem, doUpload)
{
    CMyString strContentType = req.getHeader("Content-Type");
    uint32_t contLength = req.getContentLength();
    LogDebug("DBG>Content-Type   [%s]", strContentType.c_str() );
    LogDebug("DBG>Content-Length [%u]", contLength);
    
    ext_headers["Content-Type"] = "application/json";

    // 采用多部份form上传
    size_t pos_m = strContentType.find("multipart/form-data");
    if (pos_m != std::string::npos)
    {
        int32_t ret = dealMultiPartFile(req, respBuff);
        return ret;
    }

    // 以下部份采用 完全传输，或者 chunked 传输
    // 获得文件名
    uint64_t current_tick = Now(true);
    CMyString strFile;
    CMyString strDisposition = req.getHeader("Content-Disposition");
    int32_t ret = GetDispositionFileName(strDisposition, strFile);
    if (ret< 0)
    {
        strFile = req.getParam("filename");
        if (strFile.empty())
        {
            strFile = req.getHeader("filename");
        }
        if (strFile.empty())
        {
            // 生成随机文件名
            strFile.Format("tmp-%lu", current_tick);
            //char temp_file[]="tmp_XXXXXX";
            //mktemp(temp_file);
            //strFile = temp_file;
        }
    }
    // 判断文件名是否存在"/"
    size_t pos = strFile.find_last_of('/');
    if (pos != std::string::npos)
    {
        strFile.erase(0, pos+1);
    }

    // 判断上传的文件名是否有后续，若没有，则以param传输的为准
    size_t posExt = strFile.find_last_of('.');
    if (posExt == std::string::npos)
    {
        CMyString strExt = req.getParam("ext");
        if (!strExt.empty())
        {
            strFile.Append(".%s", strExt.c_str());
        }
    }

    CMyString strFName("./upload/%s", strFile.c_str());
    // 判断文件名是否存在
#if 0
    if (access(strFName.c_str(), F_OK) != 0)
#else
    bool bFileExists(false);
    FILE *fp1 = fopen(strFName.c_str(), "r");
    if (fp1 != NULL)
    {
        bFileExists = true;
        fclose(fp1);
    }
    if (bFileExists)    
#endif
    {
        posExt = strFile.find_last_of(".");
        if (posExt == std::string::npos)
        {
            strFile.Append("_%lu", current_tick);
        }
        else
        {
            CMyString strName = strFile.substr(0, posExt);
            CMyString strExt = strFile.substr(posExt);
            strFile.Format("%s_%lu%s", strName.c_str(), current_tick, strExt.c_str());
        }
        strFName.Format("./upload/%s", strFile.c_str());
    }
    LogDebug("DBG>save data to file [%s]", strFName.c_str());


    CByteStream buff;
    req.getPost(buff);
    size_t cbBuff = buff.size();
    LogDebug("DBG>post buffer size [%lu]", cbBuff);

    Json::Value resp;
    if (cbBuff == 0)
    {
        resp["code"] = -1;
        resp["message"] = "not any data upload.";
        WriteResp(resp, respBuff);
        return 406;
    }

    FILE *fp = fopen(strFName.c_str(), "wb");
    if (NULL == fp)
    {
        resp["code"] = -3;
        resp["message"] = "save file to disk failure.";
        WriteResp(resp, respBuff);
        return 500;
    }
/*
    CMyString strMD5;
    if (req.IsChunked())
    {
        CMyString strTrailer = req.getHeader("Trailer");
        if (!strTrailer.empty())
        {
            strMD5 = req.getHeader(strTrailer.c_str());
        }
    }
*/

    // 分段写入, 并计算MD5
    MD5_CTX ctx;
    uint8_t md5[16] = {0,};
    MD5_Init(&ctx);
    uint8_t buffer[1024] = {0, };
    size_t cbData = sizeof(buffer);
    uint32_t totalWrite(0);
    do {
        if (buff.empty()) break;
        size_t readLen = std::min(cbData, buff.size());
        buff.Read(buffer, readLen);
        size_t size = fwrite(buffer, sizeof(uint8_t), readLen, fp);
        fflush(fp);
        totalWrite += size;
        MD5_Update(&ctx, buffer, readLen);
    } while(!buff.empty());
    MD5_Final(md5, &ctx);
    fflush(fp);
    fclose(fp);
    LogDebug("DBG>save data to %s succeed.", strFName.c_str());

    // 转换成md5
    char szMD5[64] = "";
    char *pc = &szMD5[0];
    for(int32_t i(0); i<(int32_t)sizeof(md5); ++i)
    {
        uint8_t c = md5[i];
        sprintf(pc+(i*2), "%02x", c);
    }
    LogDebug("DBG>MD5: %s", szMD5);

/*    
    // 判断MD5校验证是否匹配
    bool bMD5Chksum(true);
    if (req.IsChunked() && !strMD5.empty())
    {
        if (strMD5 != szMD5)
        {
            bMD5Chksum = false;
        }
    }
*/
    resp["code"] = 200;
    resp["message"] = "save file to disk ok";
    resp["size"] = (Json::Value::UInt)totalWrite;
    resp["md5"] = szMD5;
    resp["filename"] = strFile;
//    resp["chksum"] = bMD5Chksum ? "OK" : "FAIL";
    WriteResp(resp, respBuff);

    LogImpt("deal %s succeed, resp [%s].", __func__, respBuff.Str());
    return 200;
}

// http://ip:port/api/download?filename=abc.png&type=thumb
// http://ip:port/api/download?filename=abc.png
int32_t ENTRY_CGI_DECLARE(CMySystem, doDownload)
{
    CMyString strFile = req.getParam("filename");
    CMyString strFileType = req.getParam("type");
    ext_headers["Content-Type"] = "application/json";
    if (strFile.empty())
    {
        // 没有指定文件名，返回
        CMyString strResp("{\"code\":\"400\", \"message\":\"missing filename parameter\"}");
        respBuff.Assign(strResp);
        return 400;
    }
    LogDebug("filename=%s", strFile.c_str());
    CMyString strContentType;
    if (!GetContentType(strFile, strContentType))
    {
        LogWarn("uncatch content-type for file %s", strFile.c_str());
        strContentType = "text/plain";
    }

    // 查询缩略图
    bool bQueryThumb(false);
    CMyString strThumb = strFile;
    if (strContentType.find("image") != std::string::npos)
    {
        strFileType.MakeLower();
        if (strFileType=="thumb")
        {
            bQueryThumb = true;
            // 查是否存在缩略图，若不存在则生成之
            size_t pos = strThumb.find_last_of('.');
            if (pos != 0)
            {
                strThumb.insert(pos-1, "_thumb");
            }
            else
            {
                strThumb.Append("_thumb");
            }
            CMyString strTemp("./upload/%s", strThumb.c_str());
            CMyString strOrigin("./upload/%s", strFile.c_str());
            FILE *fp1 = fopen(strTemp.c_str(), "rb");
            if (fp1 == NULL)
            {
                ImageResize(strOrigin, strTemp, "120H");
            }
            else
            {
                fclose(fp1);
            }
        }
    }

    CMyString strFName("./upload/%s", strThumb.c_str());
    FILE *fp = fopen(strFName.c_str(), "rb");
    if (fp == NULL)
    {
        // 文件不存在
        LogErr("file [%s] not exists.", strThumb.c_str());
        CMyString str("{\"code\":\"404\", \"message\":\"file %s not exists\"}", strFile.c_str());
        respBuff.Assign(str);
        return 404;
    }
    CAutoFile guard(fp);
    fseek(fp, 0, SEEK_END);
    int32_t fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    CMyString strDisposition("attachment; filename=\"%s\"%s", 
            strFile.c_str(), (bQueryThumb ? ";thumbnail":" "));
    CMyString strEncoding = req.getHeader("Accept-Encoding"); //  [Accept-Encoding: gzip, deflate]
    bool bGzip(false);
/*    
 *    TODO: 暂不做压缩传输
    if (strEncoding.find("gzip") != std::string::npos)
    {
        bGzip = true;
    }
*/
    if (fileSize <=0 )
    {
        LogErr("file [%s] size is zero.", strThumb.c_str());
        CMyString str("{\"code\":\"500\", \"message\":\"file %s size is zero\"}", strFile.c_str());
        respBuff.Assign(str);
        return 500;
    }

    // 小文件直接发送
    if ((bGzip && fileSize<10240) || (!bGzip && fileSize < 2048))
    {
        ext_headers["Content-Type"] = strContentType;
        ext_headers["Content-Disposition"] = strDisposition;
        // ext_headers["Transfer-Encoding"] = "chunked";
        // ext_headers["Trailer"] = "Content-MD5";
        char szResp[1024*10] = {0};
        uint32_t cbResp = 10240;
        if (bGzip)
        {
            ext_headers["Content-Encoding"] = "gzip";
            CTempBuffer tmpbuf(fileSize+1);
            int32_t size3 = fread((void*)tmpbuf.data(), 1, fileSize, fp);
            gz_compress((byte_t*)tmpbuf.data(), size3, (byte_t*)&szResp[0], cbResp);
            respBuff.Assign(szResp);
        }
        else
        {
            cbResp = fread((void*)szResp, 1, fileSize, fp);
            respBuff.Assign(szResp);
        }
        return 200;
    }

    // 大文件，分批次发送
    CMyString strHeader("HTTP/1.1 200 OK\r\n");
    strHeader.Append("Server: webmw-alpha/1.0.27\r\n");
    strHeader.Append("Date: %s\r\n", getGMTDate(::time(NULL)).c_str());
    strHeader.Append("Connection: keep-alive\r\n");
    strHeader.Append("Content-Type: %s\r\n", strContentType.c_str());
    strHeader.Append("Transfer-Encoding: chunked\r\n");
    strHeader.Append("Content-Disposition: attachment; filename=\"%s\"%s\r\n", 
            strFile.c_str(), (bQueryThumb ? ";thumbnail":" "));
    if (m_bTrailer)
    {
        strHeader.Append("Trailer: Content-MD5\r\n");
    }
    if (bGzip)
    {
        strHeader.Append("Content-Encoding: gzip\r\n");
    }
    strHeader.Append("\r\n");
    pLogic->SendMsg(clientId, (void*)strHeader.c_str(), strHeader.size());
    LogDebug("send http resp header, size %lu", strHeader.size());

    // 文件以chunked 模式下发给前端
    // 循环发送文件，以1KB 为block
    // TODO: 需要支持gzip压缩
    MD5_CTX ctx;
    uint8_t md5[16] = {0,};
    MD5_Init(&ctx);
    fseek(fp, 0, SEEK_SET);

    int32_t totalSize(0);
    char szHeader[64] = "";
    byte_t chunk[1024] = {0, };
    char szChunkEndl[32] = "\r\n";
    if (bGzip)
    {
        // 需先进行压缩，然后再解压 
        // refer  https://en.wikipedia.org/wiki/Chunked_transfer_encoding
        CTempBuffer tmpbuf(fileSize+1);
        char *pData = tmpbuf.data();
        int32_t size1 = fread(pData, 1, fileSize, fp);

        uint32_t size2 = size1;
        CTempBuffer tmpbuf2(size2+1);
        char *pOut = tmpbuf.data();

        gz_compress((byte_t*)pData, size1, (byte_t*)pOut, size2);

        int32_t left(size2);
        do {
            int32_t size4 = std::min(left, (int32_t)sizeof(chunk));
            memcpy(chunk, pOut, size4 );
            MD5_Update(&ctx, chunk, size4);
            totalSize += size4;
            sprintf(szHeader, "%x\r\n", size4);
            pLogic->SendMsg(clientId, (void*)&szHeader[0], strlen(szHeader));
            pLogic->SendMsg(clientId, chunk, size1);
            pLogic->SendMsg(clientId, (void*)&szChunkEndl[0], 2);
            left -= size4;
        }while(left>0);
    }
    else
    {
        // 无压缩分块传输
        do {
            int32_t size1 = fread(chunk, 1, sizeof(chunk), fp);
            MD5_Update(&ctx, chunk, size1);
            totalSize += size1;
            sprintf(szHeader, "%x\r\n", size1);
            pLogic->SendMsg(clientId, (void*)&szHeader[0], strlen(szHeader));
            pLogic->SendMsg(clientId, chunk, size1);
            pLogic->SendMsg(clientId, (void*)&szChunkEndl[0], 2);
        }while(!feof(fp));
    }
    MD5_Final(md5, &ctx);

    char szChunkFinish[32] = "";
    strcpy(szChunkFinish, "0\r\n\r\n");  
    pLogic->SendMsg(clientId, (void*)&szChunkFinish[0], strlen(szChunkFinish));
    LogDebug("total send %d bytes.", totalSize);

    // 发送尾部的md5
    char szContentMD5[128] = "";
    char *pc = &szContentMD5[0];
    if (m_bTrailer)
    {
        strcpy(szContentMD5, "Content-MD5: ");
        pc = &szContentMD5[strlen(szContentMD5)];
    }
    for(int32_t i(0); i<(int32_t)sizeof(md5); ++i)
    {
        uint8_t c = md5[i];
        sprintf(pc+(i*2), "%02x", c); 
    }
    LogDebug(szContentMD5);
    if (m_bTrailer)
    {
        strcat(szContentMD5, "\r\n");
        pLogic->SendMsg(clientId, (void*)&szContentMD5[0], strlen(szContentMD5));
    }
    
    LogDebug("%s deal finished.", __func__);
    return 200;
}

int32_t ENTRY_CGI_DECLARE(CMySystem, doUrlTransfer)
{
    uint64_t timeBegin(Now());
    CMyString strHost = req.getHeader("9527_transfer_host");
    CMyString strCgi = req.getHeader("9527_transfer_url");

    Json::Value resp;
    if (strHost.empty() || strCgi.empty())
    {
        resp["code"] = -1;
        resp["message"] = "not valid transfer req.";
        WriteResp(resp, respBuff);
        ext_headers["Content-Type"] = "application/json";
        LogWarn("TRAN>host or cgi is empty.");
        return -1;
    }
    //req.removeHeader("9527_transfer_host");
    //req.removeHeader("9527_transfer_url");

    // req 转换 请求,只处理 GET/POST
    CMyString strMethod = req.getMethod();
    CMyString strParamStr;
    req.getFullParamStr(strParamStr);
    CMyString strUrl;
    if (strParamStr.empty())
        strUrl.Format("%s/%s", strHost.c_str(), strCgi.c_str());
    else
        strUrl.Format("%s/%s?%s", strHost.c_str(), strCgi.c_str(), strParamStr.c_str());

    // 获得cookies
    CMyString strCookie = req.getFullCookie();
    // 获得headers
    http::CHttpData::NodeVector headers = req.GetHeads();
    std::vector<std::string> vecHeader;
    http::CHttpData::NodeVector::iterator it(headers.begin());
    for(;it != headers.end(); ++it)
    {
        http::CHttpData::Node &node = *it;
        std::string strLine = node.name;
        strLine += ":";
        strLine += node.value;
        vecHeader.push_back(strLine);
    }

    int32_t ret(0);
    CMyString strResp;
    CMyString strHeader;
    // TODO: 需要处理RESP HEADER
    if (strMethod == "GET")
    {
         ret = m_http.HttpGetWithHeaderCookie(strUrl.c_str(), 
                 strResp, strHeader, vecHeader, strCookie.c_str());
    }
    else if (strMethod == "POST")
    {
        // 获得post数据
        CByteStream buff = req.GetPost();
        CMyString strBody;
        buff.Read(strBody);
        ret = m_http.HttpPostWithHeaderCookie(strUrl.c_str(), 
                strResp, strHeader, 
                vecHeader, strCookie.c_str(), strBody.c_str());
    }
    else
    {
        resp["code"] = -3;
        resp["message"] = CMyString("unsupport method %s", strMethod.c_str());
        WriteResp(resp, respBuff);
        ext_headers["Content-Type"] = "application/json";
        return -3;
    }

    uint64_t timeEnd(Now());
    if (ret >=0)
    {
        respBuff.Assign(strResp);
    }
    else
    {
        resp["code"] = ret;
        resp["message"] = CMyString("call cgi failure, cost %d ms", (int32_t)(timeEnd - timeBegin));
        WriteResp(resp, respBuff);
        ext_headers["Content-Type"] = "application/json";
    }

    LogDebug("execute cgi[%s] %s, cost %dms",
            strCgi.c_str(), 
            (ret>=0 ? "succeed" : "failure"), 
            (int32_t)(timeEnd-timeBegin));
    return ret;
}

CMyString NowToStr(uint64_t now, bool bMicroSecond)
{
    time_t t1(0);
    uint64_t us(0);
    if (bMicroSecond)
    {
        t1 = now/1000000;
        us = now%1000000;
    } 
    else
    {
        t1 = now/1000;
        us = (now%1000)*1000;   
    }

    struct tm t2;
    localtime_r(&t1, &t2);
    CMyString str("%04d-%02d-%02d %02d:%02d:%02d.%-3d",
            t2.tm_year+1900, t2.tm_mon+1, t2.tm_mday,
            t2.tm_hour, t2.tm_min, t2.tm_sec,
            (int32_t)(us/1000));
    return str;
}

int32_t ENTRY_CGI_DECLARE(CMySystem, doEcho)
{
    // ext_headers["Content-Type"] = "application/json";

    // time_t t1 = ::time(NULL);
    // uint64_t now = Now(true);
    // uint64_t create = pClient->getCreateTime();
    
    // Json::Value resp;
    // resp["date"] = getGMTDate(t1);
    // resp["now"] = NowToStr(now, true);
    // resp["create"] = NowToStr(create, true);
    // resp["cost"] = CMyString("%d us", (int32_t)(now-create));
    // CJsonWrapper writer;
    // CMyString strResp = writer.write(resp);
    // safe_strcpy(respBuff, strResp.c_str());
    // cbResp = strlen(szResp);


    return 0;
}

int32_t CMySystem::dealMultiPartFile(http::CHttpRequest &req, CMsgBuffer &respBuff)
{
    LogDebug("deal multipart/form-data file.");
    Json::Value resp;
    CMyString strFileInd = req.getParam("MultiFileIndicator");
    if (strFileInd.empty())
    {
        resp["code"] = 500;
        resp["message"] = "not found param \"MultiFileIndicator\" ";
        CJsonWrapper writer;
        CMyString strResp = writer.write(resp);
        respBuff.Assign(strResp);

        return 500;
    }

    // filename/size;filename/size
    CByteStream &buff = req.GetPost();
    CMyString strLine;
    bool bLast(false);
    bool bError(false);
    char szFile[256] = "";
    int32_t nFileSize(0);
    Json::Value filedata_arr;
    filedata_arr.resize(0);
    do {
        size_t pos = strFileInd.find_first_of(';');
        if (pos == std::string::npos)
        {
            strLine = strFileInd;
            bLast = true;
        }
        else
        {
            strLine = strFileInd.substr(0, pos);
            strFileInd = strFileInd.substr(pos+1);
        }

        // 获得文件名和大小
        int32_t ret = sscanf(strLine.c_str(), "%[^/]/%d", szFile, &nFileSize);
        if (ret != 2)
        {
            bError = true;
            break;
        }

        // 存储文件
        CMyString strFile(szFile);
        CMyString strPath("./upload/%s", szFile);
        FILE *fp = fopen(strPath.c_str(), "rb");
        if (fp != NULL)
        {
            fclose(fp);
            // 已存文件名，另存一个
            uint64_t now = Now(false);
            CMyString strTemp("_%lu", now);
            size_t posExt = strFile.find_last_of('.');
            if (posExt != std::string::npos)
            {
                strFile.insert(posExt, strTemp.c_str());
            }
            else
            {
                strFile.Append(strTemp.c_str());
            }
            strPath.Format("./upload/%s", strFile.c_str());
        }

        // 写入本地文件系统
        fp = fopen(strPath.c_str(), "wb");
        if (NULL != fp)
        {
            MD5_CTX ctx;
            uint8_t md5[16] = {0,}; 
            MD5_Init(&ctx);

            CTempBuffer tmpbuf(nFileSize+1);
            buff.Read((byte_t*)tmpbuf.data(), nFileSize);
            size_t wSize = fwrite(tmpbuf.data(), 1, nFileSize, fp);
            fflush(fp);
            fclose(fp);
            MD5_Update(&ctx, tmpbuf.data(), wSize); 
            MD5_Final(md5, &ctx);

            Json::Value filedata;
            filedata["filename"] = strFile.c_str();
            filedata["size"] = nFileSize;
            char szMD5[64] = "";
            filedata["md5"] = md5str(md5, sizeof(md5), szMD5);
            filedata_arr.append(filedata);
        }

    }while(!bLast && !bError && !buff.empty());

    int32_t result(500);
    if (bError)
    {
        resp["code"] = 500;
        resp["message"] = "\'MultiFileIndicator\' field failure.";
    }
    else
    {
        resp["code"] = 200;
        resp["message"] = "save file to disk ok";
        result = 200;
    }
    resp["data"] = filedata_arr;
    CJsonWrapper writer;
    CMyString strResp = writer.write(resp);
    respBuff.Assign(strResp);

    return result;
}

const char* md5str(byte_t *pData, size_t cbData, char *buffer)
{
    for(size_t i(0); i<cbData; ++i)
    {
        uint8_t c = pData[i];
        sprintf(buffer + (i*2), "%02x", c);
    }
    return buffer;
}

/* http://ip:port/api/getfile?filename=./supergm_web/qrcode/abc.jpg&download=1
 * GET
 */
int32_t CMySystem::gzip_chunk_send(FILE *fp, uint32_t fileSize, bool bTrailer, ILogic* pLogic, int64_t clientId)
{
    uint64_t t1(Now(true));

    MD5_CTX ctx;
    uint8_t md5[16] = {0,};
    if (bTrailer)
    {
        MD5_Init(&ctx);
    }

    // 压缩模式：必须先压缩，然后再分块发送
    byte_t *pData = (byte_t*)calloc(1, fileSize);
    CAutoArrayEx guard1(pData);

    byte_t *pZData = (byte_t*)calloc(1, fileSize);
    CAutoArrayEx guard2(pZData);

    fseek(fp, 0, SEEK_SET);
    fread(pData, 1, fileSize, fp);

    uint32_t nZData(fileSize);
    int32_t ret = gzip::compress(pData, fileSize, pZData, &nZData);
    if (ret != gzip::err_OK)
    {
        // 压缩失败，采用传缩的发送方式
        return -1;
    }

    // 分块传输
    int32_t totalSize(nZData);
    int32_t sizeLeft(nZData);
    char szHeader[64] = "";
    byte_t *chunk(pZData);
    char szChunkEndl[32] = "\r\n";
    do {
        int32_t size1 = sizeLeft>1024 ? 1024 : sizeLeft;
        sizeLeft -= size1;
        if (bTrailer)
        {
            MD5_Update(&ctx, chunk, size1);
        }
        sprintf(szHeader, "%x\r\n", size1);
        pLogic->SendMsg(clientId, (void*)&szHeader[0], strlen(szHeader));
        pLogic->SendMsg(clientId, chunk, size1);
        pLogic->SendMsg(clientId, (void*)&szChunkEndl[0], 2);
        chunk += size1;
        totalSize += strlen(szHeader)+2;
    }while(sizeLeft>0);
    if (bTrailer)
    {
        MD5_Final(md5, &ctx);
    }

    char szChunkFinish[32] = "";
    strcpy(szChunkFinish, "0\r\n\r\n");  
    pLogic->SendMsg(clientId, (void*)&szChunkFinish[0], strlen(szChunkFinish));
    totalSize += 5;

    // 发送尾部的md5
    if (bTrailer)
    {
        char szContentMD5[128] = "";
        char *pc = &szContentMD5[0];
        strcpy(szContentMD5, "Content-MD5: ");
        pc = &szContentMD5[strlen(szContentMD5)];
        for(int32_t i(0); i<(int32_t)sizeof(md5); ++i)
        {
            uint8_t c = md5[i];
            sprintf(pc+(i*2), "%02x", c); 
        }
        LogDebug(szContentMD5);
        strcat(szContentMD5, "\r\n");
        pLogic->SendMsg(clientId, (void*)&szContentMD5[0], strlen(szContentMD5));
        totalSize += strlen(szContentMD5);
    }

    uint64_t t2(Now(true));
    LogDebug("%sgzip-chunked%s send %d bytes, cost %s%.2lfms%s.", 
            _color_red, _color_end, totalSize,
            _color_red, (t2-t1)/1000.0, _color_end);
    return 0;
}

void CMySystem::chunk_send(FILE *fp, uint32_t fileSize, bool bTrailer, ILogic* pLogic, int64_t clientId)
{
    uint64_t t1(Now(true));

    // 文件以chunked 模式下发给前端
    // 循环发送文件，以1KB 为block
    MD5_CTX ctx;
    uint8_t md5[16] = {0,};
    if (bTrailer)
    {
        MD5_Init(&ctx);
    }

    fseek(fp, 0, SEEK_SET);

    int32_t totalSize(0);
    char szHeader[64] = "";
    byte_t chunk[1024] = {0, };
    char szChunkEndl[32] = "\r\n";
    // 无压缩分块传输
    do {
        int32_t size1 = fread(chunk, 1, sizeof(chunk), fp);
        if (bTrailer)
        {
            MD5_Update(&ctx, chunk, size1);
        }
        sprintf(szHeader, "%x\r\n", size1);
        pLogic->SendMsg(clientId, (void*)&szHeader[0], strlen(szHeader));
        pLogic->SendMsg(clientId, chunk, size1);
        pLogic->SendMsg(clientId, (void*)&szChunkEndl[0], 2);
        totalSize += size1 + strlen(szHeader)+2;
    }while(!feof(fp));
    if (bTrailer)
    {
        MD5_Final(md5, &ctx);
    }

    char szChunkFinish[32] = "";
    strcpy(szChunkFinish, "0\r\n\r\n");  
    pLogic->SendMsg(clientId, (void*)&szChunkFinish[0], strlen(szChunkFinish));
    totalSize += 5;

    // 发送尾部的md5
    if (bTrailer)
    {
        char szContentMD5[128] = "";
        char *pc = &szContentMD5[0];
        strcpy(szContentMD5, "Content-MD5: ");
        pc = &szContentMD5[strlen(szContentMD5)];
        for(int32_t i(0); i<(int32_t)sizeof(md5); ++i)
        {
            uint8_t c = md5[i];
            sprintf(pc+(i*2), "%02x", c); 
        }
        LogDebug(szContentMD5);
        strcat(szContentMD5, "\r\n");
        pLogic->SendMsg(clientId, (void*)&szContentMD5[0], strlen(szContentMD5));
        totalSize += strlen(szContentMD5);
    }

    uint64_t t2(Now(true));
    LogDebug("%schunked%s send %d bytes, cost %s%.2lfms%s.", 
            _color_red, _color_end, totalSize, 
            _color_red,(t2-t1)/1000.0, _color_end);
}

int32_t ENTRY_CGI_DECLARE(CMySystem, doGetFile)
{
    CMyString strFile = req.getParam("filename");
    ext_headers["Content-Type"] = "application/json";
    if (strFile.empty())
    {
        // 没有指定文件名，返回
        CMyString strResp = "{\"code\":\"400\", \"message\":\"missing filename parameter\"}";
        respBuff.Assign(strResp);
        return 400;
    }
    bool bDownload(false);
    req.getParam("download", bDownload);

    // 判断文件是否存在
    FILE *fp = fopen(strFile.c_str(), "rb");
    if (fp == NULL)
    {
        LogErr("file [%s] not exists.", strFile.c_str());
        CMyString str("{\"code\":\"404\", \"message\":\"file %s not exists\"}", strFile.c_str());
        respBuff.Assign(str);
        return 404;
    }
    CAutoFile guard(fp);

    // 获得文件扩展信息
    CFileInfo finfo;
    finfo.Open(fp);
    LogDebug("filename=%s", finfo.GetFullPath());
    CMyString strContentType;
    if (!GetContentType(strFile, strContentType))
    {
        LogWarn("uncatch content-type for file %s", finfo.GetFullPath());
        strContentType = "text/plain";
    }

    fseek(fp, 0, SEEK_END);
    int32_t fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (fileSize <=0 )
    {
        LogErr("file [%s] size is zero.", finfo.GetFullPath());
        CMyString str("{\"code\":\"500\", \"message\":\"file %s size is zero\"}", finfo.GetFName());
        respBuff.Assign(str);
        return 500;
    }

    // 判断文件大小，若超过1K则压缩发送
    if (fileSize < MIN_HTTPBUFFER_SIZE)
    {
        uint64_t t1(Now(true));
        // 发送文件头
        CMyString strHeader("HTTP/1.1 200 OK\r\n"
                "Server: %s\r\n"
                "Data: %s\r\n"
                "Connection: close\r\n"
                "Content-Type: %s\r\n",
                getServerInfo().c_str(),
                getGMTDate(::time(NULL)).c_str(),
                strContentType.c_str());
        if (bDownload)
        {
            strHeader.Append("Content-Disposition: attachment; filename=\"%s\"\r\n", finfo.GetFName());
        }
        strHeader.Append("\r\n");
        pLogic->SendMsg(clientId, (void*)strHeader.c_str(), strHeader.size());
        // 发送文件数据
        fseek(fp, 0, SEEK_SET);
        char szResp[1024*10] = {0};
        int32_t cbResp = 10240;
        cbResp = fread((void*)szResp, 1, fileSize, fp);
        respBuff.Assign(szResp);
        uint64_t t2(Now(true));
        LogDebug("%sonce%s send %d bytes, cost %s%dus%s", 
                _color_red, _color_end, fileSize,
                _color_red, (int32_t)(t2-t1), _color_end);
        return 200;
    }

    // 若文件是文本类型，则可以启用压缩
    bool bGZip(false);
    // if (IsTextType(strContentType))
    // {
    //     bGZip = true;
    // }

    // 分批次发送
    CMyString strHeader("HTTP/1.1 200 OK\r\n"
            "Server: %s\r\n"
            "Date: %s\r\n"
            "Connection: keep-alive\r\n"
            "Content-Type: %s\r\n"
            "Transfer-Encoding: chunked\r\n",
            getServerInfo().c_str(),
            getGMTDate(::time(NULL)).c_str(),
            strContentType.c_str()
            );
    if (bDownload)
    {
        strHeader.Append("Content-Disposition: attachment; filename=\"%s\"\r\n", finfo.GetFName());
    }
    if (bGZip)
    {
        strHeader.Append("Content-Encoding: gzip\r\n");
    }
    strHeader.Append("\r\n");
    pLogic->SendMsg(clientId, (void*)strHeader.c_str(), strHeader.size());
    LogDebug("send http resp header, size %lu", strHeader.size());

    if (bGZip)
    {
        int32_t ret = this->gzip_chunk_send(fp, fileSize, m_bTrailer, pLogic, clientId);
        if (ret >= 0)
        {
            return 200;
        }
    }

    this->chunk_send(fp, fileSize, m_bTrailer, pLogic, clientId);
    //LogDebug("%s deal finished.", __func__);
    return 200;
}


