#include "stdafx.h"
#include "CommonUI\WindowPtrRef.h"
#include "Event\WebPreviewEvent.hpp"
#include "CommonUI\UIWebPreviewInterface.h"

#include "CommonUI\UIWebPreviewWnd.h"
#include "Interface/CefUILayout.h"
#include "CommonUI\UIAnimationCtrl.h"
#include "CommonUI\UIRotateAnimation.h"
#include "Utils\WmUserMessage.h"
#include "rapidjson\rapidjson.h"
#include "rapidjson\document.h"
#include "Data/rapidjson_stream.hpp"
#include "Data/rapidjson_wrapper.hpp" 
#include "Utils\StringOperation.h"
#include "CommonUI\WebPreviewBiz.h"
#include "Event\CommonUIEvent.hpp"
#include "Event\StatisticsEventModel.hpp"
#include "Event\BizProxyHandleModel.hpp"
#include "BizProxy\BizProxyHandler.h"


using namespace rapidjson;

CWebPreviewWndUI::CWebPreviewWndUI()
{
	m_pWebControl = NULL;
	m_pLayLoading = NULL;
	m_pLoading    = NULL;

	m_pBtnBackOff = NULL;
	m_pBtnForward = NULL;	

	m_pWebPreviewBiz = NULL;
	m_bShowMask = false;
}

CWebPreviewWndUI::~CWebPreviewWndUI()
{
	DestroyWebBiz();
	if (m_pEvent)
	{
		m_pEvent->Post();
		delete m_pEvent;
	}
}

HWND CWebPreviewWndUI::CreateWebPreviewWnd(WebPreviewInfo* previewInfo, bool bShowMask)
{
	BizProxy::CBizProxyHandler::GetInstance();
	HWND hWnd = NULL;
	m_bShowMask = bShowMask;
	do 
	{
		if (!previewInfo)
		{
			break;
		}

		m_strWndId = previewInfo->wstrWndId;
		if (m_strWndId.empty())
		{
			m_strWndId = _T("WebPreviewWnd"); //默认窗口样式
		}

		
		RequestProductControlImageId reqBkId;
		reqBkId.strDefaultIconId = _T("#icon_logo");
		reqBkId.Send();
		SetAttribute(_T("icon"),	reqBkId.strProductIconId.c_str());
		SetAttribute(_T("bigicon"), reqBkId.strProductIconId.c_str());

		if (bShowMask)
		{
			RequestMaskWindow reqMaskWnd; //蒙版
			reqMaskWnd.bShow = true;
			reqMaskWnd.hAttachWnd = previewInfo->hCreateParentWnd;
			reqMaskWnd.Send();

			if (reqMaskWnd.hMaskWnd)
			{
				hWnd = Create(reqMaskWnd.hMaskWnd, m_strWndId.c_str());
				CenterWindow(reqMaskWnd.hMaskWnd);
			}
			else
			{
				hWnd = Create(previewInfo->hCreateParentWnd, m_strWndId.c_str());
			}
		}
		else
		{
			hWnd = Create(previewInfo->hCreateParentWnd, m_strWndId.c_str());
		}

		m_strUrl = previewInfo->wstrUrl;
		m_fHeightPercent = previewInfo->fHeightPercent;
		m_fWidthPercent = previewInfo->fWidthPercent;
		m_fConstraintSspectRatio = previewInfo->fConstraintSspectRatio;

		SetPreviewTitle(previewInfo->wstrPreviewTitle.c_str());
		
		UpdateWndPos(previewInfo->hParentWnd);

	} while (false);	

	return hWnd;
}

void CWebPreviewWndUI::SetParentWnd(HWND hParentWnd)
{
// 	if (!GetHWND()
// 		|| !::IsWindow(GetHWND()))
// 	{
// 		return;
// 	}
// 
// 	HWND hOldParentWnd = ::GetParent(GetHWND());
// 	if (hOldParentWnd != hParentWnd)
// 	{
// 		::SetWindowLongPtr(GetHWND(), GWLP_HWNDPARENT, (LONG_PTR)hParentWnd);
// 	}

	UpdateWndPos(hParentWnd);
}

