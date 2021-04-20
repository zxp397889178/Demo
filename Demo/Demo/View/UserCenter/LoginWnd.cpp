#include "stdafx.h"
#include "Utils/Macros.h"
#include "View\UIEditLayout.h"
#include "Event\LoginEvent.hpp"
#include "Event\UserCenterEventModel.hpp"
#include "LoginWnd.h"
#include "Biz\UserLoginBiz.h"
#include "Biz\UserCenterBiz.h"
#include "Event\LoginEvent.hpp"
#include "UcPullDownWnd.h"


#define  MsgHorOffset     2		//相对于插入按钮的left
#define  MsgVerOffset     -21	//相对于插入按钮的bottom
#define AlwaysPasswordString	_T("●●●●●●")
UCSDK::User*        pUser = NULL;

const tstring UcName = _T("10005555");
const tstring UcPassword = _T("a10005555");

HWND g_hWnd_Logincall = NULL;

CLoginWnd::CLoginWnd()
{
	CUserLoginBiz::GetInstance()->OnInit();
	CUserCenterBiz::GetInstance()->OnInit();
}

CLoginWnd::~CLoginWnd()
{
}

HWND CLoginWnd::CreateWnd(HWND hParent)
{
	SetLayerWindow(true);
	HWND hWnd = Create(hParent, _T("LoginWindow"));

	return hWnd;
}

void CLoginWnd::OnCreate()
{
	m_pLoginTip = dynamic_cast<CLabelUI*>(this->FindControl(_T("login_prompt")));
	m_pLoginingContent = dynamic_cast<CContainerUI*>(this->FindControl(_T("logining")));
	m_pLoginFrameA = dynamic_cast<CLayoutUI*>(this->FindControl(_T("LoginPanel")));
	m_pUserInfoPanel = dynamic_cast<CLayoutUI*>(this->FindControl(_T("basic_layout")));
	m_pLoginError = dynamic_cast<CLabelUI*>(this->FindControl(_T("login_prompt")));

	m_pBtnHidePwd = dynamic_cast<CButtonUI*>(this->FindControl(_T("btn_hide_pwd")));
	m_pBtnShowPwd = dynamic_cast<CButtonUI*>(this->FindControl(_T("btn_show_pwd")));

	m_pBtnPullDown = dynamic_cast<CButtonUI*>(this->FindControl(_T("btn_pull_down")));
	m_pBtnDropDown = dynamic_cast<CButtonUI*>(this->FindControl(_T("btn_drop_down")));

	m_pLoginContent = dynamic_cast<CContainerUI*>(this->FindControl(_T("login")));
	if (m_pLoginContent)
	{
		m_pEditAccount = dynamic_cast<CEditLayoutUI*>(m_pLoginContent->FindSubControl(_T("edit_account")));
		m_pEditPwd = dynamic_cast<CEditLayoutUI*>(m_pLoginContent->FindSubControl(_T("edit_password")));

		if (m_pEditAccount)
		{
			m_pEditAccount->OnEditNotify += MakeDelegate(this, &CLoginWnd::OnAccountEditNotify);
		}

		if (m_pEditPwd)
		{
			m_pEditPwd->OnEditNotify += MakeDelegate(this, &CLoginWnd::OnPwdEditNotify);
		}
	}

	//UserInfoPanel
	if (m_pUserInfoPanel)
	{
		m_pUserInfoId = dynamic_cast<CLabelUI*>(this->FindControl(_T("basic_lable_userid")));
		m_pUserInfoUserName = dynamic_cast<CLabelUI*>(this->FindControl(_T("basic_lable_username")));
		m_pUserInfoRealName = dynamic_cast<CLabelUI*>(this->FindControl(_T("basic_lable_realname")));
		m_pUserInfoNickName = dynamic_cast<CLabelUI*>(this->FindControl(_T("basic_lable_nickname")));
		m_pUserInfoBirthday = dynamic_cast<CLabelUI*>(this->FindControl(_T("basic_lable_birthday")));
		m_pUserInfoSex = dynamic_cast<CLabelUI*>(this->FindControl(_T("basic_lable_sex")));
	}

	UcRequestLastLoginInfo reqInfo;
	reqInfo.hWnd = GetHWND();
	reqInfo.PostInWorkPool();
}

void CLoginWnd::OnClose()
{
	__super::OnClose();
}

bool CLoginWnd::OnBtnCloseLogin(TNotifyUI* msg)
{
	CloseWindow();
	return true;
}

