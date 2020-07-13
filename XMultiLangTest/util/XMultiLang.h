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

- 2011/6/30 : ������ ���̾˷α� �ڽ� ���� �ؽ�Ʈ�� ���� [DIALOG] �� �����ڴ� ����� �����ؼ�
              ������ ���̾˷α׹ڽ� ������ ������ �� �ֵ��� ( [IDD_ABOUT] �� ����.. ) ��� �߰�

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
	XIni						m_ini;				// �⺻ ��� ����
	XIni						m_iniDefault;		// �⺻ �� ������ ����� �߰���� ����
	BOOL						m_bCreated;			// ���������� Create() �Ǿ��� ?

	CString						m_sCurDlgID;		// EnumChildWindows() ȣ���Ҷ� ���� ���̾˷α� �ڽ��� ���ڿ� ID ("IDD_ABOUT" �� ���� ������ ���ڿ�)
};
