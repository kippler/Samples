#include "stdafx.h"
#include "MainWnd.h"
#include "resource.h"
#include "Global.h"
#include "MyUtil.h"
#include "XStream.h"

#define	APP_NAME				_T("MyLittleSnippet")
#define APP_CLASS_NAME			_T("MyLittleSnippetClass")
#define APP_TITLE				_T("MyLittleSnippet")

#define TIMER_POLLING		1

CMainWnd::CMainWnd(void)
{
	m_cmd = 0;
}

CMainWnd::~CMainWnd(void)
{
}


BOOL CMainWnd::Create()
{
	// atl 윈도우 생성
	CWndClassInfo &winInfo = GetWndClassInfo();
	winInfo.m_wc.lpszClassName = APP_CLASS_NAME;
	winInfo.m_wc.style = 0;
	winInfo.m_wc.hbrBackground = NULL;	
	winInfo.m_wc.hIcon = ::LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_MAIN));
	m_hIcon = ::LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_MAIN));

	__super::Create(NULL, CRect(0,0,100,100),  APP_NAME, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	// 파일 로드
	m_str1 = _LoadFile(L"key1.txt");
	m_str2 = _LoadFile(L"key2.txt");


	// 보이기
	//ShowWindow(SW_SHOW);
	//DragAcceptFiles(TRUE);

	SetTrayIcon();
	RegisterHotkey();

	return TRUE;
}

LRESULT CMainWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Tray_NotifyDelete(m_hWnd, 0);
	PostQuitMessage(0);
	return 0;
}

BOOL CMainWnd::PreTranslateMessage(MSG* pMsg)
{
	return FALSE;
}



#define _CTRL ((HOTKEYF_CONTROL)<<8)
//#define CTRL_SHIFT	((HOTKEYF_SHIFT | HOTKEYF_CONTROL)<<8) 

void CMainWnd::RegisterHotkey()
{
	if (MyRegisterHotKey(m_hWnd, CMD_KEY1, _CTRL | '1') == FALSE)
		ASSERT(0);
	MyRegisterHotKey(m_hWnd, CMD_KEY2, _CTRL | '2');
}

LRESULT CMainWnd::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (wParam)
	{
	case CMD_EXIT :
		PostMessage(WM_CLOSE);
		break;
	default :
		ASSERT(0);
		break;
	}
	return 0;
}

LRESULT CMainWnd::OnHotkey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_cmd = wParam;
	SetTimer(TIMER_POLLING, 30, NULL);
	return 0;
}

CString CMainWnd::_LoadFile(CString fileName)
{
	CString  pathName = ::GetThisPath() + fileName;
	XFileReadStream file;

	if (file.Open(pathName) == FALSE) return FALSE;

	XBuffer buf;
	DWORD read;
	buf.Calloc((int)file.GetSize() + 2);
	file.Read(buf.data, (int)file.GetSize(), &read);

	// 문자열 가져와서
	CString str = (LPCWSTR)(buf.data + 2);		// BOM 건너뛰기
	return str;
}

BOOL CMainWnd::DoStr(CString str)
{
	// 대치하고
	str.Replace(L"[[date]]", GetTimeString(L"%c"));
	str.Replace(L"[[year]]", GetTimeString(L"%Y"));

	
	// 클립보드 복사
	CopyToClipboardText(str, m_hWnd);

	// 붙여넣기
	keybd_event(VK_CONTROL, 0, 0, 0);
	Sleep(10);

	keybd_event('V', 0, 0, 0);
	Sleep(10);
	keybd_event('V', 0, KEYEVENTF_KEYUP, 0);
	Sleep(10);

	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
	Sleep(10);

	return TRUE;
}

BOOL CMainWnd::DoCommand()
{
	//if (::GetAsyncKeyState(VK_CONTROL) & 0X8000) { return FALSE; }
	if (::GetAsyncKeyState(VK_SHIFT) & 0X8000) {  return FALSE; }
	if (::GetAsyncKeyState(VK_MENU) & 0X8000) { return FALSE; }
	if (::GetAsyncKeyState('1') & 0X8000) { return FALSE; }
	if (::GetAsyncKeyState('2') & 0X8000) { return FALSE; }
	if (::GetAsyncKeyState('3') & 0X8000) { return FALSE; }
	if (::GetAsyncKeyState('4') & 0X8000) { return FALSE; }

	// 눌린 키가 없으면
	switch (m_cmd)
	{
	case CMD_KEY1:
		DoStr(m_str1);
		break;

	case CMD_KEY2:
		DoStr(m_str2);
		break;

	default:
		ASSERT(0);
	}

	return TRUE;
}


LRESULT CMainWnd::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (wParam == TIMER_POLLING)
	{
		if(DoCommand())
			KillTimer(wParam);
	}

	return 0;
}


void CMainWnd::SetTrayIcon()
{
	Tray_NotifyDelete(m_hWnd, 0);
	Tray_NotifyAdd(m_hWnd, 0, m_hIcon, L"");
}

void CMainWnd::ModifyTrayIcon()
{
	Tray_NotifyModify(m_hWnd, 0, m_hIcon, L"");
}


LRESULT CMainWnd::OnTrayNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (lParam == WM_RBUTTONUP || lParam == WM_LBUTTONUP)
	{
		HMENU menu = ::LoadMenu(_Module.m_hInst, MAKEINTRESOURCE(IDR_TRAY));
		HMENU menuSub = ::GetSubMenu(menu, 0);


		CPoint pnt;
		::GetCursorPos(&pnt);
		SetForegroundWindow(m_hWnd);
		int ret = ::TrackPopupMenu(menuSub, TPM_RIGHTBUTTON | TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RETURNCMD, pnt.x, pnt.y, 0, m_hWnd, NULL);
		DestroyMenu(menu);

		if (ret>0)
			PostMessage(WM_COMMAND, ret);
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         탐색기 재시작시 트레이 아이콘 등록하기 
/// @param  
/// @return 
/// @date   Wed Mar  2 14:54:35 2016
////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CMainWnd::OnTaskbarCreated(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SetTrayIcon();
	return 0;
}

