#pragma once


class DUI_API CursorObj : public BaseObject
{
public:
	//{{
	CursorObj();
	~CursorObj();
	//}}
	HCURSOR GetHCursor() const;
	void SetHCurosr(HCURSOR val);
	//{{
protected:
	HCURSOR m_hCursor;
private:
	//}}
};

