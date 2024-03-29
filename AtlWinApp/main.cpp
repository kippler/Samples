#include "stdafx.h"
#include "MainWnd.h"
#include "resource.h"

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()
#pragma warning(push)
#pragma warning(disable: 26426)
CComModule _Module;
#pragma warning(push)

BOOL InitInstance(HINSTANCE hInst) noexcept;
BOOL ExitInstance() noexcept;

int Main()
{
	CMainWnd	main;
	MSG			msg = { nullptr, };
	HACCEL		hAccelTable = LoadAccelerators(_Module.m_hInstResource, MAKEINTRESOURCEW(IDC_ACCEL));

	if (main.CreateMainWindow()== FALSE)
		return 1;

	while (GetMessage(&msg, nullptr, 0, 0))
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

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPTSTR /*lpCmdLine*/, _In_ int /*nCmdShow*/)
{
#ifdef _DEBUG
	//_CrtSetBreakAlloc(1081);
	_onexit(_CrtDumpMemoryLeaks);
#endif

	if (!InitInstance (hInstance)) 
		return FALSE;

	const int ret = Main();

	ExitInstance();

	return ret;
}


BOOL InitInstance(HINSTANCE hInstance) noexcept
{
	#pragma warning(push)
	#pragma warning(disable: 26485)		// Expression 'L"\x44992\x52896"': No array to pointer decay (bounds.3).
	_Module.Init(ObjectMap, hInstance);
	#pragma warning(pop)

	if (FAILED(::CoInitialize(nullptr)))
		return FALSE;

	// 시스템 에러 다이알로그 박스를 보여주지 않음
#ifndef _DEBUG
	SetErrorMode(SEM_FAILCRITICALERRORS);
#endif

	return TRUE;
}

BOOL ExitInstance() noexcept
{
	::CoUninitialize();

	_Module.Term();

	return TRUE;
}
