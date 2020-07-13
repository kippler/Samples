#include "stdafx.h"
#include "MainDlg.h"
#include "Global.h"

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()
CComModule _Module;



int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	g_multilang.LoadResourceHeader(IDR_RESHEADER, _T("Binary"));
	g_multilang.Create(LANG_KOREAN_PATH, LANG_ENGLISH_PATH);

	CMainDlg	dlg;
	dlg.DoModal();
	return 0;
}


