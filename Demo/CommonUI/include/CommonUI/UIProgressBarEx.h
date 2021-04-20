#pragma once
class CUI_API CProgressBarUIEx : public CProgressBarUI
{
	UI_OBJECT_DECLARE(CProgressBarUIEx, _T("ProgressBarEx"))
public:
	CProgressBarUIEx();
	virtual void SetPos(const float fPercent, bool bUpdate = false);
	virtual void SetPos(const unsigned long long& curpos, bool bUpdate = false);
	virtual void SetAttribute(LPCTSTR lpszName, LPCTSTR lpszValue);
protected:
	void Render(IRenderDC* pRenderDC, RECT& rcPaint) override;
	virtual void Init()override;
private:
	tstring m_sFinishImage;
	tstring m_sForeImage;

	bool m_bFinish;
	bool m_bTextVisible = true;
	DWORD m_dwFinishTextColor = 1;
	DWORD m_dwForeTextColor;
};