bool CLoginWnd::OnAccountEditNotify(TNotifyUI& notify)
{
	if (notify.nType == UINOTIFY_EN_KEYDOWN
		&& notify.wParam == VK_RETURN)
	{
		if (m_pEditPwd)
		{
			m_pEditPwd->SetFocus();
		}
	}

	if (notify.nType != UINOTIFY_KILLFOCUS)
	{
		return true;
	}

	if (!m_pEditAccount)
	{
		return true;
	}

	// 		UcRequestSetAvatar setAvatar;
	// 		setAvatar.wstrUserName = m_pEditAccount->GetText();
	// 		setAvatar.PostInWorkPool();
	return true;
}

bool CLoginWnd::OnPwdEditNotify(TNotifyUI& notify)
{
	if (notify.nType == UINOTIFY_EN_KEYDOWN
		&& notify.wParam == VK_RETURN)
	{
		TNotifyUI msg;
		OnBtnLogin(&msg);
	}

	if (notify.nType != UINOTIFY_EN_CHANGE)
	{
		return true;
	}

	if (!m_pEditPwd)
	{
		return true;
	}

	if (m_pEditPwd->GetRealText() != AlwaysPasswordString)
	{
		m_strEncrptPwd = _T("");
	}

	return true;
}

bool CLoginWnd::OnBtnLogin(TNotifyUI* msg)
{
	if (!m_pEditAccount
		|| !m_pEditPwd)
	{
		return true;
	}

	if (m_pLoginError)
	{
		m_pLoginError->SetText(_T(""));
	}

	if (m_pLoginTip)
	{
		m_pLoginTip->SetText(_T(""));
	}

	if (!m_pEditAccount->CheckEmptyAndRegex())
		return true;

	if (!m_pEditPwd->CheckEmptyAndRegex())
		return true;

// 		UserCenterRequestCancelLogin reqCancelLogin;
// 		reqCancelLogin.bCancel = false;
// 		reqCancelLogin.Send();//把之前的取消清掉

//	m_bCancelLogin = false;

	//valid empty
	UserCenterRequestAccLogin reqAccLogin;
	reqAccLogin.wstrUserName = m_pEditAccount->GetText();
	reqAccLogin.bEncryptPwd = !m_strEncrptPwd.empty();
	if (!reqAccLogin.bEncryptPwd)
	{
		reqAccLogin.wstrPassword = m_pEditPwd->GetPasswordText();
	}
	else
	{
		reqAccLogin.wstrPassword = m_strEncrptPwd;
	}

	reqAccLogin.stLoginExtra.wstrUserName = m_pEditAccount->GetText();
	reqAccLogin.PostInWorkPool();

	SwitchContent(true);

	return true;
}

void CLoginWnd::OnEvent(UserCenterLoginComplete* pEvent)
{
	SwitchContent(false);
	if (pEvent->dwErrorCode != 0 || !pEvent->pUserInfo)
	{
		if (m_pLoginError)
		{
			m_pLoginError->SetText(pEvent->wstrErrorDescription.c_str());
		}
		GetRoot()->Resize();
		GetRoot()->Invalidate();
		return;
	}

	UcRequestAccSaveInfo reqSave;
	reqSave.wstrUserName = pEvent->pUserInfo->wstrUserName;
	reqSave.wstrUserPwd = pEvent->pUserInfo->wstrEncryptPwd;
	reqSave.PostInWorkPool();

	SetUserInfo(pEvent->pUserInfo);

	m_pLoginFrameA->SetVisible(false);
	m_pUserInfoPanel->SetVisible(true);

	GetRoot()->Resize();
	GetRoot()->Invalidate();

}

void CLoginWnd::OnEvent(CompleteLastLoginInfo* pEvent)
{
	if (pEvent->nInfoCount <= 0)
		return;

	if (m_pEditAccount)
	{
		m_pEditAccount->SetText(pEvent->stLoginParam.strUserName.c_str());
	}

	if (m_pEditPwd)
	{
		m_strEncrptPwd = pEvent->stLoginParam.strUserPassword;
		m_pEditPwd->SetText(AlwaysPasswordString);
	}

	GetRoot()->Resize();
	GetRoot()->Invalidate();
}

