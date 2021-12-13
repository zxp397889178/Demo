/*
 * =====================================================================================
 *
 *       Filename:  DotWalletPay.cpp
 *
 *    Description:  数字钱包支付
 *
 *        Version:  1.0
 *        Created:  2021年11月30日 19时16分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  <Your Full Name> (<Your Nick Name>), <your e-mail>
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */



#include "DotWalletPay.h"
#include "Security.h"
#include "SdkList.h"
#include "GameList.h"
#include "SdkChannelList.h"
#include "MyNet.h"
#include "Config.h"
#include "BillPointList.h"
#include <algorithm>
#include "Security/CryptHelper.h"
#include "ecdsa_secp256k1.h"

CDotWalletPay::CDotWalletPay()
    : CPayBase("DotWalletPay", depositRequest, httpRequestSync)
{
    m_nExpiresIn = 0;
    m_nTimeout = 0;
}

CDotWalletPay::~CDotWalletPay()
{

}

int32_t CDotWalletPay::Init(void)
{
    int32_t ret = this->Update();
    return ret;
}

int32_t CDotWalletPay::Update(void)
{   
    m_mapAddress.clear();

    CMyString strAddress;
    CXmlFile &xml = CONFIG.getXml();
    CMyString sect = "main/dotwallet/config/address";
    int32_t nRet = xml.getString(sect, "BSV", strAddress);
    if (nRet == 0)
    {
        m_mapAddress["BSV"] = strAddress;
    }
    else
    {
        return -1;
    }
    strAddress.clear();
    nRet = xml.getString(sect, "BTC", strAddress);
    if (nRet == 0)
    {
        m_mapAddress["BTC"] = strAddress;
    }
    else
    {
        return -2;
    }
    strAddress.clear();
    nRet = xml.getString(sect, "ETH", strAddress);
    if (nRet == 0)
    {
        m_mapAddress["ETH"] = strAddress;
    }
    else
    {
        return -3;
    }

    CMyString sect2 = "main/dotwallet/config/auth_token";
    nRet = xml.getString(sect2, "url", m_strAuthUrl);
    if (nRet != 0)
    {
        return -4;
    }
    nRet = xml.getString(sect2, "client_id", m_strClientId);
    nRet = xml.getString(sect2, "client_secret", m_strClientSecret);


    CMyString sect3 = "main/dotwallet/config/payurl";
    nRet = xml.getString(sect3, "url", m_strPayUrl);
    if (nRet != 0)
    {
        return -5;
    }

    CMyString sect4 = "main/dotwallet/config/trans_url";
    nRet = xml.getString(sect4, "url", m_strTransUrl);
    if (nRet != 0)
    {
        return -6;
    }

    CMyString sect5 = "main/dotwallet/config/redis_key";
    nRet = xml.getString(sect5, "deposit_keyFormat", m_strDepositKeyFormat);
    if (nRet != 0)
    {
        return -7;
    }
    nRet = xml.getString(sect5, "credit_keyFormat", m_strCreditKeyFormat);
    if (nRet != 0)
    {
        return -8;
    }
    nRet = xml.getInt32(sect5, "timeout", m_nTimeout);

    CMyString sect6 = "main/dotwallet/config/user_receive_adress_url";
    nRet = xml.getString(sect6, "url", m_strUserAddressUrl);
    if (nRet != 0)
    {
        return -9;
    }

    GetRate();
    
    // 启动定时器列表
    m_timerRefershRate.SetInterval(60000); //一分钟轮询一次
    m_timerRefershRate.Startup();

    return CPayBase::Init("main/dotwallet");
}

void CDotWalletPay::Maintance(void)
{
    CPayBase::Maintance();

    if (m_timerRefershRate.ToNextTime())
    {
        GetRate();
    }
}

