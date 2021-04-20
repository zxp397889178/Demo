#pragma once

namespace UCSDK
{
    /*
    Wiki��
    http://wiki.sdp.nd/index.php?title=UC_API_RestfulV0.93#.5BPATCH.5D.2Fusers.2F.7Buser_id.7D_.E8.AE.BE.E7.BD.AE.E7.94.A8.E6.88.B7.E4.B8.AA.E4.BA.BA.E5.B8.90.E5.8F.B7.E4.BF.A1.E6.81.AF
    �����������û������ʺ���Ϣ
    ����˵�������û�id�����û������ʺ���Ϣ
    ����˵����
    ��ʵ�������ǳ��������϶����ֹ��ܣ���nick_nameΪ�������������ǡ�������ȡ����ƴ����ge������nick_name_fullΪ��genie����nick_name_shortΪ��gn��
    ��չ��Ϣorg_exinfo��realm_exinfo����Ҫ�洢�û���������Ϣ(�磺���֤���ֻ������)�����Ҫ������ܺ����
    */
    class IPatchUserProfile
    {
    public:
        virtual bool Excute(const char* pszRealName, const char* pszMobile, const char* pszEmail, const char* pszNickName, const char* pszBirthday, const char* pszGender) = 0;
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
    UC/ACCOUNT_NOT_EXIST �ʺŲ�����
    UC/ORG_USER_CODE_HAS_EXIST �����Ѿ�����
    UC/USERNAME_INVALID �û�����ʽ����ȷ,������ĸ��ͷ,�ɰ�����ĸ�����֡�_���20λ
    UC/USERNAME_EXIST �û����Ѵ���
    UC/ORGUSERCODE_INVALID ���Ż�ѧ�Ÿ�ʽ����ȷ,�ɰ�����ĸ�����֡�_��.��@�������������ֻ����룬�50�ַ�
    UC/INVALID_REAL_NAME ��ʵ��������Ϊ���ҳ��Ȳ��ܳ���50�ֽ�
    UC/INVALID_NICK_NAME �ǳƲ���Ϊ���ҳ��Ȳ��ܳ���50�ֽ�
    UC/INVALID_EXINFO_KEY ��չ����key��ֻ������ĸ�����֡��»��߱�ʾ������ֻ��ʹ����ĸ��ͷ���20�ַ�
    UC/INVALID_USER_REALM_EXINFO_KEY �û�������չkey��ֻ������ĸ�����֡�.���»��߱�ʾ��ֻ��ʹ����ĸ��ͷ�����У�������.���ӣ��100�ַ�
    UC/INVALID_ENABLE_STATUS ����enable_statusȡֵ����ȷ,ֻ����0��1
    UC/NICK_NAME_CONTAIN_SENSITIVE_WORD �ǳƲ��Ϸ����������д�
    */
    typedef class IPatchUserProfile	PatchUserProfile;
}