void CWebPreviewWndUI::ShowWebPreviewWnd()
{
	if (!GetHWND()
		|| !::IsWindow(GetHWND()))
	{
		return;
	}

	if (::IsIconic(GetHWND()))
	{
		RestoreWindow();
	}
	else
	{
		ShowWindow();
	}
}

void CWebPreviewWndUI::ReCreate(RequestPreviewWebWnd* pEvent)
{
	CloseWebPreview();

	m_pEvent = (RequestPreviewWebWnd*)((IEvent*)pEvent)->Copy();
}

void CWebPreviewWndUI::SetWebBiz(CWebPreviewBiz* pBiz)
{
	m_pWebPreviewBiz = pBiz;	
}


void CWebPreviewWndUI::InitWebBiz()
{
	if (!m_pWebPreviewBiz)
		return;

	m_pWebPreviewBiz->InitWebWindow(this);	
}

void CWebPreviewWndUI::DestroyWebBiz()
{
	if (m_pWebPreviewBiz)
	{
		m_pWebPreviewBiz->DestroyWebWindow();

		delete m_pWebPreviewBiz;
		m_pWebPreviewBiz = NULL;
	}
}

LPCTSTR CWebPreviewWndUI::GetLoadedUrl()
{
	return m_strLoadedUrl.c_str();
}

void CWebPreviewWndUI::ReloadCefUrl(LPCTSTR lptcsUrl)
{
	m_strUrl = lptcsUrl;
	ShowCefWnd(m_strUrl);

	SetLoadingVisible(true);
}

HWND CWebPreviewWndUI::GetWebCefWnd()
{
	if (m_pWebControl)
	{
	   return m_pWebControl->GetWebWindowHwnd();
	}

	return NULL;
}

HWND CWebPreviewWndUI::GetPreviewHwnd()
{
	if (::IsWindow(GetHWND()))
	{
		return GetHWND();
	}

	return NULL;
}

void CWebPreviewWndUI::SetReceiveStatusChange(bool bChange)
{
	if (m_pWebControl)
	{
		m_pWebControl->SetReceiveStatusChange(bChange);
	}
}

void CWebPreviewWndUI::ShowCefUrl(const wstring& wstrUrl,
								int nType,
								int nQuestionType /*= 0*/, 
								bool bFileReq /*= false*/,
								bool bCefHide /*= false*/)
{
	if (m_pWebControl)
	{
		m_pWebControl->ShowCefWnd(wstrUrl, bFileReq);
		m_pWebControl->SetPopupType(cefui::CefPopType_MsgNotify);
		m_pWebControl->SetCefWndVisible(bCefHide);
	}

	if (m_pWebPreviewBiz)
	{
		int nType = m_pWebPreviewBiz->GetLinkPopupType();
		if (nType != -1)
		{
			if (m_pWebControl)
			{
				m_pWebControl->SetPopupType(nType);
			}
			
		}
	}
}

void CWebPreviewWndUI::OnWindowInit()
{
	InitWebBiz();

	if (!m_strUrl.empty())
	{
		ShowCefWnd(m_strUrl);
	}	
	
	SetLoadingVisible(true);

}

void CWebPreviewWndUI::OnClose()
{
	if (m_bShowMask)
	{
		RequestMaskWindow reqMaskWnd; //蒙版
		reqMaskWnd.bShow = false;
		reqMaskWnd.hAttachWnd = GetParent(GetParent(GetHWND()));
		reqMaskWnd.Send();
	}
}

void CWebPreviewWndUI::OnCreate()
{
	__super::OnCreate();

	m_pWebControl = dynamic_cast<CCefUILayout*>(FindControl(_T("homework_cefwnd")));
	m_pLayLoading = dynamic_cast<CLayoutUI*>(FindControl(_T("lay_loading")));
	m_pLoading    = dynamic_cast<CRotateAnimationUI*>(FindControl(_T("ani_loading")));
	
	m_pBtnBackOff = dynamic_cast<CButtonUI*> (FindControl(_T("btn_backoff")));
	m_pBtnForward = dynamic_cast<CButtonUI*> (FindControl(_T("btn_forward")));	
}