ENTRY_FUNC_DECLARE(CDotWalletPay, doDeposit)
{
    return CPayBase::doDepositRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CDotWalletPay, doDepositCB)
{
    // CMyString strSign1 = req.getParam("signature");
    // CMyString strPayLoad = req.getParam("payload");
    // CMyString strPubKey = req.getParam("pubkey");
    
    // std::vector<uint8_t> vchSig;
    // ECDSAWrapper::readhexEx(vchSig, strSign1.c_str());
    // std::vector<uint8_t> vchPub;
    // ECDSAWrapper::readhexEx(vchPub, strPubKey.c_str());

    // int bRet = ECDSAWrapper::verify(vchSig, strPayLoad.c_str(), vchPub);
    // // 验证签名是否匹配，忽略大小写
    // if (bRet == 0)
    // {
    //     LogErr("[%s]deposit callback sign mismatch, payload[%s], signature[%s], pubkey[%s]",
    //             m_sdkTag.c_str(), strPayLoad.c_str(), strSign1.c_str(), strPubKey.c_str());
        
    // }
    // else
    // {
    //     LogWarn("[%s]deposit callback sign success, payload[%s], signature[%s], pubkey[%s]",
    //             m_sdkTag.c_str(), strPayLoad.c_str(), strSign1.c_str(), strPubKey.c_str());
    // }

    // return 200;
    return CPayBase::doDepositCallback(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CDotWalletPay, doWithdraw)
{
    CByteStream buff;
    req.getPost(buff);
    CMyString strPost;
    buff.Read(strPost);
    CMyString headers = req.getFullUrl();
    CMyString params;
    req.getFullParamStr(params);
    LogDebug("[%s]doWithdraw, header[%s], param[%s] post[%s]",
            m_sdkTag.c_str(), headers.c_str(), params.c_str(), strPost.c_str());

    return CPayBase::doCreditRequest(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CDotWalletPay, doWithdrawCB)
{
    return CPayBase::doCreditCallback(pClient, req, szResp, cbResp, ext_headers);
}

ENTRY_FUNC_DECLARE(CDotWalletPay, doGetRate)
{
    ext_headers["Content-Type"] = "application/json";
    
    CJsonWrapper js(m_jsRate);
    safe_strcpy(szResp, cbResp, js.toString().c_str());
    cbResp = strlen(szResp);
    return 200;
}


void CDotWalletPay::doMakeDepositRequest(http::CHttpRequest& req, CGame* pGame, CSdk *pSdk, CSdkChannel *pChannel, const CMyString &orderNo, CMyString& url, CMyString &contentType, CMyString& httpData)
{
    CMyString strCoinType = req.getParam("productId");  //传coin_type
    int32_t amount = req.getParamInt("amount"); //分为单位
    int64_t nCoin = this->GetRMBTransCoin(amount, strCoinType);

    Json::Value data;
    Json::Value to(Json::arrayValue);
    Json::Value product(Json::objectValue);

    data["out_order_id"] = orderNo;
    data["coin_type"] = strCoinType;
    map<CMyString, CMyString>::iterator it_find = m_mapAddress.find(strCoinType);
    if (it_find != m_mapAddress.end())
    {
        Json::Value item;
        item["type"] = "address";
        item["content"] = it_find->second;
        item["amount"] = nCoin;
        to.append(item);
    }
    data["to"] = to;

    CMyString strProductTitle;
    int32_t bpId = req.getParamInt("bpId");
    CBillPoint *pBP = BILLPOINT_LIST.Find(bpId);
    if (NULL != pBP)
    {
        strProductTitle = pBP->getBpCode();

        product["id"] = CMyString("%d", pBP->getId());
        product["name"] = strProductTitle;
    }
    else
    {
        LogErrAlt("[%s]CDotWalletPay::doMakeDepositRequest >not found billpoint %d", m_sdkTag.c_str(), bpId);
    }
    data["product"] = product;
    data["subject"] = strProductTitle;
    data["notify_url"] = pSdk->getPayCallbackUrl();
    data["redirect_uri"] = pGame->getCallbackUrl();

    CMyDateTime tNow;
    data["expires"] = tNow.time() + m_nTimeout;

    CMyString strClientId, strClientSecret;
    Json::Value extraData = pChannel->getKeyData();
    if (!extraData.empty())
    {
        strClientId = extraData["appid"].asString();
        strClientSecret = extraData["appkey"].asString();
        LogDebug("[%s:%s:%d] strClientId: %s, strClientSecret:%s", __FILE__, __func__, __LINE__, strClientId.c_str(), strClientSecret.c_str());
    }

    {
        Json::Value redisData;
        redisData["coin_type"] = strCoinType;
        redisData["amount"] = amount;
        redisData["coin"] = nCoin;
        redisData["orderNo"] = orderNo;
        CJsonWrapper jwRedis(redisData);
        CMyString strRedisValue = jwRedis.toString();
        CMyString strRedisKey(m_strDepositKeyFormat.c_str(), orderNo.c_str());
        m_redis.Set(strRedisKey, strRedisValue, m_nTimeout, 1);
        LogMsg("[%s:%s:%d] strRedisKey: %s, strRedisValue:%s", __FILE__, __func__, __LINE__, strRedisKey.c_str(), strRedisValue.c_str());
    }

    CJsonWrapper jwData(data);
    httpData = jwData.toString();
    url = pSdk->getPayUrl();
    contentType = "Content-Type: application/json;Authorization: Bearer ";
    contentType += this->GetAccessToken(strClientId, strClientSecret);
}

bool CDotWalletPay::doParseDepositResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &depositUrlData, CMyString &depositOrderNo, CMyString &errNote)
{
    LogDebug("[%s]deposit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());
    CJsonWrapper jrRoot;
    if (!jrRoot.Init(respData))
    {
        LogErr("[%s:%s:%d] resp data [%s] not json string", __FILE__, __func__, __LINE__, respData.c_str());
        errNote.Format("resp data [%s] not json string", respData.c_str());
        return false;
    }

    int32_t nCode = jrRoot.getInt("code");
    if (nCode != 0)
    {
        LogErr("[%s:%s:%d] resp code is %d", __FILE__, __func__, __LINE__, nCode);
        CMyString strMsg = jrRoot.getString("msg");
        errNote.Format("resp code is %d, msg:%s", nCode, strMsg.c_str());
        return false;
    }

    CMyString strData = jrRoot.getString("data");
    depositOrderNo = strData;
    depositUrlData = m_strPayUrl + strData;
    return true;
}


bool CDotWalletPay::doParseDepositCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString& platformOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &depositOK)
{
    CByteStream buff;
    req.getPost(buff);
    CMyString strPost;
    buff.Read(strPost);
    CMyString headers = req.getFullUrl();
    CMyString params;
    req.getFullParamStr(params);
    LogDebug("[%s]deposit callback, header[%s], param[%s] post[%s]",
            m_sdkTag.c_str(), headers.c_str(), params.c_str(), strPost.c_str());

    CMyString str1 = req.getParam("payload");
    if (str1.empty())
    {
        LogWarn("[%s]CDotWalletPay::doParseDepositCallback payload is empty", m_sdkTag.c_str());
        return false;
    }

    CJsonWrapper jrParam;
    if (!jrParam.Init(str1))
    {
        LogErr("[%s]CDotWalletPay::doParseDepositCallback parse json err. payload: %s", m_sdkTag.c_str(), str1.c_str());
        return false;
    }

    orderNo = jrParam.getString("out_order_id");
    platformOrderNo = jrParam.getString("order_id");
    int64_t nCoinFee = (int64_t)(jrParam.getString("amount").asInt64()); //交易金额，单位：聪 satoshi(bsv)或者 gwei （eth)

    CMyString strRedisValue;
    CMyString strRedisKey(m_strDepositKeyFormat.c_str(), orderNo.c_str());
    if (m_redis.Get(strRedisKey, strRedisValue) != 1)
    {
        LogErr("[%s]CDotWalletPay::doParseDepositCallback order err, redis not find, strRedisKey:%s", m_sdkTag.c_str(), strRedisKey.c_str());
        return false;
    }
    LogMsg("[%s:%s:%d] strRedisKey: %s, strRedisValue:%s", __FILE__, __func__, __LINE__, strRedisKey.c_str(), strRedisValue.c_str());
    CJsonWrapper JsRedis;
    if (!JsRedis.Init(strRedisValue))
    {
        LogErr("[%s]CDotWalletPay::doParseDepositCallback redisvalue parse json err. strRedisValue: %s", m_sdkTag.c_str(), strRedisValue.c_str());
        return false;
    }

    int64_t nRedisCoin = JsRedis.getUInt64("coin");
    LogDebug("[%s] [%s:%d] coin range: %u - %u", m_sdkTag.c_str(), __FILE__, __LINE__, uint32_t(nRedisCoin * 0.9), uint32_t(nRedisCoin * 1.1));
    if (nCoinFee < uint32_t(nRedisCoin * 0.9) || nCoinFee > uint32_t(nRedisCoin * 1.1))
    {
        LogErrAlt("[%s][%s:%s:%d] coin error, nCoinFee: %d, order nRedisCoin: %u", m_sdkTag.c_str(), __FILE__, __func__, __LINE__,  nCoinFee, nRedisCoin);
        return false;
    }
    
    m_redis.Del(strRedisKey);
    fee = JsRedis.getUInt64("amount");
    realFee = fee;

    CMyString strCoinType = JsRedis.getString("coin_type");
    int64_t nTaxFee = jrParam.getString("fee").asInt64();//交易矿工费，单位：聪 satoshi(bsv)或者 gwei （eth)
    tax = this->GetCoinTransRMB(nTaxFee, strCoinType); 

    depositOK = true;
    return true;
}

bool CDotWalletPay::validDepositCallbackSign(http::CHttpRequest& req, CSdk* pSdk, CSdkChannel *pChannel, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("signature");
    CMyString strPayLoad = req.getParam("payload");
    CMyString strPubKey = req.getParam("pubkey");
    
    std::vector<uint8_t> vchSig;
    ECDSAWrapper::readhexEx(vchSig, strSign1.c_str());
    std::vector<uint8_t> vchPub;
    ECDSAWrapper::readhexEx(vchPub, strPubKey.c_str());

    int bRet = ECDSAWrapper::verify(vchSig, strPayLoad.c_str(), vchPub);
    // 验证签名是否匹配，忽略大小写
    if (bRet == 0)
    {
        LogErrAlt("[%s]deposit callback sign mismatch, payload[%s], signature[%s], pubkey[%s]",
                m_sdkTag.c_str(), strPayLoad.c_str(), strSign1.c_str(), strPubKey.c_str());
        return false;
    }
    return true;
}

void CDotWalletPay::doDepositCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    ext_headers["Content-Type"] = "text/plain";
    Json::Value resp;
    if (bSucceed)
    {
        resp["code"] = 0;
        resp["msg"] = "";
    }
    else
    {
        resp["code"] = errCode;
        resp["msg"] = "";
    }
    
    CJsonWrapper js(resp);
    safe_strcpy(szResp, cbResp, js.toString().c_str());
    cbResp = strlen(szResp);
}

