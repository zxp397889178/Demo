#include "stdafx.h"
#include "UIVisibleComposite.h"

CVisibleComposite::CVisibleComposite()
{

}

CVisibleComposite::~CVisibleComposite()
{

}

void CVisibleComposite::SetVisible(bool bVisible)
{
    __super::SetVisible(bVisible);
    bool bIsVisible = bVisible && this->IsInternVisible();
    if (bVisible == bIsVisible)
    {
        OnVisible(&bIsVisible);
    }
}

void CVisibleComposite::SetInternVisible(bool bVisible)
{
    __super::SetInternVisible(bVisible);
    bool bIsVisible = bVisible && this->IsVisible();
    if (bVisible == bIsVisible)
    {
        OnVisible(&bIsVisible);
    }
}
