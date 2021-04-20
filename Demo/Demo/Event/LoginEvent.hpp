#pragma once
#include "UserCenterEventModel.hpp"

struct LoginParam
{
	tstring strUserName;
	tstring strUserPassword;
	tstring strAvatarPath;
	int nKeppPassword;
	int nAutoLogin;
	bool bDoAutoLogin;
};

enum ThirdLoginType
{
	ThirdLogin_Weibo = 0,
	ThirdLogin_WeiXin,
	ThirdLogin_QQ,
	ThirdLogin_GzEic,
	ThirdLogin_FaceBook,
	ThirdLogin_Google,
	ThirdLogin_OtherWay,
};

struct CompleteBtnPullDownChange : public CTargetEventPublisher<CompleteBtnPullDownChange, HWND>
{

};

//請求第三方登錄ChannelId
struct RequestThirdPartyLoginChannelId : public CEventPublisher<RequestThirdPartyLoginChannelId>
{
	DWORD dwChannelId = 0;
};

//請求设置第三方登錄ChannelId
struct RequestSetThirdPartyLoginChannelId : public CEventPublisher<RequestSetThirdPartyLoginChannelId>
{
	DWORD dwChannelId = 0;
};

//////////////////////////////////////////////////////////////////////////
//Login
struct UserCenterLoginExtra
{
	std::wstring wstrUserName;
	bool bAutoLogin;
	bool bSavePwd;
	HWND hTagetWnd = NULL;
};

struct UserCenterRequestAccLogin : public CEventPublisher<UserCenterRequestAccLogin>
{
	time_t tBeginTime;
	std::wstring wstrUserName;
	std::wstring wstrPassword;
	bool bEncryptPwd;
	UserCenterLoginExtra stLoginExtra;
};

struct UserCenterRequestPrivateLogin : public CEventPublisher<UserCenterRequestPrivateLogin>
{
	std::wstring wstrUserName;
	std::wstring wstrPassword;
	std::string strOrganize;
	bool bEncryptPwd;
	UserCenterLoginExtra stLoginExtra;
};

struct UserCenterRequestThirdPartyLogin : public CEventPublisher<UserCenterRequestThirdPartyLogin>
{
	std::wstring wstrOpenId;
	std::wstring wstrAppId;
	std::wstring wstrSourcePlat;
	std::wstring wstrAccessToken;
	std::wstring wstrLoginPlat;
	UserCenterLoginExtra stLoginExtra;
};

struct UserCenterRequestReleasePrivateLogin : public CEventPublisher<UserCenterRequestReleasePrivateLogin>
{
	std::wstring wstrUserName;
	std::wstring wstrPassword;
	std::string strOrganize;
	bool bEncryptPwd;
	UserCenterLoginExtra stLoginExtra;
};

struct UserCenterRequestTokenLogin : public CEventPublisher<UserCenterRequestTokenLogin>
{
	std::wstring wstrHost;
	std::wstring wstrUri;
	std::wstring wstrMethod;
	std::wstring wstrAccessToken;
	std::wstring wstrMac;
	std::wstring wstrNonce;
	std::wstring wstrLoginPlat;
	UserCenterLoginExtra stLoginExtra;
};

struct UserCenterRequestBearerTokenLogin : public CEventPublisher<UserCenterRequestBearerTokenLogin>
{
	time_t tBeginTime;
	std::wstring wstrSessionId;
	std::wstring wstrLoginPlat;
	UserCenterLoginExtra stLoginExtra;
};

struct UserCenterRequestMobileLogin : public CEventPublisher<UserCenterRequestMobileLogin>
{
	std::wstring wstrUserId;
	std::wstring wstrUserName;
	std::wstring wstrAccessToken;
	std::wstring wstrMacKey;
	std::wstring wstrServerTime;
	std::wstring wstrOrgId;
	std::wstring wstrLoginPlat;
	std::wstring wstrExternalToken;
	std::wstring wstrExternalOriginalToken;
	UserCenterLoginExtra stLoginExtra;
	std::string strSession;    //complete原因返回
};

struct UserCenterRequestCancelLogin : public CEventPublisher<UserCenterRequestCancelLogin>
{
	bool bCancel = true;
};

///////////////////////////////////////////////////////////////////////////

struct UserLoginComplete
{
	UserLoginComplete() : dwErrorCode(0){}
	DWORD dwErrorCode;
	std::wstring wstrErrorCode;
	std::wstring wstrErrorDescription;
	PVOID pUserData;
};

struct UserCenterLogining : public UserLoginComplete, public CEventPublisher<UserCenterLogining>
{

};

