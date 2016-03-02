////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// 
/// 
/// @author   park
/// @date     03/02/16 14:46:34
/// 
/// Copyright(C) 2016 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once

CString	GetTimeString(LPCTSTR sFormat);
CString GetThisPath(HMODULE hModule = NULL);								// 이 프로세스의 실행 파일이 위치하는 경로

extern UINT WM_TRAY_NOTIFY;												// 트레이 상태 변화 알림 메시지
BOOL Tray_NotifyDelete(HWND hWnd, UINT uID);
BOOL Tray_NotifyAdd(HWND hWnd, UINT uID, HICON hIcon, LPCTSTR lpszTip, LPCTSTR lpszInfoTitle = NULL, LPCTSTR lpszInfo = NULL);
BOOL Tray_NotifyModify(HWND hWnd, UINT uID, HICON hIcon, LPCTSTR lpszTip, LPCTSTR lpszInfoTitle = NULL, LPCTSTR lpszInfo = NULL);
