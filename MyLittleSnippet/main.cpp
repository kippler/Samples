#include "stdafx.h"
#include "MainWnd.h"
#include "resource.h"

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()
CComModule _Module;

BOOL InitInstance(HINSTANCE hInst);
BOOL ExitInstance();

int Main()
{
	CMainWnd main;
	MSG		msg;
	HACCEL	hAccelTable;

	if (main.Create()== FALSE)
		return 1;

	hAccelTable = LoadAccelerators(_Module.m_hInstResource, MAKEINTRESOURCEW(IDC_ACCEL));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (main.PreTranslateMessage(&msg) == FALSE)
		{
			if (!TranslateAccelerator(main.m_hWnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

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

