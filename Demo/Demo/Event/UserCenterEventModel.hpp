#pragma once

#define CLASSTYPE_ADMINCLASS _T("class")
#define CLASSTYPE_OTHERCLASS _T("teaching_class")

#define LessonType_FreeMode		_T("1")
#define LessonType_ClassMode	_T("0")

#define GROUPTYPE_ADMINGROUP		_T("admin_group")
#define GROUPTYPE_SELFBUILTGROUP	_T("selfbuilt_group")

struct LibraryInfo
{
	std::wstring wstrType;
	std::wstring wstrRole;
	std::wstring wstrSchoolId;
	std::wstring wstrSchoolName;
};

struct SubjectInfo
{
	std::wstring wstrSubjectId;
	std::wstring wstrSubjectName;
};

struct StudentInfo
{
	std::wstring wstrUserId;
	std::wstring wstrUserNo;
	std::wstring wstrUserName;
	std::wstring wstrNickName;
	std::wstring wstrGroupId;
	std::wstring wstrGender;//1：男，2：女，0：未知
	std::wstring wstrHeadIconMd5;
	std::wstring wstrHeadIconUrl;
	std::wstring wstrHeadIconPath;
	std::wstring wstrLoginName;
	std::vector<std::pair<bool, std::vector<std::string>>> vecNamePinYin;
};

struct SchoolInfo
{
	std::wstring wstSchoolId;
	std::wstring wstrClassId;
	std::wstring wstrGradeId;
	std::wstring wstrSchoolName;
	std::wstring wstrGradeName;
	std::wstring wstrClassName;
	std::wstring wstrClassType;
	std::wstring wstrClassCode;
	std::wstring wstrOwnerId;
	std::wstring wstrStage;
};

struct GroupInfo
{
	GroupInfo() : nOrderId(0), nTurnType(0), nType(0), 
		ullGroupId(0), ullTeacherId(0) {}

	int nOrderId;
	int nTurnType;
	int nType;
	ULONGLONG ullGroupId;
	ULONGLONG ullTeacherId;
	std::wstring wstrClassId;
	std::wstring wstrGroupName;
	std::wstring wstrGroupLeader;
	std::wstring wstrGroupType;
	vector<std::wstring> vecStudents;
};

struct NetDiskInfoModel
{
	ULONGLONG ullNetDiskSize;//总大小
	ULONGLONG ullUsedSize;//已使用
	std::wstring wstrRootUUID;//根目录UUID
	std::wstring wstrCreateDate;
    std::wstring wstrUpdateDate;
    std::wstring wstrCurrentNetDiskFolder;
};

struct LoginStatusInfo
{
	void SetTrue()
	{
		bIsGetDBankInfo = true;
		bIsGetLibraryInfo = true;
		bIsGetZJCatagoryInfo = true;
		bIsGetSchoolInfo = true;
		bIsGetRolesInfo = true;
		bIsGetJYTokenInfo = true;
		bIsGetChapterInfo = true;
		bIsGetSubjectInfo = true;
	}
	void* pSessionId = NULL;
	bool bIsGetDBankInfo = false;
	bool bIsGetLibraryInfo = false;
	bool bIsGetZJCatagoryInfo= false;
	bool bIsGetSchoolInfo = false;
	bool bIsGetRolesInfo = false;
	bool bIsGetJYTokenInfo = false;
	bool bIsGetChapterInfo = false;
	bool bIsGetSubjectInfo = false;
};

struct JYEOOSubjectInfo
{
	std::wstring wstrSubjectId;
	std::wstring wstrSubjectName;
};

