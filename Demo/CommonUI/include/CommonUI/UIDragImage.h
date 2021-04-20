#pragma once
#include <functional>


class CUI_API CDragImageUI : public CControlUI, public INotifyUI
{
	typedef std::function<bool(float)> SizePercentChangeCallback;
#define MAX_BUF 1024
	UI_OBJECT_DECLARE(CDragImageUI, _T("DragImageControl"));
	UIBEGIN_MSG_MAP

	UIEND_MSG_MAP
	CDragImageUI();
	virtual ~CDragImageUI();
public:
	virtual void	Init();
	virtual bool Event(TEventUI& msg);
	void SetImage(LPCTSTR sFile);
	const tstring CalPicPos(const tstring & sFile);
	void ResizeImage(float fPercent);
	void Render(IRenderDC* pRenderDC, RECT& rcPaint);
	void CheckOffet();
	void RotateImage(REAL fAngle);
	void SetSizePercentChangeCallback(SizePercentChangeCallback cb);
	int AdjustBright(int nBrightLevel);//bAddOrDel true=Add false=Del
	void OnDestroy()override;
	bool SaveAsImage(const tstring & strFilePath,bool bQuality, tstring & strErr);
private:
	bool m_bDrag = false;
	POINT m_pointLast;
	RectF m_rectPaint;
	POINTF m_sizeOffset;
	SizeF m_sizePicture;
	SizeF m_sizeDragRange;
	float m_fPercent = 1.0;
	tstring m_strImagePath;
	REAL  m_fAngle = 0.0;
	SizePercentChangeCallback m_fnCb;
	Gdiplus::Bitmap *m_pBmp = nullptr;
	int m_nBitmapIndex = 0;
	map<int, Gdiplus::Bitmap * > m_mapBitmaps;
	
};