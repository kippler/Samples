////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// multi-language utilty class
/// 
/// @author   parkkh
/// @date     Monday, February 02, 2009  9:40:02 AM
/// 
/// Copyright(C) 2009 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////
/*
history

- 2011/6/30 : 기존에 다이알로그 박스 관련 텍스트를 전부 [DIALOG] 에 때려박던 방식을 개선해서
              각각의 다이알로그박스 섹션을 지정할 수 있도록 ( [IDD_ABOUT] 과 같이.. ) 기능 추가

*/

/*

#define _XMULTILANG_DO_NOT_ASSERT_WHEN_DUPE_ID

*/


#pragma once

#include "XIni.h"

#define _XMULTILANG_DO_NOT_ASSERT_WHEN_DUPE_ID


class XMultiLang  
{
public:
	XMultiLang();
	~XMultiLang();

	BOOL						Create(CString sIniFilePath, CString sIniFilePathDefault);
	BOOL						Create(LPBYTE pIniFile, int nIniFileLen);

	BOOL						LoadResourceHeader(INT nResourceID, LPCTSTR lpResourceType);
	BOOL						LoadResourceHeader(LPBYTE pResourceHeader, int nResourceHeaderLen);
	BOOL						LoadResourceHeader(CString sResourceHeaderPath);

	void						SetDlgTexts(HWND hWnd, UINT nIDD);
	CString						Lang(LPCTSTR szLangKey, LPCTSTR szSection= NULL);
	int							GetCodePage() { return m_nCodePage; }
	void						TranslateMenuString(HMENU hMenu);
	XIni&						GetXIni() { return m_ini; }

private :
	static BOOL	CALLBACK		EnumProc(HWND hWnd, LPARAM lParam);
	BOOL						_Create(VOID);
	BOOL						_LoadResourceheader(VOID);
	CString						FindDialogControlText(int nCtrlID);

private :
#ifndef _MFC_VER
	CAtlMap<CString, CString>	m_mapid2str;
#else
	CMapStringToString 			m_mapid2str;
#endif

	int							m_nCodePage;
	HWND						m_hWndParent;
	XIni						m_ini;				// 기본 언어 파일
	XIni						m_iniDefault;		// 기본 언어에 없을때 사용할 추가언어 파일
	BOOL						m_bCreated;			// 성공적으로 Create() 되었나 ?

	CString						m_sCurDlgID;		// EnumChildWindows() 호출할때 현재 다이알로그 박스의 문자열 ID ("IDD_ABOUT" 과 같은 형태의 문자열)
};