typedef vector<std::shared_ptr<SchoolInfo>>		SchoolVector;
typedef vector<std::shared_ptr<SubjectInfo>>	SubjectVector;
typedef vector<std::shared_ptr<LibraryInfo>>	LibraryVector;
typedef vector<JYEOOSubjectInfo> JyeooSubjectVector;
struct UserInfo
{
    ~UserInfo(){ if (pUser) pUser->Release(); }
	std::wstring wstrUserId;
	std::wstring wstrUserName;
	std::wstring wstrEncryptPwd;
	std::wstring wstrMacKey;
	std::wstring wstrAccessToken;
	std::wstring wstrServerTime;
	std::wstring wstrRealName;
	std::wstring wstrGender;//1：男，2：女，0：未知
	std::wstring wstrBirthday;
	std::wstring wstrMobile;
	std::wstring wstrEmail;
	std::wstring wstrOrgId;
	std::wstring wstrNickName;
	std::wstring wstrAvatarPath;
	std::wstring wstrLoginPlat;
	std::wstring wstrZJCategoryCode;
	std::wstring wstrUserRole;
	std::wstring wstrClassId;
	std::wstring wstrClassName;
	std::wstring wstrExternalToken;
	std::wstring wstrExternalOriginalToken;
	time_t		tLoginBeginTime = 0;
	ULONGLONG	tServerTime = 0;
	ULONGLONG	tLoginTime = 0;
	SchoolVector vecSchoolInfo;
	SubjectVector vecSubjectInfo;
	LibraryVector vecLibraryInfo;
	std::wstring wstrLoginResponse;
	std::wstring wstrStudentInfoResponse;
	std::wstring wstrGroupInfoResponse;
	std::wstring wstrSelfBuiltGroupInfoResponse;
	std::wstring wstrDeviceInfoResponse;
	std::wstring wstrExpiresAt;
	std::wstring wstrMacAlgorithm;
	std::wstring wstrRefreshToken;
	NetDiskInfoModel stNetDiskInfo;
	UCSDK::User* pUser = NULL;
    bool bIsToBUser;
	bool bLoginAtOnce = true; 
	bool bIsFreeClassMode = true;

	JyeooSubjectVector	vecJyeooSubject;
};

struct UserCenterRequest
{
	PVOID pUserData;
	UINT nPriority;
};

struct UserCenterComplete
{
	UserCenterComplete() : dwErrorCode(0){}
	DWORD dwErrorCode;
	std::wstring wstrErrorDescription;
	PVOID pUserData;
};

struct UserCenterClose : public CEventPublisher < UserCenterClose >
{
	bool bLogined = false;
};

//////////////////////////////////////////////////////////////////////////
struct UserCenterSetUserAvatar : public CEventPublisher<UserCenterSetUserAvatar>
{
	std::wstring wstrPath;
};

struct UserCenterSetUserAvatarComplete : public UserCenterComplete, public CEventPublisher<UserCenterSetUserAvatarComplete>
{
	std::wstring wstrPath;

	//是否是修改头像触发
	bool bModify;

	UserCenterSetUserAvatarComplete()
	{
		bModify = true;
	}
};

//////////////////////////////////////////////////////////////////////////
struct AdminSwitchClass : public CEventPublisher<AdminSwitchClass>
{
	std::wstring wstrClassId;
	std::string strSession;
};

struct CompleteAdminSwitchClass : public UserCenterComplete, public CEventPublisher<CompleteAdminSwitchClass>
{
	std::wstring wstrClassId;
	std::string strSession;
};

