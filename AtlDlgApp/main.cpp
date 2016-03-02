#include "stdafx.h"
#include "MainDlg.h"
#include "resource.h"

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()
CComModule _Module;

BOOL InitInstance(HINSTANCE hInst);
BOOL ExitInstance();

int Main()
{
	CMainDlg dlg;
	dlg.DoModal();
	return 0;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
#ifdef _DEBUG
	//_CrtSetBreakAlloc(1081);
	_onexit(_CrtDumpMemoryLeaks);
#endif

	if (!InitInstance (hInstance)) 
		return FALSE;

	int ret = Main();

	ExitInstance();

	return ret;
}


BOOL InitInstance(HINSTANCE hInstance)
{
	_Module.Init(ObjectMap, hInstance);

	if (FAILED(::CoInitialize(NULL)))
		return FALSE;

	// 시스템 에러 다이알로그 박스를 보여주지 않음
	SetErrorMode(SEM_FAILCRITICALERRORS);

	return TRUE;
}

BOOL ExitInstance()
{
	::CoUninitialize();

	_Module.Term();

	return TRUE;
}

