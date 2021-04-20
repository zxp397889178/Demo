#include "stdafx.h"
#include "UILayoutButton.h"

CUILayoutButton::CUILayoutButton()
{

}

CUILayoutButton::~CUILayoutButton()
{

}

bool CUILayoutButton::Activate()
{
    if (!IsEnabled())
        return true;
    if (!__super::Activate())
        return false;
    SendNotify(UINOTIFY_CLICK, NULL, NULL);
    return true;
}

bool CUILayoutButton::Event(TEventUI& event)
{
    switch (event.nType)
    {
    case UIEVENT_SETFOCUS:
    {
        SetState(UISTATE_FOCUS);
        Invalidate();
    }
        break;
    case UIEVENT_BUTTONDOWN:
    {
        SetState(UISTATE_DOWN);
        Invalidate();
    }
        break;

    case UIEVENT_BUTTONUP:
    {
        if (PtInRect(GetRectPtr(), event.ptMouse))
        {
            SetState(UISTATE_OVER);
            Invalidate();
            Activate();
        }
        else
        {
            SetState(UISTATE_NORMAL);
            Invalidate();
        }
    }
        break;
    case UIEVENT_LDBCLICK:
    {
        this->SendNotify(UINOTIFY_DBCLICK, NULL, NULL);
    }
        break;
    case UIEVENT_RBUTTONUP:
    {
        this->SendNotify(UINOTIFY_RCLICK, NULL, NULL);
    }
        break;
    case UIEVENT_MOUSEENTER:
    {
        SetState(UISTATE_OVER);
        Invalidate();
    }
        break;
    case UIEVENT_KILLFOCUS:
    {
        SetState(UISTATE_NORMAL);
        Invalidate();
    }
        break;
    case UIEVENT_MOUSELEAVE:
    {
        SetState(UISTATE_NORMAL);
        Invalidate();
    }
        break;
    case UIEVENT_KEYDOWN:
    {
        if (event.wParam == VK_SPACE
            || event.wParam == VK_RETURN)
        {
            Activate();
        }
    }
        break;
    }
    return __super::Event(event);
}