//////////////////////////////////////////////////////////////////////////
struct AdminFreeClass : public CEventPublisher < AdminFreeClass >
{
	std::wstring wstrClassId;
	std::wstring wstrUserId;
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
struct UserCenterSetProfile : public CEventPublisher<UserCenterSetProfile>
{
	std::wstring wstrRealName;
	std::wstring wstrGender;
	std::wstring wstrBirthday;
	std::wstring wstrMobile;
	std::wstring wstrEmail;
	std::wstring wstrNickName;
};

struct CompleteUserCenterSetProfile : public UserCenterComplete, public CEventPublisher<CompleteUserCenterSetProfile>
{

};

//////////////////////////////////////////////////////////////////////////
struct RequestResetPasswordIdentifyCodeEvent : public CEventPublisher<RequestResetPasswordIdentifyCodeEvent>
{
    
};

struct CompleteResetPasswordIdentifyCode : public UserCenterComplete, public CEventPublisher<CompleteResetPasswordIdentifyCode>
{
    std::wstring wstrFilePath;
    std::wstring wstrSessionId;
    std::wstring wstrSessionKey;
};

struct RequestRegisterIdentifyCodeEvent : public CEventPublisher<RequestRegisterIdentifyCodeEvent>
{
	
};

struct CompleteRegisterIdentifyCode : public UserCenterComplete, public CEventPublisher<CompleteRegisterIdentifyCode>
{
	std::wstring wstrFilePath;
    std::wstring wstrSessionId;
    std::wstring wstrSessionKey;
};

//////////////////////////////////////////////////////////////////////////
struct MobileRegisterEvent : public CEventPublisher<MobileRegisterEvent>
{
	time_t tBeginTime;
	std::wstring wstrMobile;
    std::wstring wstrPassword;
    std::wstring wstrMobileCode;
	std::wstring wstrCountryCode;
};

struct EmailRegisterEvent : public CEventPublisher<EmailRegisterEvent>
{
	time_t tBeginTime;
    std::wstring wstrEmail;
    std::wstring wstrPassword;
    std::wstring wstrLanguage;//默认zh_cn
};

struct CompleteRegister : public UserCenterComplete, public CEventPublisher<CompleteRegister>
{
	std::wstring wstrEmail;
	std::wstring wstrMobile;
};

//////////////////////////////////////////////////////////////////////////
struct UserCenterRequestChangePassword : public CEventPublisher<UserCenterRequestChangePassword>
{
	std::wstring wstrOldPassword;
	std::wstring wstrNewPassword;
};

struct CompleteRequestChangePassword : public UserCenterComplete, public CEventPublisher<CompleteRequestChangePassword>
{

};

//////////////////////////////////////////////////////////////////////////
struct UserEduRequestAccExsit : public CEventPublisher<UserEduRequestAccExsit>
{
	time_t tBeginTime;
	std::wstring wstrAccount;
};

struct CompleteRequestAccExsit : public UserCenterComplete, public CEventPublisher<CompleteRequestAccExsit>
{
	time_t tBeginTime;
	bool bExsit;
    std::wstring wstrMobile;
    std::wstring wstrEmail;
};

//////////////////////////////////////////////////////////////////////////
struct RequestGetBackPasswordSMSEvent : public CEventPublisher<RequestGetBackPasswordSMSEvent>
{
    std::wstring wstrMobile;
    std::wstring wstrIdentifyCode;//图片验证码
    std::wstring wstrSessionId;
    std::wstring wstrCountryCode;
};

struct CompleteGetBackPasswordSMS : public UserCenterComplete, public CEventPublisher<CompleteGetBackPasswordSMS>
{
	
};

struct RequestRegisterSMSEvent : public CEventPublisher<RequestRegisterSMSEvent>
{
	time_t tBeginTime;
	std::wstring wstrMobile;
	std::wstring wstrIdentifyCode;//图片验证码
	std::wstring wstrSessionId;
	std::wstring wstrCountryCode;
};

struct CompleteRegisterSMS : public UserCenterComplete, public CEventPublisher<CompleteRegisterSMS>
{

};

//////////////////////////////////////////////////////////////////////////
struct RequestMobileResetPasswordEvent : public CEventPublisher<RequestMobileResetPasswordEvent>
{
    std::wstring wstrMobile;
    std::wstring wstrNewPassword;
    std::wstring wstrMobileCode;
};

struct CompleteMobileResetPassword : public UserCenterComplete, public CEventPublisher<CompleteMobileResetPassword>
{

};

struct UserEduRequestGetBackPwdSendEmail : public CEventPublisher<UserEduRequestGetBackPwdSendEmail>
{
	std::wstring wstrEmail;
	std::wstring wstrLanguage;
};

struct CompleteGetBackPwdSendEmail : public UserCenterComplete, public CEventPublisher<CompleteGetBackPwdSendEmail>
{

};

//////////////////////////////////////////////////////////////////////////
struct AdminRequestLibraryAuth : public UserCenterRequest, public CEventPublisher<AdminRequestLibraryAuth>
{
	std::wstring wstrKey;
	std::wstring wstrSchoolId;
	std::wstring wstrAreaCode;
	std::wstring wstrRole; // powerUser user guest
	std::wstring wstrRemark;
};

struct CompleteLibraryAuth : public UserCenterComplete, public CTargetEventPublisher<CompleteLibraryAuth, wstring>
{

};

//////////////////////////////////////////////////////////////////////////
struct AdminRequestAreaList : public  UserCenterRequest,  CEventPublisher<AdminRequestAreaList>
{
	std::wstring wstrKey;
	std::wstring wstrAreaCode;
};

struct AdminAreaListModel
{
	std::wstring wstrAreaName;
	std::wstring wstrAreaCode;
};

struct CompleteRequestAreaList : public UserCenterComplete, public CTargetEventPublisher<CompleteRequestAreaList, wstring>
{
	vector<std::shared_ptr<AdminAreaListModel>> vecAreaListModels;
};

//////////////////////////////////////////////////////////////////////////
struct AdminRequestSearchSchool : public UserCenterRequest, public CEventPublisher<AdminRequestSearchSchool>
{
	std::wstring wstrKey;
	std::wstring wstrSchoolName;
	std::wstring wstrAreaCode;
	UINT nStart = 0;
	UINT nLimit = 50;
};

struct AdminSearchSchoolModel
{
	std::wstring wstrSchoolId;
	std::wstring wstrName;
	std::wstring wstrFullName;
	std::wstring wstrAddress;
	std::wstring wstrShortName;
	std::wstring wstrAreaCode;
};

struct CompleteSearchSchool : public UserCenterComplete, public CTargetEventPublisher<CompleteSearchSchool, wstring>
{
	vector<std::shared_ptr<AdminSearchSchoolModel>> vecSchoolModels;
	UINT nStart;
	UINT nLimit;
};

//////////////////////////////////////////////////////////////////////////
struct AdminRequestHeadIcon : public CEventPublisher<AdminRequestHeadIcon>
{
	std::shared_ptr<vector<StudentInfo>> spVecStudentInfo;
};

struct CompleteRequestHeadIcon : public UserCenterComplete, public CEventPublisher<CompleteRequestHeadIcon>
{
	
};

//////////////////////////////////////////////////////////////////////////
struct RequestUserInfo : public CEventPublisher<RequestUserInfo>
{
	std::shared_ptr<UserInfo> spUserInfo;
};

//////////////////////////////////////////////////////////////////////////
struct RequestLoginStatusInfo : public CEventPublisher<RequestLoginStatusInfo>
{
	std::shared_ptr<LoginStatusInfo> spLoginStatusInfo;
};
//////////////////////////////////////////////////////////////////////////
struct RequestLanguageIsZhCn : public CEventPublisher<RequestLanguageIsZhCn>
{
	bool bIsLanguageZhCn;
};
//////////////////////////////////////////////////////////////////////////
struct RequestLanguageIsZhCnEx : public CEventPublisher<RequestLanguageIsZhCnEx>
{
	bool bIsLanguageZhCn;
};

//////////////////////////////////////////////////////////////////////////
struct RequestIsRegionCHS : public CEventPublisher<RequestIsRegionCHS>
{
	bool bIsRegionCHS;
};

//////////////////////////////////////////////////////////////////////////
struct UserEduRequestClassInfo : public CEventPublisher<UserEduRequestClassInfo>
{
	std::wstring wstrOperate;
	std::wstring wstrClassId;
};

struct CompleteRequestClassInfo : public UserCenterComplete, public CEventPublisher<CompleteRequestClassInfo>
{
	std::wstring wstrOperate;
	std::wstring wstrClassId;
};

//////////////////////////////////////////////////////////////////////////
struct UserCenterInternal
{
	void* panCount = NULL;
	string strSession;
};

struct UserCenterRequestDbankInitInternal : 
	public UserCenterInternal,
	public CEventPublisher<UserCenterRequestDbankInitInternal>
{
	//默认初始化获取网盘大小时候，其他时候使用该消息（养成） bInit为flase
	bool bInit = true;
};

struct UserCenterRequestAvatarInternal : 
	public UserCenterInternal, 
	public CEventPublisher<UserCenterRequestAvatarInternal>
{

};

struct UserCenterRequestLibraryInitInternal : 
	public UserCenterInternal, 
	public CEventPublisher<UserCenterRequestLibraryInitInternal>
{

};

struct UserCenterRequestSchoolInfoInternal : 
	public UserCenterInternal,
	public CEventPublisher<UserCenterRequestSchoolInfoInternal>
{
};

struct UserCenterRequestRolesInfoInternal : 
	public UserCenterInternal,
	public CEventPublisher<UserCenterRequestRolesInfoInternal>
{

};

struct UserCenterRequestJYTokenInfoInternal : 
	public UserCenterInternal,
	public CEventPublisher<UserCenterRequestJYTokenInfoInternal>
{

};

struct UserCenterCompleteGetDBankInfo : public UserCenterComplete, public CEventPublisher<UserCenterCompleteGetDBankInfo>
{
	
};

/*
养成领取任务后重新获取网盘大小，通知网盘界面进行刷新消息 区别于UserCenterCompleteGetDBankInfo该消息
该消息回导致传传文件被占用问题
*/
struct UserCenterCompleteGetDBankInfoEx : public UserCenterComplete, public CEventPublisher<UserCenterCompleteGetDBankInfoEx>
{

};

struct UserCenterCompleteGetLibraryInfo : public UserCenterComplete, public CEventPublisher<UserCenterCompleteGetLibraryInfo>
{

};

struct UserCenterCompleteGetZJCategoryInfo : public UserCenterComplete, public CEventPublisher<UserCenterCompleteGetZJCategoryInfo>
{

};

struct UserCenterCompleteUserBizAsycInit : public CEventPublisher<UserCenterCompleteUserBizAsycInit>
{

};

struct CompleteGetSchoolInfo : public UserCenterComplete, public CEventPublisher<CompleteGetSchoolInfo>
{

};

struct CompleteGetRolesInfo : public UserCenterComplete, public CEventPublisher<CompleteGetRolesInfo>
{

};

struct CompleteGetJYTokenInfo : public UserCenterComplete, public CEventPublisher<CompleteGetJYTokenInfo>
{

};

struct CompleteLoginGetChapterInfo : public UserCenterComplete, public CEventPublisher<CompleteLoginGetChapterInfo>
{

};

//////////////////////////////////////////////////////////////////////////
struct UserEduRequestCustomerUnread : public CEventPublisher<UserEduRequestCustomerUnread>
{
	
};

struct CompleteRequestCustomerUnread : public UserCenterComplete, public CEventPublisher<CompleteRequestCustomerUnread>
{
	int nUnreadCount = 0;
};

//////////////////////////////////////////////////////////////////////////
struct UserEduRequestCustomerId : public CEventPublisher<UserEduRequestCustomerId>
{
	string strSdpAppId;
};

struct CompleteRequestCustomerId : public UserCenterComplete, public CEventPublisher<CompleteRequestCustomerId>
{
	std::wstring wstrCode;
	std::wstring wstrCustomerId;
};

//////////////////////////////////////////////////////////////////////////
struct UserEduRequestPostMessages : public CEventPublisher<UserEduRequestPostMessages>
{
	string strSdpAppId;
};

struct CompleteRequestPostMessages : public UserCenterComplete, public CEventPublisher<CompleteRequestPostMessages>
{
	
};

//////////////////////////////////////////////////////////////////////////
struct RequestLoginComponentUserInfo : public CEventPublisher<RequestLoginComponentUserInfo>
{
	bool bSuccess = false;
	std::shared_ptr<UserInfo> spUserInfo;
};

//////////////////////////////////////////////////////////////////////////
struct RequestLoginComponentPrivateUserInfo : public CEventPublisher<RequestLoginComponentPrivateUserInfo>
{
	bool bSuccess = false;
	std::shared_ptr<UserInfo> spPrivateUserInfo;
};

//////////////////////////////////////////////////////////////////////////
struct RequestLoginComponentReleasePrivateUserInfo : public CEventPublisher<RequestLoginComponentReleasePrivateUserInfo>
{
	bool bSuccess = false;
	std::shared_ptr<UserInfo> spReleasePrivateUserInfo;
};

//////////////////////////////////////////////////////////////////////////
struct UserEduRequestUserName : public CEventPublisher<UserEduRequestUserName>
{
	tstring wstrUserId;
	PVOID pUserData = NULL;
	bool bUGCResource = true;
};

struct CompleteRequestUserName : public UserCenterComplete, public CTargetEventPublisher<CompleteRequestUserName, PVOID>
{
	tstring wstrUserId;
	tstring wstrNickName;
	tstring wstrGender;
	bool bUGCResource = true;
};

//////////////////////////////////////////////////////////////////////////
struct UserEduRequestUGCUserInfo : public CEventPublisher<UserEduRequestUGCUserInfo>
{
	tstring wstrUserId;
	PVOID pUserData = NULL;
	bool bUGCResource = true;
};

struct CompleteRequestUGCUserInfo : public UserCenterComplete, public CTargetEventPublisher<CompleteRequestUGCUserInfo, PVOID>
{
	tstring wstrUserId;
	tstring	wstrSchoolName;
	tstring wstrSubjectName;
	tstring	wstrAvatarPath;
	tstring	wstrArea;
	bool bUGCResource = true;
};

//////////////////////////////////////////////////////////////////////////
struct RequestCurUGCInfo : public CTargetEventPublisher<RequestCurUGCInfo, HWND>
{
	tstring wstrNickName;
	tstring wstrGender;
	tstring	wstrSchoolName;
	tstring wstrSubjectName;
	tstring	wstrAvatarPath;
	tstring	wstrArea;
	CControlUI* pControl;
	bool	bUGCResource = true;
};

struct CompleteSkinMessageBox : public CTargetEventPublisher < CompleteSkinMessageBox, HWND >
{
};
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
struct RequestStudentInfo : public CEventPublisher<RequestStudentInfo>
{
    std::shared_ptr<vector<StudentInfo>> spStudentInfo;
};
//////////////////////////////////////////////////////////////////////////

struct RequestNavigationInfo : public CEventPublisher<RequestNavigationInfo>
{
	std::wstring wstrNavId;
	void*  pTargetID = NULL;
};

struct CompleteNavigationInfo : public CEventPublisher<CompleteNavigationInfo>
{
	std::wstring wstrNavId;
	std::string strValue;
};

struct CompleteNavigationInfoTarget : public CTargetEventPublisher<CompleteNavigationInfoTarget, void*>
{
	std::wstring wstrNavId;
	std::string strValue;
};

struct RequestSetNavigationInfo : public CEventPublisher<RequestSetNavigationInfo>
{
	std::wstring wstrNavId;
	std::string strValue;
};

//////////////////////////////////////////////////////////////////////////
struct RequestLessonCode : public CEventPublisher<RequestLessonCode>
{
	std::wstring wstrClassId;
	std::wstring wstrLessonType;
	std::wstring wstrTeacherId;
	bool		 bFromQrcode = true;
	bool		 bNeedRefresh = false;
};

struct CompleteRequestLessonCode : public UserCenterComplete, public CEventPublisher<CompleteRequestLessonCode>
{
	std::wstring wstrClassId;
	std::wstring wstrLessonCode;
	std::wstring wstrTeacherId;
	bool		 bFromQrcode = true;
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
struct RequestRemoveLessonCode : public CEventPublisher<RequestRemoveLessonCode>
{
	std::wstring wstrLessonCode;
};

struct CompleteRemoveLessonCode : public UserCenterComplete, public CEventPublisher<CompleteRemoveLessonCode>
{

};
//////////////////////////////////////////////////////////////////////////
struct RequestSetFrameARedDotTipVisible : public CTargetEventPublisher<RequestSetFrameARedDotTipVisible, tstring>
{
	tstring strRedDotTipId;
	bool	bVisible = false;
};

////////////////////////////////////////////////////////////////////////////
struct RequestGetKeyValueInfo : public CEventPublisher<RequestGetKeyValueInfo>
{
	std::wstring wstrKey;
	void*  pTargetID = NULL;
};

struct RequestSetKeyValueInfo : public CEventPublisher<RequestSetKeyValueInfo>
{
	std::wstring wstrKey;
	std::string strValue;
};

struct CompleteGetKeyValueTarget : public CTargetEventPublisher<CompleteGetKeyValueTarget, void*>
{
	std::wstring wstrKey;
	std::string strValue;
};//////////////////////////////////////////////////////////////////////////

struct RequestUserAuthTeachingmaterial : public CEventPublisher<RequestUserAuthTeachingmaterial>
{
	vector<tstring> vecAuthTeach;
};

struct RequestUserAuthorizationHeader : public CEventPublisher<RequestUserAuthorizationHeader>
{
	std::string strHost;
	std::string strUrl;
	std::string strMethod;
	std::string strAccessToken;
	std::string strMacKey;
	std::string strServerTime;
	time_t tLoginTime;

	__out std::string strAuthorization;
};


struct RequestCurrentSchoolID : public CEventPublisher<RequestCurrentSchoolID>
{
	std::wstring wstrCurSchoolID;
};

struct RequestIsGettingStudentInfo : public CEventPublisher<RequestIsGettingStudentInfo>
{
	bool bGet = false;
};

struct RequestIsGettingSchoolInfo : public CEventPublisher<RequestIsGettingSchoolInfo>
{
	bool bGet = false;
};

struct RequestGetHeadIconJson : public CEventPublisher<RequestGetHeadIconJson>
{
	std::wstring wstrHeadIconJson;
};