LRESULT CWebPreviewWndUI::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
	 	if (wParam == VK_ESCAPE)
	 	{
	 		CloseWebPreview();
	 		return TRUE;
	 	}
	 	break;
	case WM_SYSKEYDOWN:
	 	if (wParam == VK_F4)
	 	{
	 		CloseWebPreview();
	 		return TRUE;
	 	}
	 	break;
	case WM_COPYDATA:
		{
	 		COPYDATASTRUCT* pCopyDataStruct = (COPYDATASTRUCT*)lParam;
	 		if (!pCopyDataStruct)
	 		{
	 			break;
	 		}
	 
	 		switch (pCopyDataStruct->dwData)
	 		{
	 		case MSG_CEF_PLAYERLOADED:
	 			{
	 				return FALSE;
	 			}
	 			break;
	 		case MSG_CEF_INFO:
	 		case MSG_CEF_ICRINVOKENATIVE:
	 		case MSG_CEF_INVOKEMETHODPPT:
	 			{
					if (pCopyDataStruct->lpData
						&& pCopyDataStruct->cbData < 0x100000)
					{
						string jsonInfo((const char*)pCopyDataStruct->lpData, pCopyDataStruct->cbData);
						OnCefInvokeMethodPpt(jsonInfo.c_str());
					}	 
	 			}
	 			break;
	 		case MSG_CEF_INVOKE_ASYNC://窗口收到了含有CEF异步回调的请求 处理完成后调用回调
	 			{
	 				if (pCopyDataStruct->lpData
	 					&& pCopyDataStruct->cbData < 0x100000)
	 				{
	 					string strJson((LPCSTR)pCopyDataStruct->lpData, (int)pCopyDataStruct->cbData);
						OnCefInvokeAsync(strJson.c_str());
	 				}
	 			}
	 			break;
	 		case MSG_CEF_INVOKE://窗口收到了含有CEF的invoke的请求
	 			{
	 				if (pCopyDataStruct->lpData
	 					&& pCopyDataStruct->cbData < 0x100000)
	 				{
	 					string strJson((LPCSTR)pCopyDataStruct->lpData, (int)pCopyDataStruct->cbData);
						OnCefInvoke(strJson.c_str());
	 				}
	 			}
	 			break;
			case MSG_CEF_NATIVE_INVOKE:
			case MSG_CEF_NATIVE_LISTEN:
			case MSG_CEF_NATIVE_REMOVE_LISTEN:
			{
				BizProxy::RequestBizProxyHandle requestHandle;
				requestHandle.nType = pCopyDataStruct->dwData;
				requestHandle.strData = std::string((LPCSTR)pCopyDataStruct->lpData, (int)pCopyDataStruct->cbData);
				requestHandle.Post();
				break;
			}
	 		case MSG_CEF_ESC:
	 			break;
	 		case MSG_CEF_LOADED:
	 			{	 			
					OnCefLoaded();

					char * pszUrl = new_nothrow char[pCopyDataStruct->cbData + 1];
					pszUrl[pCopyDataStruct->cbData] = 0;
					memcpy(pszUrl, (const char *)pCopyDataStruct->lpData, pCopyDataStruct->cbData);
					tstring strUrl = Utf82Str(pszUrl);					
					delete []pszUrl;

					m_strLoadedUrl = strUrl;
	 			}
	 			break;
	 		case MSG_CEF_URLPOPUP:
	 			if (pCopyDataStruct->lpData
	 				&& pCopyDataStruct->cbData < 0x100000)
	 			{
					Document doc;
					doc.Parse((char*)pCopyDataStruct->lpData, pCopyDataStruct->cbData);
					if (!doc.HasParseError())
					{
						string strEventData = rjwrapper::GetStringByKey(doc, "eventData");
						tstring strUrl = Utf82Str(strEventData);
						OnCefUrlPopUp(strUrl.c_str());
					}
	 			}
	 			break;
	 		case MSG_CEF_URLCHANGE:
	 			{
	 				char * pszUrl = new_nothrow char[pCopyDataStruct->cbData + 1];
	 				pszUrl[pCopyDataStruct->cbData] = 0;
	 				memcpy(pszUrl, (const char *)pCopyDataStruct->lpData, pCopyDataStruct->cbData);
					std::wstring wstrUrl = Utf82Str(pszUrl);	 		
	 				delete []pszUrl;
	 
					OnCefUrlChange(wstrUrl.c_str());
	 			}
	 			break;
	 		case MSG_CEF_URLFAILED:
	 			{
	 
	 				char * pszUrl = new char[pCopyDataStruct->cbData + 1];
	 				pszUrl[pCopyDataStruct->cbData] = 0;
	 				memcpy(pszUrl, (const char *)pCopyDataStruct->lpData, pCopyDataStruct->cbData);
					std::wstring wstrUrl = Utf82Str(pszUrl);
	 				delete []pszUrl;
	 
					OnCefUrlFailed(wstrUrl.c_str());	 		
	 			}
	 			break;
			case MSG_CEF_STATECHANGE:
				{
					Document doc;
					doc.Parse((char*)pCopyDataStruct->lpData, pCopyDataStruct->cbData);
					if (!doc.HasParseError())
					{
						string strPath  = rjwrapper::GetStringByKey(doc, "path");
						tstring wstrMsg = Utf82Str(strPath);						
						//0 网页非加载中 1 网页加载中
						bool bLoading = !!rjwrapper::GetIntByKey(doc, "isLoading");
						bool bCanGoBack = !!rjwrapper::GetIntByKey(doc, "canGoBack");
						bool bCanGoForward = !!rjwrapper::GetIntByKey(doc, "canGoForward");
						string strURL = rjwrapper::GetStringByKey(doc, "url");

						UpdateDirection(bCanGoBack, bCanGoForward);
						
						m_strLoadedUrl = Utf82Str(strURL);
					}
				}
				break;
	 		} //end switch (pCopyDataStruct->dwData)		
		}//end case WM_COPYDATA
		break;
	}

	return __super::WindowProc(message, wParam, lParam);
}


