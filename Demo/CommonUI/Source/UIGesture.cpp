#include "stdafx.h"
#include "UIGesture.h"
#include <boost/bind.hpp>
#include <intsafe.h>
#include <uxtheme.h>
#include <boost/dll.hpp>

long xOverpan = 0;
long yOverpan = 0;
CContainerUI* sel = nullptr;
POINTS last = {};
GestureAction  cur_ga = {};
boost::dll::shared_library user32dll;
boost::dll::shared_library uxdll;

CContainerUI* HitTest(HWND wnd, const POINTS& pts)
{
	auto pWnd = static_cast<CWindowUI*>(CWindowUI::FromHandle(wnd));
	if (!pWnd)
		return nullptr;

	POINT test{ pts.x, pts.y };
	::ScreenToClient(wnd, &test);

	return singleton<CGestureSubject>::instance()->HitTest(pWnd, test, cur_ga);
}

LRESULT DecodeGesture(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	GESTUREINFO gi = {};
	gi.cbSize = sizeof(GESTUREINFO);

	auto handle = false;

	if (!user32dll.is_loaded() || !uxdll.is_loaded())
	{
		user32dll.load("user32.dll", boost::dll::load_mode::search_system_folders);
		uxdll.load("UxTheme.dll", boost::dll::load_mode::search_system_folders);
	}

	if (!user32dll.has("GetGestureInfo"))
		return DefWindowProc(hWnd, message, wParam, lParam);

	static auto get_gesture_info = user32dll.get<BOOL WINAPI(_In_ HGESTUREINFO hGestureInfo, _Out_ PGESTUREINFO pGestureInfo)>("GetGestureInfo");
	static auto begin_panning_feedback = uxdll.get<BOOL WINAPI(__in HWND hwnd)>("BeginPanningFeedback");
	static auto end_panning_feedback = uxdll.get<BOOL WINAPI(__in HWND hwnd, __in BOOL fAnimateBack)>("EndPanningFeedback");
	static auto update_panning_feedback = uxdll.get<BOOL WINAPI(__in HWND hwnd, __in LONG lTotalOverpanOffsetX, __in LONG lTotalOverpanOffsetY, __in BOOL fInInertia)>("UpdatePanningFeedback");
	
	if (get_gesture_info && get_gesture_info((HGESTUREINFO)lParam, &gi))
	{
		switch (gi.dwID)
		{
		case GID_BEGIN:
		{
			sel = HitTest(hWnd, gi.ptsLocation);
		}
		break;
		case GID_PAN:
		{
			if (!sel)
				sel = HitTest(hWnd, gi.ptsLocation);

			if (!sel)
				break;

			if (gi.dwFlags & GF_BEGIN)
			{
				if (begin_panning_feedback)
					begin_panning_feedback(hWnd);

				last = gi.ptsLocation;
				xOverpan = yOverpan = 0;
			}
			else if (gi.dwFlags & GF_END)
			{
				if (end_panning_feedback)
					end_panning_feedback(hWnd, TRUE);

				sel = nullptr;
				cur_ga = {};
				xOverpan = yOverpan = 0;
			}
			else
			{
				auto offsetx = gi.ptsLocation.x - last.x;
				auto offsety = gi.ptsLocation.y - last.y;

				xOverpan -= last.x - gi.ptsLocation.x;
				yOverpan -= last.y - gi.ptsLocation.y;

				last = gi.ptsLocation;

				if (!update_panning_feedback)
					break;

				if (cur_ga.XScrollFunc && cur_ga.XScrollFunc(offsetx))
					update_panning_feedback(hWnd, xOverpan, 0, gi.dwFlags & GF_INERTIA);

				if (cur_ga.YScrollFunc && cur_ga.YScrollFunc(-offsety))
					update_panning_feedback(hWnd, 0, yOverpan, gi.dwFlags & GF_INERTIA);
			}

			handle = true;
		}
		break;
		default:
			break;
		}
	}

	if (handle)
		return 0;
	else
		return DefWindowProc(hWnd, message, wParam, lParam);
}

bool CGestureSubject::RegisterCtrl(CContainerUI* key, const GestureAction& ga)
{
	if (!key)
		return false;

	return gestures_.insert(std::make_pair(key, ga)).second;
}

bool CGestureSubject::UnregisterCtrl(CContainerUI* key)
{
	return 1 == gestures_.erase(key);
}

CContainerUI* CGestureSubject::HitTest(CWindowUI* pWnd, POINT& test, GestureAction& cur_ga) const
{
	for (auto& g : gestures_)
	{
		auto ctrl = g.first;
		auto& ga = g.second;

		auto hit_test = ga.HitTestFunc ? ga.HitTestFunc : [&](CWindowUI* touch, const POINT& test){return (ctrl && ctrl->HitTest(POINT(test)) && ctrl->IsInternVisible() && touch == ctrl->GetWindow()); };

		if (hit_test(pWnd, test))
		{
			cur_ga = ga;
			return ctrl;
		}
	}

	return nullptr;
}

