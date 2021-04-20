#include "stdafx.h"
#include "AnimationEffectWnd.h"

//////////////////////////////////////////////////////////////////////////
// CTurnObjectWnd
CTurnObjectWnd::CTurnObjectWnd( HBITMAP hBitmap, RECT rect, CWindowUI *pWindow )
:m_hBmpSideA(hBitmap)
{
	m_pWindow = pWindow;

	m_iAnimationIndex = 0;
	m_rcWindow = rect;
	m_rcWindow.top = m_rcWindow.top - 100;
	m_rcWindow.bottom = m_rcWindow.bottom + 100;
	m_iWindowWidth = m_rcWindow.right - m_rcWindow.left;
	m_iWindowHeight = m_rcWindow.bottom - m_rcWindow.top;

	Create( NULL, _T("CTurnObjectWnd"), m_rcWindow.left, m_rcWindow.top, m_iWindowWidth, m_iWindowHeight);

}

CTurnObjectWnd::~CTurnObjectWnd()
{

}

void CTurnObjectWnd::Play( )
{
	HPALETTE hPal = ( HPALETTE )GetStockObject( DEFAULT_PALETTE );
	m_pBmpSideA = Bitmap::FromHBITMAP( m_hBmpSideA, hPal);

	UpdateAnimation();
	SetTimer( 1, 50, NULL );
	ShowWindow( SW_SHOW );

}

void CTurnObjectWnd::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == 1 )
	{
		m_iAnimationIndex++;
		UpdateAnimation();
	}
	else if ( nIDEvent == 2 )
	{
		m_pWindow->ShowWindow( SW_SHOW );
		//ShowWindow( SW_HIDE );
		KillTimer( 2 );
		//CloseWindow( );
	}

}