int32_t CDotWalletPay::doDepositHttpRequest(http::CHttpClient &net, const CMyString &strUrl, http::VEC_STRING &header_list, const CMyString &httpData, CMyString &strResp, CMyString &strRespHeader)
{
    if (!header_list.empty())
    {
        CMyString strContentType = header_list[0];
        uint32_t nPos = strContentType.find(";");
        if (nPos != std::string::npos)
        {
            CMyString strContent1 = strContentType.substr(0, nPos);
            CMyString strContent2 = strContentType.substr(nPos + 1);
            header_list.clear();
            header_list.push_back(strContent1);
            header_list.push_back(strContent2);
        }
    }

    return net.HttpPostWithHeaders(strUrl.c_str(), strResp, strRespHeader, header_list,  httpData.c_str());
}

void CDotWalletPay::doMakeCreditRequest(Json::Value& req, CGame* pGame, CSdk *pSdk, CMyString& url, CMyString &contentType, CMyString& httpData) 
{
    Json::Value data;
    Json::Value to(Json::arrayValue);
    Json::Value product(Json::objectValue);

    CMyString strClientId, strClientSecret;
    Json::Value extraData = pSdk->getPayExtraData();
    if (!extraData.empty())
    {
        strClientId = extraData["appid"].asString();
        strClientSecret = extraData["appkey"].asString();
        LogDebug("[%s:%s:%d] strClientId: %s, strClientSecret:%s", __FILE__, __func__, __LINE__, strClientId.c_str(), strClientSecret.c_str());
    }

    CMyString strCoinType = req["bankCode"].asString();
    CMyString strUserId = req["bankName"].asString();
    int64_t nRMBAmount = req["fee"].asInt64();  //单位分
    int64_t nCoin = GetRMBTransCoin(nRMBAmount, strCoinType);
    CMyString strAccessToken = this->GetAccessToken(strClientId, strClientSecret);

    data["out_order_id"] = req["orderNo"].asString();
    data["coin_type"] = strCoinType;
    data["user_id"] = pSdk->getAccount();  //兑出付款方userid
    data["notify_url"] = pSdk->getWithdrawCallbackUrl();
    
    Json::Value item;
    item["type"] = "address";
    item["content"] = this->GetUserReceiveAddress(strAccessToken, strCoinType, strUserId);
    item["amount"] = nCoin;
    to.append(item);
    data["to"] = to;

    product["id"] = req["orderNo"].asString();
    product["name"] = req["orderNo"].asString();
    data["product"] = product;

    {
        Json::Value redisData;
        redisData["coin_type"] = strCoinType;
        redisData["amount"] = nRMBAmount;
        redisData["coin"] = nCoin;
        redisData["orderNo"] = req["orderNo"].asString();
        CJsonWrapper jwRedis(redisData);
        CMyString strRedisValue = jwRedis.toString();
        CMyString strRedisKey(m_strCreditKeyFormat.c_str(), req["orderNo"].asString().c_str());
        m_redis.Set(strRedisKey, strRedisValue, m_nTimeout, 1);
        LogMsg("[%s:%s:%d] strRedisKey: %s, strRedisValue:%s", __FILE__, __func__, __LINE__, strRedisKey.c_str(), strRedisValue.c_str());
    }

    CJsonWrapper jw(data);
    httpData = jw.toString();
    url = pSdk->getWithdrawUrl();
    contentType = "Content-Type: application/json;Authorization: Bearer ";
    contentType += this->GetAccessToken(strClientId, strClientSecret);
}

