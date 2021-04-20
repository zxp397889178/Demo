#include "stdafx.h"
#include "UITransition.h"


ITransition::ITransition()
{
	m_pAnimation = NULL;
}

void ITransition::SetTransitionAnimation( TransitionAnimation* pAnimation )
{
	CControlUI* pControl = dynamic_cast<CControlUI*>(this);
	if (m_pAnimation)
		ActionManager::GetInstance()->removeViewAnimationByTarget(pControl);
	m_pAnimation = pAnimation;
}

void ITransition::Transition( ImageObj* pOutImageObj, ImageObj* pInImageObj, WPARAM wParam, LPARAM lParam )
{
	CControlUI* pControl = dynamic_cast<CControlUI*>(this);
	if (pControl->GetWindow() && pControl->GetWindow()->IsWindowRender() && m_pAnimation)
	{
		ImageStyle* pOutImage = dynamic_cast<ImageStyle*>(pControl->GetStyle(_T("out")));
		if (pOutImage)
		{
			if (pOutImageObj)
			{
				pOutImage->SetImage(pOutImageObj);
				pOutImage->SetVisible(true);
			}
		}
		ImageStyle* pInImage = dynamic_cast<ImageStyle*>(pControl->GetStyle(_T("in")));
		if (pInImage)
		{
			if (pInImageObj)
			{
				pInImage->SetImage(pInImageObj);
				pInImage->SetVisible(false);
			}
		}

		if (pOutImageObj && pInImageObj)
		{
			if (m_pAnimation && m_pAnimation->GetIn() == NULL && m_pAnimation->GetOut() == NULL)
			{
				m_pAnimation->BindObj(pOutImage, pInImage);
				m_pAnimation->SetActionListener(this);
				m_pAnimation->FinishedOfRelease(false);
				ActionManager::GetInstance()->addViewAnimation(m_pAnimation, pControl);
				m_pAnimation->Paused();
			}
			m_pAnimation->SetCurFrame(0, false);
			if (OnTransition(wParam, lParam))
				m_pAnimation->Resume();
			else
			{
				if (pOutImage)
					pOutImage->SetVisible(false);
				m_pAnimation->Paused();
			}
		}
		
	}
}

TransitionAnimation* ITransition::GetTransitionAnimation()
{
	return m_pAnimation;
}

//////////////////////////////////////////////////////////////////////////
CTransitionUI::CTransitionUI()
:m_szCurSel(_T(""))
,m_pCurSel(NULL)
{
	SetStyle(_T("Transition"));
	SlideTransition* pAnimation = new SlideTransition;
	pAnimation->SetCurveID(_T("curve.quad.out"));
	pAnimation->SetParam(20, 8);
	SetTransitionAnimation(pAnimation);
}

CTransitionUI::~CTransitionUI()
{

}


void CTransitionUI::Render( IRenderDC* pRenderDC, RECT& rcPaint )
{
	if (IsRunning())
	{
		RECT rcTemp = {0};
		RECT rcItem = GetRect();
		if( ::IntersectRect(&rcTemp, &rcPaint, &rcItem)) 
		{
			ClipObj clip;
			ClipObj::GenerateClipWithAnd(pRenderDC->GetDC(), rcTemp, clip);
			CControlUI::Render(pRenderDC, rcPaint);
		}
	}
	else
	{
		__super::Render(pRenderDC, rcPaint);
	}
}

bool CTransitionUI::Remove(CControlUI* pControl)
{
	if (m_pCurSel == pControl)
		m_pCurSel = NULL;
	return __super::Remove(pControl);
}

void CTransitionUI::RemoveAll()
{
	m_pCurSel = NULL;
	__super::RemoveAll();
}

void CTransitionUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (equal_icmp(lpszName, _T("Sel")))
		m_szCurSel = lpszValue;
	else
		__super::SetAttribute(lpszName, lpszValue);
}

bool CTransitionUI::SelectItem(CControlUI* pControl)
{
	if( pControl == m_pCurSel ) return true;
	CControlUI* pOutControl = NULL;
	if( m_pCurSel) 
	{
		m_pCurSel->SetVisible(false);
		pOutControl = m_pCurSel;
	}

	m_pCurSel = pControl;
	CControlUI* pInControl = NULL;
	if (pControl != NULL)
	{
		m_pCurSel->SetVisible(true);
		pInControl = m_pCurSel;
	}
	Resize();
	ImageObj* pOutImageObj = NULL;
	if (pOutControl)
		pOutImageObj = pOutControl->GenerateImage();
	ImageObj* pInImageObj = NULL;
	if (pInControl)
		pInImageObj = pInControl->GenerateImage();
	Transition(pOutImageObj, pInImageObj, (WPARAM)pOutControl, (LPARAM)pInControl);
	if (pOutImageObj)
		pOutImageObj->Release();
	if (pInImageObj)
		pInImageObj->Release();
	Invalidate();
	SendNotify(UINOTIFY_SELCHANGE, (WPARAM)pOutControl, LPARAM(m_pCurSel));
	return true;
}

int CTransitionUI::GetCurSel()
{
	if (m_pCurSel)
		return m_pCurSel->GetIndex();
	return -1;
}

void CTransitionUI::Init()
{
	__super::Init();
	if (!m_szCurSel.empty())
	{
		int nCnt = GetCount();
		CControlUI* pSel = GetItem(m_szCurSel.c_str());
		for (int i = 0; i < nCnt; i++)
		{
			CControlUI* pControl = GetItem(i);
			if (pControl)
			{
				if (pSel != pControl)
					pControl->SetVisible(false);
				else
					pControl->SetVisible(true);
			}
		}
		if (pSel)
			SelectItem(pSel);
	}
}

bool CTransitionUI::OnTransition( WPARAM wParam, LPARAM lParam )
{
	CControlUI* pOut = (CControlUI*)(wParam);
	CControlUI* pIn = (CControlUI*)(lParam);
	if (pOut && pIn)
	{
		SlideTransition* pSlideTransition = dynamic_cast<SlideTransition*>(GetTransitionAnimation());
		if (pSlideTransition)
		{
			if (pOut->GetIndex() < pIn->GetIndex())
			{
				pSlideTransition->SetSlideType(SlideTransition::SlideAnimation_Left);
			}
			else
			{
				pSlideTransition->SetSlideType(SlideTransition::SlideAnimation_Right);
			}
		}
	}
	return true;
}

void CTransitionUI::OnActionStateChanged( CAction* pAction )
{
	CControlUI* pControl = this;
	if (pControl && pAction->GetState() == CAction::ActionState_Finished)
	{
		ImageStyle* pOut = dynamic_cast<ImageStyle*>(pControl->GetStyle(_T("out")));
		if (pOut)
		{
			pOut->SetImage(_T(""));
			pOut->SetVisible(false);
		}
		ImageStyle* pIn = dynamic_cast<ImageStyle*>(pControl->GetStyle(_T("in")));
		if (pIn)
		{
			pIn->SetImage(_T(""));
			pIn->SetVisible(false);
		}
		pControl->Invalidate();
	}
}
