#include "stdafx.h"
#include "Global.h"


XMultiLang		g_multilang;


LPCTSTR Lang(LPCTSTR szLangKey, LPCTSTR szSection)
{
	return g_multilang.Lang(szLangKey, szSection);
}



CString  GetThisPath(HMODULE hModule)
{
	static TCHAR szModuleName[MAX_PATH * 4] = { '\0' };

	if(!*szModuleName)
	{
		::GetModuleFileName(hModule, szModuleName, MAX_PATH * 4);
		TCHAR *pLastBackslash = _tcsrchr(szModuleName, '\\');
		if(pLastBackslash) *(pLastBackslash + 1) = '\0';
	}
	return CString(szModuleName);
}