int32_t CDotWalletPay::doCreditHttpRequest(http::CHttpClient &net, const CMyString &strUrl, http::VEC_STRING &header_list, const CMyString &httpData, CMyString &strResp, CMyString &strRespHeader)
{
    if (!header_list.empty())
    {
        CMyString strContentType = header_list[0];
        uint32_t nPos = strContentType.find(";");
        if (nPos != std::string::npos)
        {
            CMyString strContent1 = strContentType.substr(0, nPos);
            CMyString strContent2 = strContentType.substr(nPos + 1);
            header_list.clear();
            header_list.push_back(strContent1);
            header_list.push_back(strContent2);
        }
    }

    return net.HttpPostWithHeaders(strUrl.c_str(), strResp, strRespHeader, header_list,  httpData.c_str());
}

bool CDotWalletPay::doParseCreditResp(CGame* pGame, const CMyString &respHeader, const CMyString &respData, CMyString &creditOrderNo, CMyString &errNote)
{
    LogDebug("[%s]credit resp, header[%s], data[%s]", m_sdkTag.c_str(), respHeader.c_str(), respData.c_str());
    
    CJsonWrapper jrRoot;
    if (!jrRoot.Init(respData))
    {
        LogErr("[%s:%s:%d] resp data [%s] not json string", __FILE__, __func__, __LINE__, respData.c_str());
        errNote.Format("resp data [%s] not json string", respData.c_str());
        return false;
    }

    int32_t nCode = jrRoot.getInt("code");
    if (nCode != 0)
    {
        LogErr("[%s:%s:%d] resp code is %d", __FILE__, __func__, __LINE__, nCode);
        CMyString strMsg = jrRoot.getString("msg");
        errNote.Format("resp code is %d, msg:%s", nCode, strMsg.c_str());
        return false;
    }

    Json::Value data;
    if (!jrRoot.getJsonObject("data", data))
    {
        LogErr("[%s:%s:%d] data is not object", __FILE__, __func__, __LINE__);
        errNote.Format("data is not object");
        return false;
    }

    creditOrderNo = data["order_id"].asString();

    return true;
}


