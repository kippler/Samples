#include "stdafx.h"
#include "resource.h"
#include "MainDlg.h"
#include "Global.h"



CMainDlg::CMainDlg(void)
{
	m_hIcon = ::LoadIcon(_Module.m_hInst, MAKEINTRESOURCE(IDI_MAIN));
}

CMainDlg::~CMainDlg(void)
{
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         다이알로그 박스 초기화
/// @param  
/// @return 
/// @date   Friday, November 19, 2010  3:43:08 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CMainDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ApplyMultiLang();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
	this->CenterWindow();


	return TRUE;
}

LRESULT CMainDlg::OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDOK);
	return 0;
}

LRESULT CMainDlg::OnBnClickedCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(IDCANCEL);
	return 0;
}


void CMainDlg::ApplyMultiLang()
{
	// 다이알로그 박스 컨트롤에 언어 적용
	g_multilang.SetDlgTexts(m_hWnd, IDD);

	// 메뉴에 언어 적용
	HMENU hMenuOld = GetMenu();
	HMENU hMenu = ::LoadMenu(_Module.m_hInst, MAKEINTRESOURCE(IDR_MENU1));
	g_multilang.TranslateMenuString(hMenu);
	this->SetMenu(hMenu);
	::DestroyMenu(hMenuOld);
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///         영어로..
/// @param  
/// @return 
/// @date   Friday, September 23, 2011  10:06:14 AM
////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CMainDlg::OnLangEnglish(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	g_multilang.Create(LANG_ENGLISH_PATH, LANG_ENGLISH_PATH);
	ApplyMultiLang();

	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         한국어로
/// @param  
/// @return 
/// @date   Friday, September 23, 2011  10:06:35 AM
////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CMainDlg::OnLangKorean(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	g_multilang.Create(LANG_KOREAN_PATH, LANG_ENGLISH_PATH);
	ApplyMultiLang();

	return 0;
}

LRESULT CMainDlg::OnBnClickedBtnTest(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MessageBox(LANG(TEXT_MSG_MESSAGE), LANG(TEXT_MSG_TITLE), MB_OK);

	return 0;
}