struct UserCenterLoginComplete : public UserLoginComplete, public CEventPublisher<UserCenterLoginComplete>
{
	time_t tBeginTime;
	std::shared_ptr<UserInfo> pUserInfo;
	UserCenterLoginExtra stLoginExtra;
	std::string strSession; //Request获取
};

struct UserCenterPrivateLoginComplete : public UserLoginComplete, public CEventPublisher<UserCenterPrivateLoginComplete>
{
	std::shared_ptr<UserInfo> pUserInfo;
	UserCenterLoginExtra stLoginExtra;
};

struct UserCenterReleasePrivateLoginComplete : public UserLoginComplete, public CEventPublisher<UserCenterReleasePrivateLoginComplete>
{
	std::shared_ptr<UserInfo> pUserInfo;
	UserCenterLoginExtra stLoginExtra;
};

//////////////////////////////////////////////////////////////////////////
struct UserCenterRequestLogout : public CEventPublisher<UserCenterRequestLogout>
{

};

struct CompleteUserCenterLogout : public CEventPublisher<CompleteUserCenterLogout>
{

};

//第三方登录信息
//////////////////////////////////////////////////////////////////////////
struct LoginPlatformInfo
{
	LoginPlatformInfo() : dwId(0), ullCreateTime(0), ullUpdateTime(0) {}
	DWORD dwId;
	std::wstring wstrPlatformName;
	std::wstring wstrPlatformLogo;
	std::wstring wstrSourcePlat;
	std::wstring wstrPcLoginUrl;
	std::wstring wstrAppLoginUrl;
	std::wstring wstrLogoIconPath;
	ULONGLONG ullCreateTime;
	ULONGLONG ullUpdateTime;
};


struct PPTEduRequestThirdPartyLoginPlatformInfo : public CEventPublisher<PPTEduRequestThirdPartyLoginPlatformInfo>
{
	std::wstring wstrAreaCode;
	HWND		 hWnd;
};

struct CompleteThirdPartyLoginPlatformInfo : public UserLoginComplete, public CTargetEventPublisher<CompleteThirdPartyLoginPlatformInfo, HWND>
{
	int nCount = 0;
	vector<LoginPlatformInfo> vecPlatformInfo;
};

//点击登录按钮
struct CompleteStartLoginEvent : public CEventPublisher<CompleteStartLoginEvent>
{

};

//RequestVirtualAccLogin
struct RequestUserLoginEvent : public CEventPublisher<RequestUserLoginEvent>
{

};

//第三方登录窗口
struct RequestStartThirdLoginWnd : public CEventPublisher<RequestStartThirdLoginWnd>
{

};

//////////////////////////////////////////////////////////////////////////
struct RequestAndCreateLoginStatusInfo : public CEventPublisher<RequestAndCreateLoginStatusInfo>
{
	std::shared_ptr<LoginStatusInfo> spLoginStatusInfo;
};

//////////////////////////////////////////////////////////////////////////
struct RequestIsLogining : public CEventPublisher<RequestIsLogining>
{
	bool bIsLogining;
};

//////////////////////////////////////////////////////////////////////////
struct RequestIsCancelLogin : public CEventPublisher<RequestIsCancelLogin>
{
	bool bIsCancelLogin;
};

//////////////////////////////////////////////////////////////////////////
struct RequestGetThirdPartyHost : public CEventPublisher<RequestGetThirdPartyHost>
{
	tstring tstrHost;
};

//////////////////////////////////////////////////////////////////////////
struct RequestLoginAfter : public CEventPublisher<RequestLoginAfter>
{
	std::string strSession;
};

//请求登录，成功后返回目标
struct CompleteLoginComeBack : public CTargetEventPublisher <CompleteLoginComeBack, tstring>
{

};

struct UcRequestAccSaveInfo : public CEventPublisher<UcRequestAccSaveInfo>
{
	tstring wstrUserName;
	tstring wstrUserPwd;
	int iKeepPwd;
	int iAutoLogin;
};

struct UcRequestLastLoginInfo : public CEventPublisher<UcRequestLastLoginInfo>
{
	HWND		hWnd;
};

struct CompleteLastLoginInfo : public CTargetEventPublisher<CompleteLastLoginInfo, HWND>
{
	int			nInfoCount;
	LoginParam	stLoginParam;
};

struct UcRequestClearAutoLogin : public CEventPublisher<UcRequestClearAutoLogin>
{
	tstring strUserName;
};

struct UcRequestPullDownInfo : public CEventPublisher<UcRequestPullDownInfo>
{

};