bool CDotWalletPay::doParseCreditCallback(http::CHttpRequest &req, CMyString &orderNo, CMyString &creditOrderNo, int64_t &fee, int64_t &realFee, int64_t &tax, bool &creditOK)
{
    CByteStream buff;
    req.getPost(buff);
    CMyString strPost;
    buff.Read(strPost);
    CMyString headers = req.getFullUrl();
    CMyString params;
    req.getFullParamStr(params);
    LogDebug("[%s]credit callback, header[%s], param[%s], post[%s]",
            m_sdkTag.c_str(), headers.c_str(), params.c_str(), strPost.c_str());


    CMyString str1 = req.getParam("payload");
    CJsonWrapper jrParam;
    if (!jrParam.Init(str1))
    {
        LogErr("[%s]CDotWalletPay::doParseCreditCallback parse json err. payload: %s", m_sdkTag.c_str(), str1.c_str());
        return false;
    }

    orderNo = jrParam.getString("out_order_id");
    creditOrderNo = jrParam.getString("order_id");
    int64_t nCoinFee = (int64_t)(jrParam.getString("amount").asInt64()); //交易金额，单位：聪 satoshi(bsv)或者 gwei （eth)

    CMyString strRedisValue;
    CMyString strRedisKey(m_strCreditKeyFormat.c_str(), orderNo.c_str());
    if (m_redis.Get(strRedisKey, strRedisValue) != 1)
    {
        LogErr("[%s]CDotWalletPay::doParseCreditCallback order err, redis not find, strRedisKey:%s", m_sdkTag.c_str(), strRedisKey.c_str());
        return false;
    }
    LogMsg("[%s:%s:%d] strRedisKey: %s, strRedisValue:%s", __FILE__, __func__, __LINE__, strRedisKey.c_str(), strRedisValue.c_str());
    CJsonWrapper JsRedis;
    if (!JsRedis.Init(strRedisValue))
    {
        LogErr("[%s]CDotWalletPay::doParseCreditCallback redisvalue parse json err. strRedisValue: %s", m_sdkTag.c_str(), strRedisValue.c_str());
        return false;
    }

    int64_t nRedisCoin = JsRedis.getUInt64("coin");
    LogDebug("[%s] [%s:%d] coin range: %u - %u", m_sdkTag.c_str(), __FILE__, __LINE__, uint32_t(nRedisCoin * 0.9), uint32_t(nRedisCoin * 1.1));
    if (nCoinFee < uint32_t(nRedisCoin * 0.9) || nCoinFee > uint32_t(nRedisCoin * 1.1))
    {
        LogErrAlt("[%s][%s:%s:%d] coin error, nCoinFee: %d, order nRedisCoin: %u", m_sdkTag.c_str(), __FILE__, __func__, __LINE__,  nCoinFee, nRedisCoin);
        return false;
    }
    
    m_redis.Del(strRedisKey);
    fee = JsRedis.getUInt64("amount");
    realFee = fee;

    CMyString strCoinType = JsRedis.getString("coin_type");
    int64_t nTaxFee = jrParam.getString("fee").asInt64();//交易矿工费，单位：聪 satoshi(bsv)或者 gwei （eth)
    tax = this->GetCoinTransRMB(nTaxFee, strCoinType); 

    creditOK = true;

    LogDebug("[%s]credit callback: orderNo:%s, creditOrderNo:%s, fee:%ld, realFee:%ld, tax:%ld, creditOK:%d",
            m_sdkTag.c_str(), orderNo.c_str(), creditOrderNo.c_str(), fee, realFee, tax, creditOK);
    return true;
}


