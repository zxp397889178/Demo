#pragma once

class CUI_API COptionLayoutUIEx :
    public CLayoutUI
{
public:

    virtual ~COptionLayoutUIEx();

    virtual bool Add(CControlUI* pControl);

    virtual bool OnChildNotify(TNotifyUI& msg);

    virtual bool Event(TEventUI& event);
};

class CUI_API CShadowOptionLayoutUI : public COptionLayoutUIEx
{
	UI_OBJECT_DECLARE(CShadowOptionLayoutUI, _T("ShadowOptionLayout"))

public:
	void SetMaxLines(int nMaxLines);

	virtual bool Resize();

protected:
	virtual void OnProcessScrollbar(int cxRequired, int cyRequired);
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);

private:
	void HandleOutOfLines();

private:
	int m_nMaxLines = -1;
};
