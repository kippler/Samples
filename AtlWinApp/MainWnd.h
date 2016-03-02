////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// 메인 윈도우
/// 
/// @author   parkkh
/// @date     Friday, November 19, 2010  4:15:21 PM
/// 
/// Copyright(C) 2010 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once

class CMainWnd  : public CWindowImpl<CMainWnd, CWindow>
{
public:
	CMainWnd();
	~CMainWnd();

	BOOL			Create();
	BOOL			PreTranslateMessage(MSG* pMsg);


public :
	BEGIN_MSG_MAP(CMainWnd)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	END_MSG_MAP()

private:
	LRESULT			OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

};