bool CDotWalletPay::validCreditCallbackSign(http::CHttpRequest& req, CGame *pGame, CSdk* pSdk, CMyString& errNote)
{
    CMyString strSign1 = req.getParam("signature");
    CMyString strPayLoad = req.getParam("payload");
    CMyString strPubKey = req.getParam("pubkey");

    std::vector<uint8_t> vchSig;
    ECDSAWrapper::readhexEx(vchSig, strSign1.c_str());
    std::vector<uint8_t> vchPub;
    ECDSAWrapper::readhexEx(vchPub, strPubKey.c_str());

    int bRet = ECDSAWrapper::verify(vchSig, strPayLoad.c_str(), vchPub);
    // 验证签名是否匹配，忽略大小写
    if (bRet == 0)
    {
        LogErrAlt("[%s]credit callback sign mismatch, payload[%s], signature[%s], pubkey[%s]",
                m_sdkTag.c_str(), strPayLoad.c_str(), strSign1.c_str(), strPubKey.c_str());
        return false;
    }
    return true;
}


void CDotWalletPay::doCreditCallbackResp(bool bSucceed, int32_t errCode, std::map<CMyString, CMyString> &ext_headers, char *szResp, uint32_t& cbResp)
{
    ext_headers["Content-Type"] = "text/plain";
    Json::Value resp;
    if (bSucceed)
    {
        resp["code"] = 0;
        resp["msg"] = "";
    }
    else
    {
        resp["code"] = errCode;
        resp["msg"] = "";
    }
    
    CJsonWrapper js(resp);
    safe_strcpy(szResp, cbResp, js.toString().c_str());
    cbResp = strlen(szResp);
}