bool CWebPreviewWndUI::OnBtnClose(TNotifyUI* msg)
{
	if (0 == wcscmp(_T("WebStuLinkWnd"), m_strWndId.c_str()))
	{
		RequestSmartClassroomInfo req;
		time(&req.reqTimeInfo.tBeginTime);
		req.eOperationType = SmartClassroomOperation_Close;
		req.PostInWorkPool();
	}
	CloseWebPreview();

	return true;
}

bool CWebPreviewWndUI::OnBtnMini(TNotifyUI* msg)
{	
	if (m_pWebPreviewBiz)
	{
		m_pWebPreviewBiz->MiniWnd();
	}

	MinimizeWindow();

	return true;
}

bool CWebPreviewWndUI::OnBtnBackOff(TNotifyUI* msg)
{
	if (m_pWebPreviewBiz)
	{
		m_pWebPreviewBiz->BackOffCefWnd();
	}	

	return true;
}

bool CWebPreviewWndUI::OnBtnForward(TNotifyUI* msg)
{
	if (m_pWebPreviewBiz)
	{
		m_pWebPreviewBiz->ForwardCefWnd();
	}	

	return true;
}

bool CWebPreviewWndUI::OnBtnRefresh(TNotifyUI* msg)
{
	if (m_pWebPreviewBiz)
	{
		m_pWebPreviewBiz->RefreshCefWnd();
	}

	return true;
}

void CWebPreviewWndUI::SetPreviewTitle(LPCTSTR lpctTitle)
{
	if (!lpctTitle)
	{
		return;
	}

	CControlUI* pTitle = FindControl(_T("lay_title"));

	if (pTitle)
	{
		pTitle->SetAttribute(_T("text"), lpctTitle);

		if (pTitle->GetParent())
		{
			pTitle->GetParent()->OnlyResizeChild();
			pTitle->GetParent()->Invalidate();
		}		
	}
	
	this->SetTitle(I18NSTR(lpctTitle));
}

