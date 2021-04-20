#pragma once

namespace UCSDK
{
    /*
    Wiki��
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPOST.5D.2Fusers.2Factions.2Fcheck_.E5.B8.90.E5.8F.B7.E6.A0.A1.E9.AA.8C
    �������ʺ�У��
    ����˵�����ʺ�У���Ƿ����
    ����˵���������û���org_name�������ô������ߴ��գ���org_name:""
    */
    class ICheckUserExsits
    {
    public:
        virtual bool Excute(const char* pszLoginName, const char* pszOrgName, const char* pszDeviceId) = 0;
        virtual const char* GetUserId() = 0;//�û����
        virtual const char* GetEmail() = 0;//email
        virtual const char* GetMobile() = 0;//�ֻ���
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
    UC/SESSION_TYPE_INVALID �Ự���Ͳ���ȷ
    */
    typedef class ICheckUserExsits	CheckUserExsits;
}