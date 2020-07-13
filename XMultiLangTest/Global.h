#pragma once


#include "util/XMultiLang.h"

#define LANG_KOREAN_PATH		GetThisPath() + _T("korean.ini")
#define LANG_ENGLISH_PATH		GetThisPath() + _T("english.ini")

#define LANG(x)					g_multilang.Lang(_T(#x))

// LANG(x) �� Lang(x) �� TEXT Section �� ����
// Lang(x,s)�� TEXT Section �ܿ��� ���� ����

extern XMultiLang g_multilang;

LPCTSTR Lang(LPCTSTR szLangKey, LPCTSTR szSection = NULL);
CString  GetThisPath(HMODULE hModule=NULL);

