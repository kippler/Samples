#pragma once
#include "resource.h"

class CMainDlg : public CDialogImpl <CMainDlg, CWindow>
{
public:
	CMainDlg();
	~CMainDlg();

	enum { IDD = IDD_MAIN};

BEGIN_MSG_MAP(CMainDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnBnClickedOk)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnBnClickedCancel)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
END_MSG_MAP()


private :
	HICON	m_hIcon;

private :
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnBnClickedOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedCancel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

};