void CTurnObjectWnd::UpdateAnimation( int iAlpha /* = 255 */ )
{
	int iImageHeight = m_pBmpSideA->GetHeight();
	int iImageWidth = m_pBmpSideA->GetWidth();

	int iDisX = (int)iImageWidth / 10;
	int iBaseY = 100;
	int iDisY = 10;

	HDC hDC = ::GetDC( m_hWnd );
	m_hMemoryDC = CreateCompatibleDC( hDC );
	HBITMAP hBitMap = CreateCompatibleBitmap( hDC, m_iWindowWidth, m_iWindowHeight );
	SelectObject( m_hMemoryDC, hBitMap );
	Graphics graphics( m_hMemoryDC );
	graphics.SetSmoothingMode( SmoothingModeAntiAlias );
	graphics.SetInterpolationMode( InterpolationModeHighQualityBicubic );

	int iTemp = 0;
	if ( m_iAnimationIndex == 0 )
	{
		iTemp = iBaseY;
		Point newpoints1[] = {Point(0, iTemp), Point(iImageWidth, iTemp), Point(0, iImageHeight+iTemp)};
		graphics.DrawImage(m_pBmpSideA, newpoints1, 3);
	}
	else if ( m_iAnimationIndex == 1 )
	{
		iTemp = iBaseY+iDisY;
		Point newpoints1[] = {Point(iDisX, iTemp), Point(iImageWidth-iDisX, 200-iTemp), Point(iDisX, iImageHeight+iTemp)};
		graphics.DrawImage(m_pBmpSideA, newpoints1, 3);
	}
	else if ( m_iAnimationIndex == 2 )
	{
		iTemp = iBaseY+iDisY*2;
		Point newpoints2[] = {Point(iDisX*2, iTemp), Point(iImageWidth-iDisX*2, 200-iTemp), Point(iDisX*2, iImageHeight+iTemp)};
		graphics.DrawImage(m_pBmpSideA, newpoints2, 3);
	}
	else if ( m_iAnimationIndex == 3 )
	{
		iTemp = iBaseY+iDisY*3;
		Point newpoints3[] = {Point(iDisX*4, iTemp), Point(iImageWidth-iDisX*4, 200-iTemp), Point(iDisX*4, iImageHeight+iTemp)};
		graphics.DrawImage(m_pBmpSideA, newpoints3, 3);
	}
	else if ( m_iAnimationIndex == 4 )//center
	{
		iTemp = iBaseY-iDisY*4;
		Point newpoints4[] = {Point(iDisX*4, iTemp), Point(iImageWidth-iDisX*4, 200-iTemp), Point(iDisX*4, iImageHeight+iTemp)};
		graphics.DrawImage(m_pBmpSideA, newpoints4, 3);
	}
	else if ( m_iAnimationIndex == 5 )
	{
		iTemp = iBaseY-iDisY*3;
		Point newpoints5[] = {Point(iDisX*3, iTemp), Point(iImageWidth-iDisX*3, 200-iTemp), Point(iDisX*3, iImageHeight+iTemp)};
		graphics.DrawImage(m_pBmpSideA, newpoints5, 3);
	}
	else if ( m_iAnimationIndex == 6 )
	{
		iTemp = iBaseY-iDisY*2;
		Point newpoints6[] = {Point(iDisX*2, iTemp), Point(iImageWidth-iDisX*2, 200-iTemp), Point(iDisX*2, iImageHeight+iTemp)};
		graphics.DrawImage(m_pBmpSideA, newpoints6, 3);
	}
	else if ( m_iAnimationIndex == 7 )
	{
		iTemp = iBaseY-iDisY;
		Point newpoints7[] = {Point(iDisX, iTemp), Point(iImageWidth-iDisX, 200-iTemp), Point(iDisX, iImageHeight+iTemp)};
		graphics.DrawImage(m_pBmpSideA, newpoints7, 3);
	}
	else if ( m_iAnimationIndex == 8 )
	{
		iTemp = iBaseY;
		Point newpoints1[] = {Point(0, iTemp), Point(iImageWidth, iTemp), Point(0, iImageHeight+iTemp)};
		graphics.DrawImage(m_pBmpSideA, newpoints1, 3);
		m_BlendFun.SourceConstantAlpha=255;

		KillTimer( 1 );
		SetTimer( 2, 1, NULL );
	}

	HDC hScreenDC = ::GetDC( m_hWnd );
	POINT ptSrc = { 0, 0};
	POINT ptWinPos = { m_rcWindow.left, m_rcWindow.top };
	SIZE sizeWindow ={ m_iWindowWidth, m_iWindowHeight };
	DWORD dwExStyle = GetWindowLong( m_hWnd, GWL_EXSTYLE );
	if ( (dwExStyle&0x80000) != 0x80000 )
	{
		SetWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle^0x80000 );
	}
	UpdateLayeredWindow( m_hWnd, hScreenDC, &ptWinPos, &sizeWindow, m_hMemoryDC, &ptSrc, 0, &m_BlendFun, 2 );

	::ReleaseDC( m_hWnd, hScreenDC );
	hScreenDC = NULL;
	DeleteObject( hBitMap );
	DeleteDC( m_hMemoryDC );
	m_hMemoryDC = NULL;
}

//////////////////////////////////////////////////////////////////////////
// CPosChangeWnd
CPosChangeWnd::CPosChangeWnd( HBITMAP hBitmap, RECT rect )
      :m_hBmpSideA(hBitmap)
{
	m_iAnimationIndex = 0;
	m_rcWindow.left = rect.left - 50;
	m_rcWindow.right = rect.right + 50;
	m_rcWindow.top = rect.top - 50;
	m_rcWindow.bottom = rect.bottom + 50;
	m_iWindowWidth = m_rcWindow.right - m_rcWindow.left;
	m_iWindowHeight = m_rcWindow.bottom - m_rcWindow.top;

	Create( NULL, _T("CTurnObjectWnd"), m_rcWindow.left, m_rcWindow.top, m_iWindowWidth, m_iWindowHeight);
}

CPosChangeWnd::~CPosChangeWnd()
{

}

