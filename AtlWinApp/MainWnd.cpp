#include "stdafx.h"
#include "MainWnd.h"
#include "resource.h"

#define	APP_NAME				_T("AtlWinApp")
#define APP_CLASS_NAME			_T("AtlWinAppClass")
#define APP_TITLE				_T("AtlWinApp")

CMainWnd::CMainWnd(void)
{
}

CMainWnd::~CMainWnd(void)
{
}

BOOL CMainWnd::Create()
{
	// atl 윈도우 생성
	CWndClassInfo &winInfo = GetWndClassInfo();
	winInfo.m_wc.lpszClassName = APP_CLASS_NAME;
	winInfo.m_wc.style = CS_DBLCLKS;
	winInfo.m_wc.hbrBackground = NULL;	
	winInfo.m_wc.hIcon = ::LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_MAIN));

	__super::Create(NULL, CRect(0,0,640,480),  APP_NAME, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	CenterWindow();

	// 보이기
	ShowWindow(SW_SHOW);
	DragAcceptFiles(TRUE);

	return TRUE;
}

LRESULT CMainWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PostQuitMessage(0);
	return 0;
}

BOOL CMainWnd::PreTranslateMessage(MSG* pMsg)
{
	return FALSE;
}
