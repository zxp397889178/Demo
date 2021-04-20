#include "stdafx.h"
#include "UIFadeLayout.h"

#define NORMAL_TO_TRANSPARENT_ID 1000
#define TRANSPARENT_TO_NORMAL_ID 1001

#define MIN_ALPHA 105
#define MAX_ALPHA 255

CFadeLayout::CFadeLayout()
{
	m_curAlpha = MAX_ALPHA;
}

CFadeLayout::~CFadeLayout()
{
	KillTimer(TRANSPARENT_TO_NORMAL_ID);
	KillTimer(TRANSPARENT_TO_NORMAL_ID);
}

void CFadeLayout::StartFade()
{
	KillTimer(TRANSPARENT_TO_NORMAL_ID);
	SetTimer(NORMAL_TO_TRANSPARENT_ID, 100);
}

void CFadeLayout::StopFade()
{
	KillTimer(NORMAL_TO_TRANSPARENT_ID);
	SetTimer(TRANSPARENT_TO_NORMAL_ID, 100);
}

bool CFadeLayout::Event(TEventUI& event)
{
	switch (event.nType)
	{
	case UIEVENT_TIMER:
		if (NORMAL_TO_TRANSPARENT_ID == event.wParam)
		{
			const static int nChangeVal = 15;//渐变间隔
			m_curAlpha -= nChangeVal;
			if (m_curAlpha <= MIN_ALPHA)
			{
				m_curAlpha = MIN_ALPHA;
				SetAlpha(m_curAlpha);
				Invalidate();
				KillTimer(NORMAL_TO_TRANSPARENT_ID);
				break;
			}
			SetAlpha(m_curAlpha);
			Invalidate();
		}
		else if (TRANSPARENT_TO_NORMAL_ID == event.wParam)
		{
			const static int nChangeVal = 30;//渐变间隔
			m_curAlpha += nChangeVal;
			if (m_curAlpha >= MAX_ALPHA)
			{
				m_curAlpha = MAX_ALPHA;
				SetAlpha(m_curAlpha);
				Invalidate();
				KillTimer(TRANSPARENT_TO_NORMAL_ID);
				break;
			}
			SetAlpha(m_curAlpha);
			Invalidate();
		}
		default:
			break;
	}
	return __super::Event(event);
}
