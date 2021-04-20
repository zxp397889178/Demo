#pragma once

class DibObj;
class DUI_API ImageObj
{
public:
	//{{
#ifdef _GDIPLUS_SUPPORT
	ImageObj(Bitmap* bitmap, IStream* pStream = NULL);
	//}}

	void SetBitmap(Bitmap* bitmap, IStream* pStream = NULL);
	Bitmap*  GetBitmap();
	IStream* GetIStream();

	//{{
#endif
	ImageObj(HBITMAP hBitmap);
	ImageObj(HICON hIcon);
	ImageObj(BYTE* pPixels, int nWidth, int nHeight);
	virtual ~ImageObj(void);
	//}}

	void SetHBitmap(HBITMAP hBitmap);
	HBITMAP  GetHBitmap();
	
	void SetHIcon(HICON hIcon);
	HICON  GetHIcon();

	void SetPixels(BYTE* pPixels, int nWidth, int nHeight);
	BYTE*  GetPixels();

	void Copy(DibObj* pDibObj);
	void Copy(ImageObj* pImageObj);
	void Detach();
	bool Reload();

	BYTE* LockBits(LPVOID* lppvoid);
	void  UnLockBits(LPVOID lpvoid);
public:
	void Release();
	ImageObj* Get();
	

public:
	int		 GetCellWidth()	    {return m_iCellWidth;}
	int		 GetCellHeight()	{return m_iCellHeight;}
	int		 GetWidth()		    {return m_nWidth;}
	int		 GetHeight()		{return m_nHeight;}
	void	 SetWidth(int nWidth);
	void	 SetHeight(int nHeight);
	int		 GetFrameCount()	{return m_nFrameCount;}
	long     GetFrameDelay (int nFrame);
	void     SetActiveFrame(int nFrame);
	bool	 GetAdjustColor()	{return m_bNeedAdjustColor;}
	void     SetAdjustColor(bool bAdjustColor);
	LPCTSTR  GetFilePath();
	void     SetFilePath(LPCTSTR lpszPath);
	void     SetInset(RECT& rect);
	int		 GetCellNum();//�����������
	void     SetCellNum(int nRows, int nCols);
	int		 GetCellRow(){return m_iNumCellRows;}
	int		 GetCellCol(){return m_iNumCellColumns;}
	void     SetCached(bool bCached);
	bool     IsCached();
	RECT     GetInset(){return m_rcInset;};
	UINT     GetFlags();
	void     SetFlags(UINT uFlag);

	void    SetRightOrientation();
	//{{
protected:
	void    Init();
	void    Desotry();
	long    m_reflong;            //���ü���
	tstring m_strFilePath;       //�ļ���
	bool    m_bCached;

	BYTE*   m_pPixels;	//ͼ����
	HBITMAP m_hBitmap;	//ͼ����
	HICON   m_hIcon;
	int     m_nWidth, m_nHeight; //��ȸ�
	int     m_iCellHeight,  m_iCellWidth; //��Ԫ���ȸ߶�
	int     m_iNumCellRows, m_iNumCellColumns; //��������
	RECT    m_rcInset; //�Ź������ò���
	bool    m_bNeedAdjustColor; //�Ƿ�Ҫ��ɫ
	UINT	m_nFrameCount;
	UINT    m_uFlags;            //ͼƬ�����־
	UITYPE_IMAGE m_eImageType;
#ifdef _GDIPLUS_SUPPORT
	Bitmap  * m_pBitmap;
	PropertyItem*	m_pPropertyItem;
	IStream* m_pStream;
#endif
	//}}
};