void CWebPreviewWndUI::SetLoadingVisible(bool bVisible)
{
	if (m_pLayLoading)
	{
		m_pLayLoading->SetVisible(bVisible);
		if (m_pLoading)
		{
			m_pLoading->SetAnimatable(bVisible);
		}
	}

	if (m_pWebControl)
	{
		m_pWebControl->SetCefWndVisible(!bVisible);
	}
}

void CWebPreviewWndUI::UpdateDirection(bool bCanGoBack, bool bCanGoForward)
{
	if (m_pBtnBackOff)
	{
		m_pBtnBackOff->SetEnabled(bCanGoBack);
	}

	if (m_pBtnForward)
	{
		m_pBtnForward->SetEnabled(bCanGoForward);
	}
}

bool CWebPreviewWndUI::CanGoBackCefWnd()
{
	bool bRet = false;

	if (m_pWebControl)
	{
		bRet = m_pWebControl->WndCanGoBack();
	}

	return bRet;
}

bool CWebPreviewWndUI::CanGoForwardCefWnd()
{
	bool bRet = false;

	if (m_pWebControl)
	{
		bRet = m_pWebControl->WndCanGoForward();
	}

	return bRet;
}

CControlUI* CWebPreviewWndUI::FindWndControl(LPCTSTR lptcsId)
{
	return this->FindControl(lptcsId);
}

void CWebPreviewWndUI::AddWndNotifier(INotifyUI* pNotifier)
{
	this->AddNotifier(pNotifier);
}

void CWebPreviewWndUI::RemoveWndNotifier(INotifyUI* pNotifier)
{
	this->RemoveNotifier(pNotifier);
}

void CWebPreviewWndUI::CloseWebPreview()
{
	ExitCefWnd(); //退出网页，关闭cef窗口

	CloseWindow();
}

void CWebPreviewWndUI::ShowCefWnd(std::wstring& wstrUrl)
{
	SetLoadingVisible(false); 
	if (m_pWebControl)
	{
		m_pWebControl->ShowCefWnd(wstrUrl, 0);
	}

	if (m_pWebPreviewBiz)
	{
		int nType = m_pWebPreviewBiz->GetLinkPopupType();
		if (nType != -1)
		{
			m_pWebControl->SetPopupType(nType);
		}
	}
}

void CWebPreviewWndUI::ExitCefWnd()
{
	if (m_pWebControl)
	{
		m_pWebControl->CloseCefWnd();
	}
}

void CWebPreviewWndUI::BackOffCefWnd()
{
	if (m_pWebControl)
	{
		m_pWebControl->WndGoBack();
	}
}

void CWebPreviewWndUI::ForwardCefWnd()
{
	if (m_pWebControl)
	{
		m_pWebControl->WndGoForward();
	}
}

void CWebPreviewWndUI::ReloadCefWnd()
{
	if (m_pWebControl)
	{	
		m_pWebControl->WndReload();
	}
}

void CWebPreviewWndUI::MiniPreviewWnd()
{
	MinimizeWindow();
}

void CWebPreviewWndUI::DoExecJs(std::wstring& wstrJs)
{
	if (m_pWebControl)
	{
		m_pWebControl->ExecJs(wstrJs.c_str());
	}
}

void CWebPreviewWndUI::OnCefInvokeMethodPpt(LPCSTR pszJsonInfo)
{
	if (m_pWebPreviewBiz)
	{
		m_pWebPreviewBiz->OnCefInvokeMethodPpt(pszJsonInfo);
	}
}

void CWebPreviewWndUI::OnCefInvokeAsync(LPCSTR pszJsonInfo)
{
	if (m_pWebPreviewBiz)
	{
		m_pWebPreviewBiz->OnCefInvokeAsync(pszJsonInfo);
	}
}

void CWebPreviewWndUI::OnCefInvoke(LPCSTR pszJsonInfo)
{
	if (m_pWebPreviewBiz)
	{
		m_pWebPreviewBiz->OnCefInvoke(pszJsonInfo);
	}
}

void CWebPreviewWndUI::OnCefLoaded()
{
	SetLoadingVisible(false);

	if (m_pWebPreviewBiz)
	{
		m_pWebPreviewBiz->OnCefLoaded();
	}
}

