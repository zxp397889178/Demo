#pragma once

class CUI_API CDwmWindowUI :public CWindowUI
{

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam); 
	virtual void OnCreate();
	virtual void OnSetWindowRgn();
	virtual void OnDrawBefore(IRenderDC* pRenderDC, RECT& rc, RECT& rcPaint);
	virtual void OnDrawAfter(IRenderDC* pRenderDC, RECT& rc, RECT& rcPaint);

private:
	UITYPE_FONT m_eDefaultFontType;
};