void CLoginWnd::OnEvent(CompleteSelectUserInfo* pEvent)
{
	if (!pEvent)
	{
		return;
	}

	if (m_pEditAccount)
	{
		m_pEditAccount->SetText(pEvent->m_stUserInfo.strUserName.c_str());
	}

	if (m_pEditPwd)
	{
		m_strEncrptPwd = pEvent->m_stUserInfo.strUserPassword;
		m_pEditPwd->SetText(AlwaysPasswordString);
	}

	TNotifyUI notify;
	notify.nType = UINOTIFY_KILLFOCUS;
	OnAccountEditNotify(notify);

	Invalidate();
}

void CLoginWnd::SwitchContent(bool bLogining)
{
	if (m_pLoginContent)
	{
		m_pLoginContent->SetVisible(!bLogining);
	}

	if (m_pLoginingContent)
	{
		m_pLoginingContent->SetVisible(bLogining);
	}

	if (m_pLoginFrameA)
	{
		m_pLoginFrameA->Resize();
		m_pLoginFrameA->Invalidate();
	}
}

bool CLoginWnd::OnBtnCancelLogin(TNotifyUI* msg)
{
	UserCenterRequestCancelLogin req;
	req.bCancel = true;
	req.Send();

	return true;
}

bool CLoginWnd::OnBtnLoginOut(TNotifyUI* msg)
{
	UserCenterRequestLogout req;
	req.Post();

	m_pLoginFrameA->SetVisible(true);
	m_pUserInfoPanel->SetVisible(false);

	GetRoot()->Resize();
	GetRoot()->Invalidate();

	return true;
}

void CLoginWnd::SetUserInfo(std::shared_ptr<UserInfo>& pUserInfo)
{
	if (!pUserInfo)
		return;

	if (m_pUserInfoId)
		m_pUserInfoId->SetText(pUserInfo->wstrUserId.c_str());
	if (m_pUserInfoUserName)
		m_pUserInfoUserName->SetText(pUserInfo->wstrUserName.c_str());
	if (m_pUserInfoRealName)
		m_pUserInfoRealName->SetText(pUserInfo->wstrRealName.c_str());
	if (m_pUserInfoNickName)
		m_pUserInfoNickName->SetText(pUserInfo->wstrNickName.c_str());
	if (m_pUserInfoBirthday)
		m_pUserInfoBirthday->SetText(pUserInfo->wstrBirthday.c_str());
	if (m_pUserInfoSex)
		m_pUserInfoSex->SetText(pUserInfo->wstrGender.c_str());
}

HWND CLoginWnd::GetTargetId()
{
	return GetHWND();
}

bool CLoginWnd::OnBtnHidePassword(TNotifyUI* msg)
{
	if (m_pEditPwd)
	{
		m_pEditPwd->SetTextMode(true);
	}

	if (m_pBtnShowPwd)
	{
		m_pBtnShowPwd->SetVisible(true);
	}

	if (m_pBtnHidePwd)
	{
		m_pBtnHidePwd->SetVisible(false);
		m_pBtnHidePwd->GetParent()->Resize();
		m_pBtnHidePwd->GetParent()->Invalidate();
	}
	return true;
}

bool CLoginWnd::OnBtnShowPassword(TNotifyUI* msg)
{
	if (m_pEditPwd)
	{
		m_pEditPwd->SetTextMode(false);
	}

	if (m_pBtnShowPwd)
	{
		m_pBtnShowPwd->SetVisible(false);
	}

	if (m_pBtnHidePwd)
	{
		m_pBtnHidePwd->SetVisible(true);
		m_pBtnHidePwd->GetParent()->Resize();
		m_pBtnHidePwd->GetParent()->Invalidate();
	}

	return true;
}

bool CLoginWnd::OnBtnPullDown(TNotifyUI* msg)
{
	CControlUI * pControl = msg->pSender;
	if (!pControl)
	{
		return true;
	}

	CUcPullDownWnd* pUcPullDownWnd = new_nothrow CUcPullDownWnd;
	pUcPullDownWnd->SetAutoDel(true);
	pUcPullDownWnd->CreateWnd(GetHWND(), m_pEditAccount);
	pUcPullDownWnd->ShowWindow();

	if (m_pBtnDropDown)
	{
		pControl->SetVisible(false);
		m_pBtnDropDown->SetVisible(true);
	}

	return true;
}

bool CLoginWnd::OnBtnDropDown(TNotifyUI* msg)
{
	CControlUI * pControl = msg->pSender;
	if (pControl)
	{
		pControl->SetVisible(false);
	}

	if (m_pBtnPullDown)
	{
		m_pBtnPullDown->SetVisible(true);
	}

	return true;
}