void CWebPreviewWndUI::OnCefUrlPopUp(LPCTSTR lptcsUrl)
{
	if (m_pWebPreviewBiz)
	{
		m_pWebPreviewBiz->OnCefUrlPopUp(lptcsUrl);
	}
}

void CWebPreviewWndUI::OnCefUrlChange(LPCTSTR lptcsUrl)
{
	if (m_pWebPreviewBiz)
	{
		m_pWebPreviewBiz->OnCefUrlChange(lptcsUrl);
	}
}

void CWebPreviewWndUI::OnCefUrlFailed(LPCTSTR lptcsUrl)
{
	if (m_pWebPreviewBiz)
	{
		bool bRet = m_pWebPreviewBiz->isRetryErrorUrl();
		wstring strBuff = m_pWebPreviewBiz->OnCefRetryErrorUrl();
		if (strBuff.empty())
		{
			strBuff = lptcsUrl;
		}

		if (m_pWebControl)
		{
			m_pWebControl->LoadErrorUrl(strBuff.c_str(), bRet);
		}
	}
}

void CWebPreviewWndUI::UpdateWndPos(HWND hParent)
{
	if (m_pWebPreviewBiz)
	{

		m_pWebPreviewBiz->UpdateWindowPos(this, hParent, m_fWidthPercent, m_fHeightPercent, m_fConstraintSspectRatio);
	}
	else
	{
	if (!hParent
		|| !::IsWindow(hParent))
	{
		return;
	}
	RECT rt = { 0 };
	if (::IsIconic(hParent))
	{
		MONITORINFO oMonitor = { 0 };
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(hParent, MONITOR_DEFAULTTONEAREST), &oMonitor);
		rt = oMonitor.rcMonitor;
	}
	else
	{
		::GetWindowRect(hParent, &rt);
		}		

		if (m_fWidthPercent > 0.0001 && m_fHeightPercent > 0.0001)
		{
	int nWndWidth = (rt.right - rt.left);
	int nWndHeight = rt.bottom - rt.top;
			int nPosX = (int)(rt.left + nWndWidth * (1 - m_fWidthPercent) * 0.5);
			int nPosY = (int)(rt.top + nWndHeight * (1 - m_fHeightPercent) * 0.5);
			nWndHeight = (int)(nWndHeight * m_fHeightPercent);
			nWndWidth = (int)(nWndWidth * m_fWidthPercent);

	::SetWindowPos(GetHWND(), HWND_TOP, nPosX, nPosY, nWndWidth, nWndHeight, SWP_SHOWWINDOW);
		}
		else if (m_fWidthPercent > 0.0001)
		{
			//使用 宽约束高
			int nWndWidth = (rt.right - rt.left);
			int nPosX = (int)(rt.left + nWndWidth * (1 - m_fWidthPercent) * 0.5);
			nWndWidth = (int)(nWndWidth * m_fWidthPercent);
			int nWndHeight = (int)(nWndWidth * m_fConstraintSspectRatio);
			int nPosY = (int)(rt.top + (rt.bottom - rt.top - nWndHeight) * 0.5);
			
			::SetWindowPos(GetHWND(), HWND_TOP, nPosX, nPosY, nWndWidth, nWndHeight, SWP_SHOWWINDOW);
		}
		else if (m_fHeightPercent > 0.0001)
		{
			//使用 高约束宽
			int nWndHeight = rt.bottom - rt.top;
			int nPosY = (int)(rt.top + nWndHeight * (1 - m_fHeightPercent) * 0.5);
			nWndHeight = (int)(nWndHeight * m_fHeightPercent);
			int nWndWidth = (int)(nWndHeight * m_fConstraintSspectRatio);
			int nPosX = (int)(rt.left + (rt.right - rt.left - nWndWidth) * 0.5);

			::SetWindowPos(GetHWND(), HWND_TOP, nPosX, nPosY, nWndWidth, nWndHeight, SWP_SHOWWINDOW);
		}
		
	}
}

void CWebPreviewWndUI::SetErrorLanguage(bool bLocal)
{
	if (m_pWebControl)
	{
		m_pWebControl->SetErrorLanguage(bLocal);
	}
}