gesture::CBasicGestureSupport::~CBasicGestureSupport()
{
	UninitGesture();
}

void gesture::CBasicGestureSupport::InitGesture(CContainerUI* pGrid)
{
	UninitGesture();

	m_pGrid = pGrid;

	GestureAction ga;
	ga.HitTestFunc = std::bind(&gesture::CBasicGestureSupport::HitTestFunc, this, std::placeholders::_1, std::placeholders::_2);
	ga.XScrollFunc = std::bind(&gesture::CBasicGestureSupport::XScroll, this, std::placeholders::_1);
	ga.YScrollFunc = std::bind(&gesture::CBasicGestureSupport::YScroll, this, std::placeholders::_1);

	singleton<CGestureSubject>::instance()->RegisterCtrl(pGrid, ga);
}

void gesture::CBasicGestureSupport::UninitGesture()
{
	if (m_pGrid)
		singleton<CGestureSubject>::instance()->UnregisterCtrl(m_pGrid);

	m_pGrid = nullptr;
}

void gesture::CBasicGestureSupport::SetHitRegion(const RECT& region)
{
	m_rcHitRegion = region;
}

bool gesture::CBasicGestureSupport::HitTestFunc(CWindowUI* touch, const POINT& test)
{
	if (FALSE == ::IsRectEmpty(&m_rcHitRegion))
	{
		if (TRUE == ::PtInRect(&m_rcHitRegion, test))
			return true;
	}

	auto pCurLayout = GetCurGrid();
	if (!pCurLayout)
		return false;

	return (pCurLayout && pCurLayout->HitTest(POINT(test)) && pCurLayout->IsInternVisible() && touch == pCurLayout->GetWindow());
}

bool gesture::CBasicGestureSupport::OnNotXSrcoll(int offsetx)
{
	return false;
}

void gesture::CBasicGestureSupport::OnHaveXSrcoll(int offsetx)
{
	auto pCurLayout = GetCurGrid();
	if (!pCurLayout)
		return;

	if (auto pHScroll = pCurLayout->GetHScrollbar())
		pHScroll->SetScrollPos(pHScroll->GetScrollPos() + offsetx);
}

bool gesture::CBasicGestureSupport::OnXScrollEndFeedback(int offsetx)
{
	return true;
}

bool gesture::CBasicGestureSupport::OnNotYSrcoll(int offsetx)
{
	return false;
}

void gesture::CBasicGestureSupport::OnHaveYSrcoll(int offsety)
{
	auto pCurLayout = GetCurGrid();
	if (!pCurLayout)
		return;

	if (auto pVScroll = pCurLayout->GetVScrollbar())
		pVScroll->SetScrollPos(pVScroll->GetScrollPos() + offsety);
}

bool gesture::CBasicGestureSupport::OnYScrollEndFeedback(int offsety)
{
	return true;
}

gesture::CBasicGestureSupport::CBasicGestureSupport()
{

}

CContainerUI* gesture::CBasicGestureSupport::GetCurGrid()
{
	return m_pGrid;
}

bool gesture::CBasicGestureSupport::XScroll(int offsetx)
{
	auto pCurLayout = GetCurGrid();
	if (!pCurLayout)
		return false;

	auto pHScroll = pCurLayout->GetHScrollbar();

	auto bFeedback = false;

	if (pHScroll)
	{
		bool bXEndFeedback = false;
		auto nCurPos = pHScroll->GetScrollPos();
		auto nMaxLen = pHScroll->GetScrollRange();

		if (nCurPos <= 0 || nCurPos >= nMaxLen)
		{
			auto test = nCurPos + offsetx;

			bXEndFeedback = test < 0 || test > nMaxLen;
		}

		if (bXEndFeedback)
			bFeedback = OnXScrollEndFeedback(offsetx);
		else
			OnHaveXSrcoll(offsetx);
	}
	else
	{
		bFeedback = OnNotXSrcoll(offsetx);
	}

	return bFeedback;
}

bool gesture::CBasicGestureSupport::YScroll(int offsety)
{
	auto pCurLayout = GetCurGrid();
	if (!pCurLayout)
		return false;

	auto pVScroll = pCurLayout->GetVScrollbar();

	auto bFeedback = false;

	if (pVScroll)
	{
		bool bYEndFeedback = false;
		auto nCurPos = pVScroll->GetScrollPos();
		auto nMaxLen = pVScroll->GetScrollRange();

		if (nCurPos <= 0 || nCurPos >= nMaxLen)
		{
			auto test = nCurPos + offsety;

			bYEndFeedback = test < 0 || test > nMaxLen;
		}

		if (bYEndFeedback)
			bFeedback = OnYScrollEndFeedback(offsety);
		else
			OnHaveYSrcoll(offsety);
	}
	else
	{
		bFeedback = OnNotYSrcoll(offsety);
	}

	return bFeedback;
}

void gesture::CGestureLayout::Init()
{
	__super::Init();
	InitGesture(this);
}

void gesture::CGestureLayout::OnDestroy()
{
	__super::OnDestroy();
	UninitGesture();
}