void CPosChangeWnd::Play()
{
	HPALETTE hPal = ( HPALETTE )GetStockObject( DEFAULT_PALETTE );
	m_pBmpSideA = Bitmap::FromHBITMAP( m_hBmpSideA, hPal);

	m_BlendFun.SourceConstantAlpha=225;
	UpdateAnimation();
	SetTimer( 1, 50, NULL );
	ShowWindow( SW_SHOW );
}

void CPosChangeWnd::OnTimer(UINT_PTR nIDEvent)
{
	m_iAnimationIndex++;
	UpdateAnimation();
}

void CPosChangeWnd::UpdateAnimation( int iAlpha /* = 255 */ )
{
	int iImageHeight = m_pBmpSideA->GetHeight();
	int iImageWidth = m_pBmpSideA->GetWidth();

	//int iDisX = (int)iImageWidth / 10;
	//int iBaseY = 100;
	//int iDisY = 10;

	HDC hDC = ::GetDC( m_hWnd );
	m_hMemoryDC = CreateCompatibleDC( hDC );
	HBITMAP hBitMap = CreateCompatibleBitmap( hDC, m_iWindowWidth, m_iWindowHeight );
	SelectObject( m_hMemoryDC, hBitMap );
	Graphics graphics( m_hMemoryDC );
	graphics.SetSmoothingMode( SmoothingModeAntiAlias );
	graphics.SetInterpolationMode( InterpolationModeHighQualityBicubic );

	//////////////////////////////////////////////////////////////////////////
	//int iTemp = 0;
	if ( m_iAnimationIndex == 0 )
	{
		Rect rect(50,50,iImageWidth,iImageHeight);
		graphics.DrawImage(m_pBmpSideA, rect);
	}
	else if ( m_iAnimationIndex == 1 )
	{
		Rect rect(49,49,iImageWidth+2,iImageHeight+2);
		graphics.DrawImage(m_pBmpSideA, rect);
	}
	else if ( m_iAnimationIndex == 2 )
	{
		Rect rect(48,48,iImageWidth+4,iImageHeight+4);
		graphics.DrawImage(m_pBmpSideA, rect);
	}
	else if ( m_iAnimationIndex == 3 )
	{
		Rect rect(47,47,iImageWidth+6,iImageHeight+6);
		graphics.DrawImage(m_pBmpSideA, rect);
	}
	else if ( m_iAnimationIndex == 4 )
	{
		Rect rect(48,48,iImageWidth+4,iImageHeight+4);
		graphics.DrawImage(m_pBmpSideA, rect);
	}
	else if ( m_iAnimationIndex == 5 )
	{
		Rect rect(49,49,iImageWidth+2,iImageHeight+2);
		graphics.DrawImage(m_pBmpSideA, rect);
	}
	else if ( m_iAnimationIndex == 6 )//center
	{
		Rect rect(50,50,iImageWidth,iImageHeight);
		graphics.DrawImage(m_pBmpSideA, rect);

		m_BlendFun.SourceConstantAlpha=255;
		KillTimer( 1 );
	}
	//////////////////////////////////////////////////////////////////////////
	HDC hScreenDC = ::GetDC( m_hWnd );
	POINT ptSrc = { 0, 0};
	POINT ptWinPos = { m_rcWindow.left, m_rcWindow.top };
	SIZE sizeWindow ={ m_iWindowWidth, m_iWindowHeight };
	DWORD dwExStyle = GetWindowLong( m_hWnd, GWL_EXSTYLE );
	if ( (dwExStyle&0x80000) != 0x80000 )
	{
		SetWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle^0x80000 );
	}
	UpdateLayeredWindow( m_hWnd, hScreenDC, &ptWinPos, &sizeWindow, m_hMemoryDC, &ptSrc, 0, &m_BlendFun, 2 );

	::ReleaseDC( m_hWnd, hScreenDC );
	hScreenDC = NULL;
	DeleteObject( hBitMap );
	DeleteDC( m_hMemoryDC );
	m_hMemoryDC = NULL;
}