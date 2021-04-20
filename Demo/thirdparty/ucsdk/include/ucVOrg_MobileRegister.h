#pragma once

namespace UCSDK
{
    /*
    Wiki��
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Fusers.5B.3Fauto_login.3Dfalse.5D_.E4.BD.BF.E7.94.A8.E4.B8.8B.E5.8F.91.E7.9A.84.E7.9F.AD.E4.BF.A1.E9.AA.8C.E8.AF.81.E7.A0.81.E8.BF.9B.E8.A1.8C.E5.B8.90.E5.8F.B7.E6.B3.A8.E5.86.8C
    �������ֻ�ע��
    ����˵����
    ����auto_login(��ѡ)����auto_login=trueʱ���ḽ�������Զ���¼��token��Ϣ
    �ǳ��������϶����ֹ��ܣ���nick_nameΪ�������������ǡ�������ȡ����ƴ����ge������nick_name_fullΪ��genie����nick_name_shortΪ��gn��
    */
    class IVOrgMobileRegister
    {
    public:
		virtual bool Excute(const char* pszMobile, const char* pszPassword, const char* pszMobileCode, const char* pszCountryCode, const char* pszOrgId, const char* pszOrgName, const char* pszSessionId, const char* pszSessionKey) = 0;
        virtual const char* GetUserId() = 0;//�û����
        virtual const char* GetUserName() = 0;//Token
        virtual const char* GetNickName() = 0;//Token
        virtual const char* GetCreateTime() = 0;//�û�����ʱ��
        /*----------�����������������auto_login = trueʱ����--------------------*/
        virtual const char* GetAccessToken() = 0;//Token
        virtual const char* GetExpiresAt() = 0;//Token����ʱ��
        virtual const char* GetRefreshToken() = 0;//ˢ����Token
        virtual const char* GetMacKey() = 0;//Mac keyֵ
        virtual const char* GetMacAlgorithm() = 0;//Mac�㷨
        virtual time_t GetServerTime() = 0;//������ʱ��
        virtual const char* GetReturnCode() = 0;
        virtual const wchar_t* GetReturnMessage() = 0;
    public:
        virtual DWORD GetHttpCode() = 0;
        virtual const char* GetResult() = 0;
        virtual const char* GetHttpHeader() = 0;
        virtual void Release() = 0;
    };

    /*
    ����Code�б�:
    UCSDK/REQUIRE_FAILED ��������ʧ��
    UCSDK/REQUIRE_ARGUMENT ȱ�ٲ���
    UCSDK/PARSE_FAILED ���ݽ���ʧ��
    UC/REQUIRE_ARGUMENT ȱ�ٲ���
    UC/INVALID_ARGUMENT ��Ч����(��ʽ����,���ȹ�������̵�)
    UC/PHONE_HAS_REGISTER �ֻ������Ѿ�ע���
    UC/SMS_EXPIRED ����δ�·����Ѿ�����
    UC/SMS_INVALID ������֤�벻��ȷ
    UC/USERNAME_INVALID �û�����ʽ����ȷ,������ĸ��ͷ,�ɰ�����ĸ�����֡�_���20�ַ�
    UC/USERNAME_EXIST �û����Ѵ���
    UC/INVALID_NICK_NAME �ǳƲ���Ϊ���ҳ��Ȳ��ܳ���50�ֽ�
    UC/INVALID_ENABLE_STATUS ����enable_statusȡֵ����ȷ,ֻ����0��1
    UC/INVALID_COUNTRY_CODE�������Ÿ�ʽ����ȷ,ֻ����+��ͷ����+�����֣��16�ַ�
    UC/SMSCODE_ERROR_OVER_SUM ������֤��һ�����������������ܳ�������(5��)�������»�ȡ������֤�룡
    UC/NICK_NAME_CONTAIN_SENSITIVE_WORD �ǳƲ��Ϸ����������д�
    UC/ORG_NOT_EXIST","ָ������֯������
    UC/SESSION_TYPE_INVALID","�Ự���Ͳ���ȷ
    */

	typedef class IVOrgMobileRegister	VOrgMobileRegister;
}

