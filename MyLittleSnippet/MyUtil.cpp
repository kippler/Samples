#include "stdafx.h"
#include "MyUtil.h"

#define LEN_TIME_BUF 128
CString	GetTimeString(LPCTSTR sFormat)
{
	TCHAR	buf[LEN_TIME_BUF];
	time_t ltime = time(NULL);
	tm now;
	_localtime64_s(&now, &ltime);

	::_tcsftime(buf, LEN_TIME_BUF, sFormat, &now);
	return buf;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///          이 프로세스의 실행 파일이 위치하는 경로
/// @return  
/// @date    2008-09-23 15:37:24
////////////////////////////////////////////////////////////////////////////////////////////////////
CString  GetThisPath(HMODULE hModule)
{
	static TCHAR szModuleName[MAX_PATH * 4] = { 0, };
	static BOOL bIsFirst = TRUE;
	if (bIsFirst)
	{
		::GetModuleFileName(hModule, szModuleName, MAX_PATH * 4);
		*(_tcsrchr(szModuleName, '\\') + 1) = NULL;
		bIsFirst = FALSE;
	}
	return CString(szModuleName);
}



UINT	WM_TRAY_NOTIFY = ::RegisterWindowMessage(_T("WM_TRAY_NOTIFY"));
#define TRAY_INFO_TIMEOUT	5000	// 5초

static BOOL TrayMessage(HWND hWnd, DWORD dwMessage, UINT uID, HICON hIcon, LPCTSTR lpszTip,
	LPCTSTR lpszInfoTitle, LPCTSTR lpszInfo)
{
	BOOL res;
	NOTIFYICONDATA tnd;

	tnd.cbSize = sizeof(NOTIFYICONDATA);
	tnd.hWnd = hWnd;
	tnd.uID = uID;
	tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnd.uCallbackMessage = WM_TRAY_NOTIFY;
	tnd.hIcon = hIcon;

	if (lpszTip)
	{
		int nSize = min((int)(sizeof(tnd.szTip) / sizeof(tnd.szTip[0])), lstrlen(lpszTip) + 1);
		if (lstrcpyn(tnd.szTip, lpszTip, nSize) == NULL)
			return FALSE;
	}
	else
	{
		tnd.szTip[0] = _T('\0');
	}

	if (lpszInfoTitle && lpszInfo)
	{
		int nSize;

		tnd.uFlags |= NIF_INFO;
		tnd.dwInfoFlags = NIIF_INFO;	// add an icon to a balloon ToolTip
		tnd.uTimeout = TRAY_INFO_TIMEOUT;

		nSize = min((int)(sizeof(tnd.szInfoTitle) / sizeof(tnd.szInfoTitle[0])), lstrlen(lpszInfoTitle) + 1);
		if (lstrcpyn(tnd.szInfoTitle, lpszInfoTitle, nSize) == NULL)
			return FALSE;

		nSize = min((int)(sizeof(tnd.szInfo) / sizeof(tnd.szInfo[0])), lstrlen(lpszInfo) + 1);
		if (lstrcpyn(tnd.szInfo, lpszInfo, nSize) == NULL)
			return FALSE;
	}

	res = Shell_NotifyIcon(dwMessage, &tnd);

	if (hIcon)
		DestroyIcon(hIcon);

	return res;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///          트레이 아이콘을 지운다
/// @param   hWnd 
/// @param   uID 
/// @return  
/// @date    2009-02-02 19:01:37
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL Tray_NotifyDelete(HWND hWnd, UINT uID)
{
	return TrayMessage(hWnd, NIM_DELETE, uID, NULL, NULL, NULL, NULL);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///          트레이 아이콘을 추가한다.
/// @param   hWnd 
/// @param   uID 
/// @param   hIcon 
/// @param   lpszTip 
/// @return  
/// @date    2009-02-02 19:01:49
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL Tray_NotifyAdd(HWND hWnd, UINT uID, HICON hIcon, LPCTSTR lpszTip, LPCTSTR lpszInfoTitle /*= NULL*/, LPCTSTR lpszInfo /*= NULL*/)
{
	return TrayMessage(hWnd, NIM_ADD, uID, hIcon, lpszTip, lpszInfoTitle, lpszInfo);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///          트레이의 아이콘 이미지를 바꾼다
/// @param   hWnd 
/// @param   uID 
/// @param   hIcon 
/// @param   lpszTip 
/// @return  
/// @date    2009-02-02 19:02:02
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL Tray_NotifyModify(HWND hWnd, UINT uID, HICON hIcon, LPCTSTR lpszTip, LPCTSTR lpszInfoTitle /*= NULL*/, LPCTSTR lpszInfo /*= NULL*/)
{
	return TrayMessage(hWnd, NIM_MODIFY, uID, hIcon, lpszTip, lpszInfoTitle, lpszInfo);
}

