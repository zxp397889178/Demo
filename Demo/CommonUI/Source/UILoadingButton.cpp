#include "stdafx.h"
#include "UIAnimationCtrl.h"
#include "UILoadingButton.h"

const static TCHAR IdBtn[]		= _T("btn");
const static TCHAR IdLoadingLayout[] = _T("lay");
const static TCHAR IdLoading[] = _T("anim");

CLoadingButtonUI::CLoadingButtonUI()
{
	m_pBtn = NULL;
	m_pLayLoading = NULL;
	m_bFirstPaint = false;
	m_pLoading = NULL;
}

CLoadingButtonUI::~CLoadingButtonUI()
{

}

void CLoadingButtonUI::Init()
{
	__super::Init();
	
	m_pBtn = dynamic_cast<CButtonUI*>(this->FindSubControl(IdBtn));
	m_pLayLoading = dynamic_cast<CContainerUI*>(this->FindSubControl(IdLoadingLayout));

	if (m_pLayLoading)
	{
		m_pLoading = dynamic_cast<CAnimationCtrlUI*>(m_pLayLoading->FindSubControl(IdLoading));
	}

	if (m_pBtn)
	{
		m_pBtn->OnNotify += MakeDelegate(this, &CLoadingButtonUI::OnBtnNotify);
	}
}

bool CLoadingButtonUI::OnBtnNotify(TNotifyUI& notify)
{
	if (notify.nType == UINOTIFY_CLICK)
	{
		DoAction();
	}
	return true;
}

void CLoadingButtonUI::Start()
{
	UpdateState(true);
}

void CLoadingButtonUI::Stop()
{
	UpdateState(false);
}

void CLoadingButtonUI::UpdateState(bool bLoading)
{
	if (m_pBtn)
	{
		m_pBtn->SetVisible(!bLoading);
	}

	if (m_pLayLoading)
	{
		m_pLayLoading->SetVisible(bLoading);
	}

	if (m_pLoading)
	{
		if (bLoading)
		{
			m_pLoading->StartAnimation();
		}
		else
		{
			m_pLoading->StopAnimation();
		}
	}
}

void CLoadingButtonUI::SetBkImage(LPCTSTR lptcsImage)
{
	if (m_pBtn)
	{
		m_pBtn->SetAttribute(_T("bk.image"), lptcsImage);
	}
}

void CLoadingButtonUI::DoAction()
{

}
