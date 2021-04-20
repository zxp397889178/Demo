#pragma once

class CUI_API CRoundProgressBar : public CProgressBarUI
{
	UI_OBJECT_DECLARE(CRoundProgressBar, _T("RoundProgressBar"))
public:
	CRoundProgressBar();
	virtual void SetPos(const float fPercent, bool bUpdate = false);
	void SetShowValue(bool bShowValue);
protected:
	virtual void Init();
	virtual void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	float m_fPercent;
private:
	bool m_bShwValue;
};