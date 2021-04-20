#include "stdafx.h"
#include "UIMarqueeLabel.h"

const float gMoveRate = 1.07f;

CMarqueeLabelUI::CMarqueeLabelUI()
{
	m_nTxtWidth = 0;
	m_nLastTxtWidth = 0;
	m_pAlphaChange = NULL;
	SetRectEmpty(&m_rectTxt);
}

CMarqueeLabelUI::~CMarqueeLabelUI()
{
	if (m_pAlphaChange)
	{
		delete m_pAlphaChange;
		m_pAlphaChange = NULL;
	}
}

void CMarqueeLabelUI::SetRect(RECT& rectRegion)
{
	CViewUI::SetRect(rectRegion);
	TextStyle* pStyle = GetTextStyle(_T("txt"));
	if (pStyle)
	{
		pStyle->OnSize(rectRegion);
		RECT rect = pStyle->GetRect();
		RECT rect1 = rect;
		OffsetRect(&rect, -rect.left, -rect.top);
		pStyle->GetFontObj()->CalcText(GetWindow()->GetPaintDC(), rect, m_strText.c_str(), pStyle->GetAlign());
		RECT rcItem = GetRect();

		m_nTxtWidth = rect.right - rect.left;

	}

	FlipRect();

	RECT rcItem = GetRect();
	int nCnt = GetStyleCount();
	for (int i = 0; i < nCnt; i++)
	{
		StyleObj* pStyleObj = (StyleObj*)GetStyle(i);
		if (pStyleObj)
			pStyleObj->OnSize(rcItem);
	}
}

void CMarqueeLabelUI::ShowAnimation()
{
	bool bWndVisible = (GetWindow()->GetHWND() && ::IsWindowVisible(GetWindow()->GetHWND()));
	if (m_rcItem.right - m_rcItem.left >= m_nTxtWidth
		|| !IsVisible()
		|| !IsInternVisible()
		|| !bWndVisible)
	{
		RECT rt = m_rcItem;
		rt.left = m_rcItem.left + (m_rcItem.right - m_rcItem.left - m_nTxtWidth) / 2;
		rt.right = rt.left + m_nTxtWidth;
		TextStyle* pStyle = GetTextStyle(_T("txt"));
		if (pStyle)
		{
			pStyle->SetRect(rt); //不显示跑马灯，文字居中
		}

		return;
	}	

	InitPosAnimation();

	if (m_pAlphaChange)
	{
		m_pAlphaChange->SetCurFrame(0, false);
		m_pAlphaChange->Resume();
	}
}

void CMarqueeLabelUI::StopAnimation()
{	
	if (m_pAlphaChange)
	{
		m_pAlphaChange->Stop();
	}
}

void CMarqueeLabelUI::OnActionStateChanged(CAction* pAction)
{
	if (_tcsicmp(pAction->GetId(), _T("AutoSizeRun")) == 0)
	{
		if (pAction->GetState() == CAction::ActionState_Finished)
		{
			TextStyle* pStyle = GetTextStyle(_T("txt"));
			if (pStyle)
			{
				pStyle->SetRect(m_rectTxt);
			}

			if (m_pAlphaChange)
			{
				m_pAlphaChange->Stop();
			}
			
			ShowAnimation();
		}
	}
}

void CMarqueeLabelUI::SetVisible(bool bVisible)
{
	__super::SetVisible(bVisible);

	ShowAnimation();
}

void CMarqueeLabelUI::SetInternVisible(bool bVisible)
{
	__super::SetInternVisible(bVisible);

	ShowAnimation();
}

void CMarqueeLabelUI::OnSize(const RECT& rectParent)
{
	__super::OnSize(rectParent);
	ShowAnimation();
}

void CMarqueeLabelUI::InitPosAnimation()
{
	TextStyle* pTxtStyle = static_cast<TextStyle*>(GetStyle(_T("txt")));
	if (!pTxtStyle)
	{
		return;
	}

	if (!m_pAlphaChange)
	{
		m_pAlphaChange = new_nothrow PosChangeAnimation;		

		if (!m_pAlphaChange)
		{
			return;
		}		

		m_pAlphaChange->SetCurveID(_T("curve.liner"));
		
		m_pAlphaChange->FinishedOfRelease(false);
		m_pAlphaChange->SetActionListener(this);
		m_pAlphaChange->SetId(_T("AutoSizeRun"));	
		ActionManager::GetInstance()->addViewAnimation(m_pAlphaChange, pTxtStyle, true);
	}	

	if (m_nLastTxtWidth != m_nTxtWidth)
	{
		m_nLastTxtWidth = m_nTxtWidth;
		m_rectTxt = pTxtStyle->GetRect();

		Coordinate stBeginCoordinate;
		stBeginCoordinate.anchor = UIANCHOR_LTLB;

		stBeginCoordinate.rect.left = m_rcItem.right - m_rcItem.left; //头从最左边开始移动
		stBeginCoordinate.rect.top = m_rectTxt.top - m_rcItem.top;
		stBeginCoordinate.rect.right = m_nTxtWidth + stBeginCoordinate.rect.left;
		stBeginCoordinate.rect.bottom = m_rcItem.bottom - m_rectTxt.bottom;

		Coordinate stEndCoordinate;
		stEndCoordinate.anchor = UIANCHOR_RTRB;
		stEndCoordinate.rect.right = m_rcItem.right - m_rcItem.left;//尾移动到最右边结束移动
		stEndCoordinate.rect.left = stEndCoordinate.rect.right + m_nTxtWidth;
		stEndCoordinate.rect.top = m_rectTxt.top - m_rcItem.top;
		
		stEndCoordinate.rect.bottom = m_rcItem.bottom - m_rectTxt.bottom;

		int nTotalFrame = (int)(m_nTxtWidth * gMoveRate);
		m_pAlphaChange->SetParam(20, nTotalFrame);
		m_pAlphaChange->SetKeyFrameCoordinate(&stBeginCoordinate, &stEndCoordinate);		
		m_pAlphaChange->Stop();
	}		
}

void CMarqueeLabelUI::SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue)
{
	if (_tcsicmp(lpszName, _T("text")) == 0)
	{
		StopAnimation();
	}

	__super::SetAttribute(lpszName, lpszValue);
}