struct CompletePullDownInfo : public CEventPublisher<CompletePullDownInfo>
{
	vector<LoginParam> m_vLoginParam;
};

struct UcRequestSelectUserInfo : public CEventPublisher<UcRequestSelectUserInfo>
{
	tstring wstrUserName;
	HWND	hwnd;
};

struct UcRequestDelUserInfo : public CEventPublisher<UcRequestDelUserInfo>
{
	tstring wstrUserName;
};

struct CompleteDelUserInfo : public CEventPublisher<CompleteDelUserInfo>
{
	tstring wstrUserName;
};

struct CompleteSelectUserInfo : public CTargetEventPublisher<CompleteSelectUserInfo, HWND>
{
	LoginParam m_stUserInfo;
};

struct UcRequestSetAvatar : public CEventPublisher<UcRequestSetAvatar>
{
	tstring	wstrUserName;
	HWND    hTargetWnd  = NULL;
};

struct UcRequestUpdateAvatar : public CEventPublisher<UcRequestUpdateAvatar>
{
	tstring wstrUserName;
	tstring wstrAvatarPath;
};

struct CompleteSetAvatar : public CEventPublisher<CompleteSetAvatar>
{
	tstring wstrAvatarPath;
};

struct CompleteSetTargetAvatar : public CTargetEventPublisher<CompleteSetTargetAvatar, HWND>
{
	tstring wstrAvatarPath;
};

struct UcRequestUpdateDBUserInfo : public CEventPublisher<UcRequestUpdateDBUserInfo>
{

};

struct UcRequestGetAllLoginInfo : public CEventPublisher<UcRequestGetAllLoginInfo>
{
	bool bSuccess = false;
	vector<LoginParam> vLoginParam;
};

struct RequestThirdPlatformLogin : public CEventPublisher<RequestThirdPlatformLogin>
{
	// enum ThirdLoginType
	int		nThirdLoginType;
	HWND	hParent;
	HWND    hTargetWnd = NULL;
	bool    bScreenLogin;
};

class IThirdPlatformLogin;
struct RequestScreenThirdPlatformLogin : public CTargetEventPublisher<RequestScreenThirdPlatformLogin, HWND>
{
	IThirdPlatformLogin* pLogin = NULL;
};

struct RequestInternationLogin : public CEventPublisher < RequestInternationLogin >
{
	// enum ThirdLoginType
	HWND	hParent;
	HWND    hTargetWnd = NULL;
	tstring strLoginUrl;
	tstring strTitle;
	tstring	strResoursePlat;
};

struct RequestScreenInternationLogin : public CTargetEventPublisher<RequestScreenInternationLogin, HWND>
{
	IThirdPlatformLogin* pLogin = NULL;
};

struct RequestCultivateLevelImgseqView : public CEventPublisher<RequestCultivateLevelImgseqView>
{
	int iLevelImgseq;
};

struct RequestCurrLevelView : public CEventPublisher<RequestCurrLevelView>
{
	int iLevel;
	ULONGLONG ullGiveNetDisk;
};

//登录面板点击注册
struct RequestShowRegisterPanel : public CEventPublisher<RequestShowRegisterPanel>
{
	
};

//登录面板点击忘记密码
struct RequestShowFindPasswordPanel : public CEventPublisher<RequestShowFindPasswordPanel>
{

};

//第三方登录入口点击
struct RequestClickThirdLoginEntry : public CEventPublisher<RequestClickThirdLoginEntry>
{
	tstring strPlatform;
};

struct RequestPearsonToken : public CEventPublisher<RequestPearsonToken>
{
	tstring wstrLoginPlat;
	tstring wstrPearsonToken;
	tstring wstrLoginName;
};

/////////////////////////////////////////////////////////////
//第三方登录自动登录相关消息

struct ThirdPartyLoginParam
{
	std::wstring wstrUserId;
	std::wstring wstrUserName;
	std::wstring wstrAccessToken;
	std::wstring wstrMacKey;
	std::wstring wstrServerTime;
	std::wstring wstrOrgId;
	std::wstring wstrLoginPlat;
	std::wstring wstrExternalToken;
	std::wstring wstrExternalOriginalToken;
	bool bDoAutoLogin = true;
};

struct UcRequestThirdPartyLoginSaveInfo : public CEventPublisher<UcRequestThirdPartyLoginSaveInfo>
{
	ThirdPartyLoginParam stLoginParam;
}; 

struct UcRequestGetThirdPartyLoginInfo : public CEventPublisher<UcRequestGetThirdPartyLoginInfo>
{
	bool bSuccess = false;
	vector<ThirdPartyLoginParam> vLoginParam;
};