CMyString CDotWalletPay::GetAccessToken(CMyString& strClientId, CMyString& strClientSecret)
{
    CMyString strAccessToken;
    if (m_strAuthUrl.empty())
    {
        LogErr("[%s][%s:%s:%d]GetAccessToken m_strAuthUrl is empty", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
        return strAccessToken;
    }
    strAccessToken = m_strAccessToken;
    CMyDateTime tNow;
    if (tNow.time() < m_nExpiresIn && !strAccessToken.empty())
    {
        LogMsg("[%s][%s:%s:%d]GetAccessToken accesstoken not expires. accesstoken:%s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strAccessToken.c_str());
        return strAccessToken;
    }

    if (strClientId.empty())
    {
        strClientId = m_strClientId;
    }
    if (strClientSecret.empty())
    {
        strClientSecret = m_strClientSecret;
    }

    Json::Value root;
    root["client_id"] = strClientId;
    root["client_secret"] = strClientSecret;
    root["grant_type"] = "client_credentials";

    CJsonWrapper jw(root);
    CMyString httpData = jw.toString();

    http::VEC_STRING header_list;
    CMyString strResp;
    CMyString strRespHeader;
    CMyString strContent = "Content-Type: application/json";
    header_list.push_back(strContent);

    http::CHttpClient net;
    int32_t nRet = net.HttpPostWithHeaders(m_strAuthUrl.c_str(), strResp, strRespHeader, header_list, httpData.c_str());
    if (nRet == 0)
    {
        LogMsg("[%s][%s:%s:%d]url:%s, httpData:%s, strResp:%s, strRespHeader:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strAuthUrl.c_str(), httpData.c_str(),  strResp.c_str(), strRespHeader.c_str());

        do
        {
            CJsonWrapper jrRoot;
            if (!jrRoot.Load(strResp))
            {
                LogErr("[%s][%s:%s:%d]resp data not json string", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }

            int32_t nCode = jrRoot.getInt("code");
            if (nCode != 0)
            {
                CMyString strMsg = jrRoot.getString("msg");
                LogErr("[%s][%s:%s:%d]code = %d, msg=%s", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, nCode, strMsg.c_str());
                break;
            }

            Json::Value data;
            if (!jrRoot.getJsonObject("data", data))
            {
                LogErr("[%s][%s:%s:%d]data is not object", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }

            m_strAccessToken = data["access_token"].asString();
            int64_t nExpires = data["expires_in"].asInt64();
            m_nExpiresIn = tNow.time() + nExpires - 10;
            strAccessToken = m_strAccessToken;

        } while (false);
    }
    else
    {
        CMyString strErrNote;
        net.getError(nRet, strErrNote);  
        LogErrAlt("[%s][%s:%s:%d]url:%s, httpData:%s, strResp:%s, strRespHeader:%s, err:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strAuthUrl.c_str(), httpData.c_str(), strResp.c_str(), strRespHeader.c_str(), nRet, strErrNote.c_str());
    }

    return strAccessToken;
}

CMyString CDotWalletPay::GetUserReceiveAddress(const CMyString& strAccessToken, const CMyString& strCoinType, const CMyString& strUserId)
{
    CMyString strUserAccess;
    if (m_strUserAddressUrl.empty())
    {
        LogErr("[%s][%s:%s:%d]GetUserReceiveAddress m_strUserAddressUrl is empty", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
        return strUserAccess;
    }

    Json::Value root;
    root["user_id"] = strUserId;
    root["coin_type"] = strCoinType;

    CJsonWrapper jw(root);
    CMyString httpData = jw.toString();

    http::VEC_STRING header_list;
    CMyString strResp;
    CMyString strRespHeader;
    CMyString contentType = "Content-Type: application/json";
    CMyString strAuthorization = "Authorization: Bearer ";
    strAuthorization += strAccessToken;
    header_list.push_back(contentType);
    header_list.push_back(strAuthorization);

    http::CHttpClient net;
    int32_t nRet = net.HttpPostWithHeaders(m_strUserAddressUrl.c_str(), strResp, strRespHeader, header_list, httpData.c_str());
    if (nRet == 0)
    {
        LogMsg("[%s][%s:%s:%d] url:%s, httpData:%s,  strResp:%s, strRespHeader:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strUserAddressUrl.c_str(), httpData.c_str(), strResp.c_str(), strRespHeader.c_str());

        do
        {
            CJsonWrapper jrRoot;
            if (!jrRoot.Load(strResp))
            {
                LogErr("[%s][%s:%s:%d]resp data not json string", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }
            int32_t nCode = jrRoot.getInt("code");
            if (nCode != 0)
            {
                LogErr("[%s][%s:%s:%d]resp code = %d", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, nCode);
                break;
            }

            Json::Value data;
            if(!jrRoot.getJsonObject("data", data))
            {
                LogErr("[%s][%s:%s:%d]not find data object", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }
            strUserAccess = data["primary_wallet"]["address"].asString();

        } while (false);
    }
    else
    {
        CMyString strErrNote;
        net.getError(nRet, strErrNote);  
        LogErrAlt("[%s][%s:%s:%d]url:%s, httpData:%s,  strResp:%s, strRespHeader:%s err:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strUserAddressUrl.c_str(), httpData.c_str(), strResp.c_str(), strRespHeader.c_str(), nRet, strErrNote.c_str());
    }

    return strUserAccess;
}

void CDotWalletPay::GetRate()
{
    if (m_strTransUrl.empty())
    {
        LogErr("[%s][%s:%s:%d]GetRate m_strUserAddressUrl is empty", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
        return ;
    }

    CMyString strResp;
    CMyString strRespHeader;
    http::VEC_STRING header_list;
    CMyString strContentType = "Authorization: Bearer ";
    strContentType += GetAccessToken(m_strClientId, m_strClientSecret);
    header_list.push_back(strContentType);

    http::CHttpClient net;
    int32_t nRet = net.HttpGetWithHeaders(m_strTransUrl.c_str(), strResp, strRespHeader, header_list);
    if (nRet == 0)
    {
        LogMsg("[%s][%s:%s:%d] strResp:%s, strRespHeader:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, strResp.c_str(), strRespHeader.c_str());

        do
        {
            CJsonWrapper jrRoot;
            if (!jrRoot.Load(strResp))
            {
                LogErr("[%s][%s:%s:%d]resp data not json string", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }

            int32_t nCode = jrRoot.getInt("code");
            if (nCode != 0)
            {
                LogErr("[%s][%s:%s:%d]code != 0, code :%d", m_sdkTag.c_str(), __FILE__, __func__, __LINE__, nCode);
                break;
            }

            Json::Value data;
            if (!jrRoot.getJsonObject("data", data))
            {
                LogErr("[%s][%s:%s:%d]data not object", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
                break;
            }

            m_jsRate = jrRoot.getJson();

        } while (false);
    }
    else
    {
        CMyString strErrNote;
        net.getError(nRet, strErrNote);  
        LogErrAlt("[%s][%s:%s:%d] url:%s, err:%d, msg:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_strTransUrl.c_str(), nRet, strErrNote.c_str());
    }
}

int64_t CDotWalletPay::GetRMBTransCoin(int64_t nRMB, const CMyString& strCoinType)
{
    int64_t nCoin = 0;
    if (!m_jsRate.empty())
    {
        LogMsg("[%s][%s:%s:%d] m_jsRate:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_jsRate.toStyledString().c_str());

        Json::Value data = m_jsRate["data"];
        int64_t nCNY = data["CNY"].asInt64();
        if (strCoinType == "BTC")
        {
            int64_t nBTC = data["BTC"].asInt64();
            //nCoin = ceil((nRMB / (nBTC * 100.0f)) * nCNY) * 100000000;

            LogWarn("[%s:%s:%d] nBTC=%ld, nCNY=%ld", __FILE__, __func__, __LINE__, nBTC, nCNY);
            double temp1 = nRMB / (nBTC * 100.0f);
            double temp2 = temp1 * nCNY;
            int64_t temp3 = ceil(temp2 * 100000000.0f);
            LogWarn("[%s:%s:%d] temp1=%lf, temp2=%lf, temp3=%ld", __FILE__, __func__, __LINE__, temp1, temp2, temp3);
            nCoin = temp3;
        }
        else if (strCoinType == "BSV")
        {
            int64_t nBSV = data["BSV"].asInt64();
            //nCoin = ceil(((nCNY * 100) / (nBSV * 1.0f)) * 100000000.0f) * nRMB;

            LogWarn("[%s:%s:%d] nBSV=%ld, nCNY=%ld", __FILE__, __func__, __LINE__, nBSV, nCNY);
            double temp1 = nRMB / (nBSV * 100.0f);
            double temp2 = temp1 * nCNY;
            int64_t temp3 = ceil(temp2 * 100000000.0f);
            LogWarn("[%s:%s:%d] temp1=%lf, temp2=%lf, temp3=%ld", __FILE__, __func__, __LINE__, temp1, temp2, temp3);
            nCoin = temp3;
        }
        else if (strCoinType == "ETH")
        {
            int64_t nETH = data["ETH"].asInt64();
            //nCoin = ceil(((nCNY * 100) / (nETH * 1.0f)) * 1000000000.0f) * nRMB;

            LogWarn("[%s:%s:%d] nBSV=%ld, nCNY=%ld", __FILE__, __func__, __LINE__, nETH, nCNY);
            double temp1 = nRMB / (nETH * 100.0f);
            double temp2 = temp1 * nCNY;
            int64_t temp3 = ceil(temp2 * 1000000000.0f);
            LogWarn("[%s:%s:%d] temp1=%lf, temp2=%lf, temp3=%ld", __FILE__, __func__, __LINE__, temp1, temp2, temp3);
            nCoin = temp3;
        }
    }
    else
    {
        LogErrAlt("[%s][%s:%s:%d] m_jsRate is empty", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
    }

    return nCoin;
}

int64_t CDotWalletPay::GetCoinTransRMB(int64_t nCoin, const CMyString& strCoinType)
{
    int64_t nRMB = 0;
    if (!m_jsRate.empty())
    {
        LogMsg("[%s][%s:%s:%d] m_jsRate:%s",
            m_sdkTag.c_str(), __FILE__, __func__, __LINE__, m_jsRate.toStyledString().c_str());

        Json::Value data = m_jsRate["data"];
        int64_t nCNY = data["CNY"].asInt64();
        if (strCoinType == "BTC")
        {
            int64_t nBTC = data["BTC"].asInt64();

            LogWarn("[%s:%s:%d] BTC=%ld, nCNY=%ld", __FILE__, __func__, __LINE__, nBTC, nCNY);
            double temp1 = nBTC / nCNY;
            double temp2 = temp1 / 1000000.0f;
            int64_t temp3 = floor(temp2 * nCoin);
            LogWarn("[%s:%s:%d] temp1=%lf, temp2=%lf, temp3=%ld", __FILE__, __func__, __LINE__, temp1, temp2, temp3);
            nRMB = temp3;
        }
        else if (strCoinType == "BSV")
        {
            int64_t nBSV = data["BSV"].asInt64();

            LogWarn("[%s:%s:%d] nBSV=%ld, nCNY=%ld", __FILE__, __func__, __LINE__, nBSV, nCNY);
            double temp1 = nBSV / nCNY;
            double temp2 = temp1 / 1000000.0f;
            int64_t temp3 = floor(temp2 * nCoin);
            LogWarn("[%s:%s:%d] temp1=%lf, temp2=%lf, temp3=%ld", __FILE__, __func__, __LINE__, temp1, temp2, temp3);
            nRMB = temp3;
        }
        else if (strCoinType == "ETH")
        {
            int64_t nETH = data["ETH"].asInt64();

            LogWarn("[%s:%s:%d] nETH=%ld, nCNY=%ld", __FILE__, __func__, __LINE__, nETH, nCNY);
            double temp1 = nETH / nCNY;
            double temp2 = temp1 / 10000000.0f;
            int64_t temp3 = floor(temp2 * nCoin);
            LogWarn("[%s:%s:%d] temp1=%lf, temp2=%lf, temp3=%ld", __FILE__, __func__, __LINE__, temp1, temp2, temp3);
            nRMB = temp3;
        }
    }
    else
    {
        LogErrAlt("[%s][%s:%s:%d] m_jsRate is empty", m_sdkTag.c_str(), __FILE__, __func__, __LINE__);
    }

    return nRMB;
}