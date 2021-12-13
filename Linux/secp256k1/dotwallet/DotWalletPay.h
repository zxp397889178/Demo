/*
 * =====================================================================================
 *
 *       Filename:  DotWalletPay.h
 *
 *    Description:  数字钱包支付
 *
 *        Version:  1.0
 *        Created:  2021年11月30日 19时16分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  <Your Full Name> (<Your Nick Name>), <your e-mail>
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */



#pragma once
#include "../PayBase.h"
#include "IWebPlugin.h"

class CDotWalletPay : public CPayBase
{
public:
    CDotWalletPay();
    virtual ~CDotWalletPay();

    int32_t Init(void);
    int32_t Update(void);
    virtual void Maintance(void);

public:
    ENTRY_FUNC_DEFINE(doDeposit);
    ENTRY_FUNC_DEFINE(doDepositCB);
    ENTRY_FUNC_DEFINE(doWithdraw);
    ENTRY_FUNC_DEFINE(doWithdrawCB);
    ENTRY_FUNC_DEFINE(doGetRate);

protected:
    PAYBASE_DEPOSIT_FUNCTIONS_DECLARE
    PAYBASE_CREDIT_FUNCTIONS_DECLARE
    int32_t doDepositHttpRequest(http::CHttpClient &net, const CMyString &strUrl, http::VEC_STRING &header_list, const CMyString &httpData, CMyString &strResp, CMyString &strRespHeader);
    int32_t doCreditHttpRequest(http::CHttpClient &net, const CMyString &strUrl, http::VEC_STRING &header_list, const CMyString &httpData, CMyString &strResp, CMyString &strRespHeader);

private:
    void    GetRate();
    int64_t GetRMBTransCoin(int64_t nRMB, const CMyString& strCoinType);  //人民币单位为分
    int64_t GetCoinTransRMB(int64_t nCoin, const CMyString& strCoinType); //人民币单位为分

    CMyString GetAccessToken(CMyString& strClientId, CMyString& strClientSecret);
    CMyString GetUserReceiveAddress(const CMyString& strAccessToken, const CMyString& strCoinType, const CMyString& strUserId);

private:
    map<CMyString, CMyString> m_mapAddress;
    CMyString m_strAuthUrl;
    CMyString m_strClientId;
    CMyString m_strClientSecret;
    CMyString m_strUserAddressUrl;
    CMyString m_strPayUrl;
    CMyString m_strTransUrl;
    CMyString m_strAccessToken;
    int64_t   m_nExpiresIn;
    
    CMyString m_strDepositKeyFormat;
    CMyString m_strCreditKeyFormat;
    int32_t   m_nTimeout;

    CMyTimer  m_timerRefershRate;  // 定时检查发货缓存
    Json::Value m_jsRate;
};
