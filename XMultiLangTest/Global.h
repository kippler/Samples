#pragma once


#include "util/XMultiLang.h"

#define LANG_KOREAN_PATH		GetThisPath() + _T("korean.ini")
#define LANG_ENGLISH_PATH		GetThisPath() + _T("english.ini")

#define LANG(x)					g_multilang.Lang(_T(#x))

// LANG(x) 와 Lang(x) 는 TEXT Section 만 접근
// Lang(x,s)는 TEXT Section 외에도 접근 가능

extern XMultiLang g_multilang;

LPCTSTR Lang(LPCTSTR szLangKey, LPCTSTR szSection = NULL);
CString  GetThisPath(HMODULE hModule=NULL);

