////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// 메인 윈도우
/// 
/// @author   parkkh
/// @date     Fri Oct 30 13:40:11 2015
/// 
/// Copyright(C) 2015 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

extern UINT WM_TRAY_NOTIFY;
extern UINT WM_TASKBARCREATED;

class CMainWnd  : public CWindowImpl<CMainWnd, CWindow>
{
public:
	CMainWnd();
	~CMainWnd();
	BOOL			Create();
	BOOL			PreTranslateMessage(MSG* pMsg);

private :
	void			RegisterHotkey();
	BOOL			DoStr(CString str);
	BOOL			DoCommand();

	void			SetTrayIcon();
	void			ModifyTrayIcon();
	CString			_LoadFile(CString fileName);

private :
	DWORD			m_cmd;
	HICON			m_hIcon;
	CString			m_str1;
	CString			m_str2;

public :
	BEGIN_MSG_MAP(CMainWnd)
		MESSAGE_HANDLER(WM_TRAY_NOTIFY, OnTrayNotify)
		MESSAGE_HANDLER(WM_TASKBARCREATED, OnTaskbarCreated)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_HOTKEY, OnHotkey)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
	END_MSG_MAP()

public:
	LRESULT			OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT			OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT			OnHotkey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT			OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT			OnTrayNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT			OnTaskbarCreated(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

