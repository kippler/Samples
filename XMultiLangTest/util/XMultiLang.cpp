#include "stdafx.h"
#include "XMultiLang.h"
#include "XToken.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////
//
// 제작자 : parkkh
// 수정자 : 서광현 (gline123@naver.com)
// 수정일 : 2012. 1. 6 
//
// History:
// 2012/01/06
//    1. Text를 읽을때 XMultiLang의 Hash Table에 있으면 그것을 얻고 없으면
//       XIni의 Linked List를 찾아서 얻는 방식이었는데 속도 향상을 위해
//       두번에 걸친 찾기를 하지 않도록 변경함(XIni를 Hash Table로 구현)
//
//    2. 기존 방식으로 Text를 읽을 경우 XIni에 있는 자료가 XMultiLang의
//       Hash Table에도 그대로 저장되어 Text Section의 자료가 이중으로
//       저장되는 단점이 있었음
//
// 2012/01/07
//    1. Resource Header 파일 로딩 분리 , Create() 할때 매번 로딩하지
//       않고 처음 한번만 하도록 변경함
//
//    2. Resource.h 를 파일로 두면 편집될 수 있기 때문에 리소스에 포함
//       하고 그것을 로딩할 수 있는 함수를 추가함
//       LoadResourceHeader(INT nResourceID, LPCTSTR lpResourceType)
//       
////////////////////////////////////////////////////////////////////

#define LEN_BUF 1024


XMultiLang::XMultiLang()
{
	m_hWndParent = NULL;
	m_nCodePage = CP_ACP;
	m_bCreated = FALSE;
}

XMultiLang::~XMultiLang()
{

}

BOOL XMultiLang::Create(LPBYTE pIniFile, int nIniFileLen)
{
	m_bCreated = FALSE;

	// 메모리로 ini 파일 열기
	m_ini.OpenMemory(pIniFile, nIniFileLen);

	return _Create();
}

BOOL XMultiLang::Create(CString sIniFilePath, CString sIniFilePathDefault)
{
	m_bCreated = FALSE;

	BOOL ret1=FALSE, ret2=FALSE;

	ret1 = m_ini.Open(sIniFilePath);
	if(sIniFilePathDefault.IsEmpty()==FALSE)
		ret2 = m_iniDefault.Open(sIniFilePathDefault);

	// ini 파일을 열 수 없음.
	if(ret1==FALSE && ret2==FALSE)
		return FALSE;

	return _Create();
}


BOOL XMultiLang::_Create(VOID)
{
	//m_nCodePage = ::GetIniInt(_T("GENERAL"), _T("CODEPAGE"), CP_ACP, m_sIniFilePath);
	m_nCodePage = m_ini.GetInt(	_T("GENERAL"), _T("CODEPAGE"), CP_ACP);

	m_bCreated = TRUE;

	return TRUE;
}

// 사용예 : LoadResourceHeader(IDR_RESHEADER, _T("Binary"));
BOOL XMultiLang::LoadResourceHeader(INT nResourceID, LPCTSTR lpResourceType)
{
	HMODULE hModule = AfxGetResourceHandle();
	BOOL bRet = FALSE;

	HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(nResourceID), lpResourceType);
	if(hResource == NULL) return FALSE;

	HGLOBAL hBinGlob = LoadResource(hModule, hResource);
	if(hBinGlob)
	{
		VOID *pBinData = LockResource(hBinGlob);
		if(pBinData)
		{
			bRet = LoadResourceHeader((PBYTE)pBinData, SizeofResource(hModule, hResource));
		}
		UnlockResource(hBinGlob);
	}
	FreeResource(hResource);
	return bRet;
}

BOOL XMultiLang::LoadResourceHeader(LPBYTE pResourceHeader, int nResourceHeaderLen)
{
	m_mapid2str.RemoveAll();

	CHAR *pSrc = (PCHAR)pResourceHeader;

/* 복사를 해야할 경우 free(pSrc) 와 이것의 주석을 풀면됨
	// 메모리로 리로스 헤더 파일 열기
	// 메모리 복사하기
	CHAR* pSrc = (CHAR*)malloc(nResourceHeaderLen + sizeof(CHAR));
	if(pSrc==NULL){ASSERT(0); return FALSE;}

	memcpy(pSrc, pResourceHeader, nResourceHeaderLen);
	pSrc[nResourceHeaderLen] = NULL;
*/

	CHAR szDefine[LEN_BUF], szTextID[LEN_BUF], szNumID[LEN_BUF];
	CString temp;

	int		nSrcPos=0;

	while(nSrcPos < nResourceHeaderLen)
	{
		szDefine[0] = szTextID[0] = szNumID[0] = 0;
		//_stscanf(buf, _T("%s %s %s"), szDefine, szTextID, szNumID);
		nSrcPos += sscanf_s(pSrc + nSrcPos, "%s %s %s", szDefine, LEN_BUF, szTextID, LEN_BUF, szNumID, LEN_BUF);
		if(strncmp(szDefine, "#define", 7)==0)
		{
			//m_mapstr2id[szTextID] = szNumID;
			if(szTextID[0]!='_')
			{
				if(m_mapid2str.Lookup(CString(szNumID), temp)) ASSERT(0);	// 이미 있다??
				m_mapid2str[CString(szNumID)] = szTextID;
			}
		}

		while(nSrcPos < nResourceHeaderLen)
		{
			if( *(pSrc + nSrcPos)=='#') 
				break;
			else
				nSrcPos++;
		}
	}
//	free(pSrc);

	return _LoadResourceheader();
}

BOOL XMultiLang::LoadResourceHeader(CString sResourceHeaderPath)
{
	m_mapid2str.RemoveAll();

	if(sResourceHeaderPath.IsEmpty()) return FALSE;
	FILE* fp;
	//fp = _tfopen(sResourceHeaderPath, _T("r"));
	errno_t err = _tfopen_s(&fp, sResourceHeaderPath, _T("r"));
	if(err!=0) {ASSERT(0);return FALSE;}
	if(fp==NULL) {ASSERT(0);return FALSE;}


	TCHAR buf[LEN_BUF];
	TCHAR szDefine[LEN_BUF], szTextID[LEN_BUF], szNumID[LEN_BUF];
	CString temp;

	while(!feof(fp))
	{
		_fgetts(buf, LEN_BUF, fp);
		szDefine[0] = szTextID[0] = szNumID[0] = 0;
		//_stscanf(buf, _T("%s %s %s"), szDefine, szTextID, szNumID);
		_stscanf_s(buf, _T("%s %s %s"), szDefine, LEN_BUF, szTextID, LEN_BUF, szNumID, LEN_BUF);
		if(_tcsncmp(szDefine, _T("#define"), 7)==0)
		{
			//m_mapstr2id[szTextID] = szNumID;
			if(szTextID[0]!='_')
			{
				if(m_mapid2str.Lookup(szNumID, temp)) 
				{
#ifndef _XMULTILANG_DO_NOT_ASSERT_WHEN_DUPE_ID
					ASSERT(0);
#endif
					// 이미 있다?? - 쉼표로 뒤에 붙인다
					temp = temp + _T(",") + szTextID;
					m_mapid2str[szNumID] = temp;
				}
				else
					m_mapid2str[szNumID] = szTextID;
			}
		}
	}
	fclose(fp);
	return _LoadResourceheader();
}

BOOL XMultiLang::_LoadResourceheader(VOID)
{
	m_mapid2str[_T("1")] = _T("IDOK");
	m_mapid2str[_T("2")] = _T("IDCANCEL");

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         다이알로그 박스 컨트롤의 텍스트를 찾는다. (타이틀도 찾아준다.)
/// @param  
/// @return 
/// @date   Thursday, June 30, 2011  11:31:10 AM
////////////////////////////////////////////////////////////////////////////////////////////////////
CString	XMultiLang::FindDialogControlText(int nCtrlID)
{
	CString sNumID;
	CString sTextID;
	CString sControlText;

	sNumID.Format(_T("%d"), nCtrlID);

	if(m_mapid2str.Lookup(sNumID, sTextID)==FALSE) return sControlText;

	// 쉼표로 구분된 경우 각각의 데이타에 대해서 다 뒤진다.
	XToken token;
	token.Init(sTextID, ',');

	sTextID = token.GetNextToken();
	while(sTextID.IsEmpty()==FALSE)
	{
		// "IDD_ABOUT" 과 같은 섹션에서 찾는다.
		// 첫번째 언어 파일에서 찾기
		sControlText = m_ini.GetStr(m_sCurDlgID, sTextID, _T(""));

		// 없을경우 기본 언어 파일에서 찾기
		if(sControlText.IsEmpty())
			sControlText = m_iniDefault.GetStr(m_sCurDlgID, sTextID, _T(""));

		// 그래도 없는 경우 --- "DIALOG" 섹션에서 찾는다.
		if(sControlText.IsEmpty() && m_sCurDlgID.IsEmpty()==FALSE)
		{
			// 첫번째 언어 파일에서 찾기
			sControlText = m_ini.GetStr(_T("DIALOG"), sTextID, _T(""));

			// 없을경우 기본 언어 파일에서 찾기
			if(sControlText.IsEmpty())
				sControlText = m_iniDefault.GetStr(_T("DIALOG"), sTextID, _T(""));
		}

		if(sControlText.IsEmpty()==FALSE)
			break;

		// 다음 토큰
		sTextID = token.GetNextToken();
	}


	// 줄바꿈 처리
	sControlText.Replace(_T("\\n"), _T("\n"));
	sControlText.Replace(_T("\\r"), _T("\r"));


	return sControlText;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         enum 콜백
/// @param  
/// @return 
/// @date   Thursday, June 30, 2011  11:36:50 AM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK XMultiLang::EnumProc(HWND hWnd, LPARAM lParam)
{
	XMultiLang*  _this = (XMultiLang*)lParam;

	if(_this->m_hWndParent != ::GetParent(hWnd)) return TRUE;

	UINT nCtrlID = ::GetDlgCtrlID(hWnd);
	CString sControlText;

	sControlText = _this->FindDialogControlText(nCtrlID);

	// 컨트롤 텍스트 바꾸기
	if(sControlText.IsEmpty()==FALSE)
		SetWindowText(hWnd, sControlText);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          다이알로그 아이템의 텍스트 세팅하기..
/// @param   hWnd  
/// @return  
/// @date    2005-03-12 오후 4:54:35
////////////////////////////////////////////////////////////////////////////////////////////////////
void XMultiLang::SetDlgTexts(HWND hDlg, UINT nIDD)
{
	m_hWndParent = hDlg;

	// 다이알로그 박스의 문자열 아이디 찾기
	CString sIDD;
	sIDD.Format(_T("%d"), nIDD);
	m_sCurDlgID.Empty();
	m_mapid2str.Lookup(sIDD, m_sCurDlgID);

	// 다이알로그 박스 내 컨트롤들
	EnumChildWindows(hDlg, EnumProc, (LPARAM)this);

	/*
	// 기존에 호출하던 코드--> 어떤 용도인지 모르겠다..
	EnumProc(hDlg, (LPARAM)this);
	*/


	// 다이알로그 박스 타이틀
	CString sControlText = FindDialogControlText(nIDD);

	if(sControlText.IsEmpty()==FALSE)
		SetWindowText(hDlg, sControlText);
}


CString XMultiLang::Lang(LPCTSTR szLangKey, LPCTSTR szSection)
{
	if(m_bCreated==FALSE) return _T("Can't load language file");		// 초기화 실패한 경우.


	CString buf;

	if(m_ini.GetStr2(szSection, szLangKey,buf)==FALSE || buf.IsEmpty())
	{
		if(m_iniDefault.GetStr2(szSection, szLangKey, buf)==FALSE)
			ASSERT(0);											// 언어파일에 없다..
	}

	buf.Replace(_T("\\n"), _T("\n"));
	buf.Replace(_T("\\r"), _T("\r"));

	return buf;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         메뉴 문자열 바꾸기
/// @param  
/// @return 
/// @date   2007년 11월 29일 목요일  오후 11:01:58
////////////////////////////////////////////////////////////////////////////////////////////////////
#define LEN_BUF	1024
void XMultiLang::TranslateMenuString(HMENU hMenu)
{
	int count;
	int i;
	int ret;
	CString str;
	MENUITEMINFO  info;
	TCHAR			buf[LEN_BUF];
	int				id;

	count = ::GetMenuItemCount(hMenu);

	info.cbSize = sizeof (MENUITEMINFO);
	info.fMask = MIIM_SUBMENU;

	for(i=0;i<count;i++)
	{
		ret = ::GetMenuString(hMenu, i, buf, LEN_BUF, MF_BYPOSITION);
		id = ::GetMenuItemID(hMenu, i);

		str = m_ini.GetStr(_T("MENU"), buf);
		if(str.IsEmpty())
			str = m_iniDefault.GetStr(_T("MENU"), buf);

		str.Replace(_T("\\t"), _T("\t"));		// \t to tab

		if(str.IsEmpty()==FALSE)
			::ModifyMenu(hMenu, i, MF_BYPOSITION | MF_STRING, id, str);

		// sub menu 처리
		if(	GetMenuItemInfo(hMenu, i, TRUE, &info))
		{
			if(info.hSubMenu)
			{
				HMENU hSub = GetSubMenu(hMenu, i);
				TranslateMenuString(hSub);
			}
		}
	}
}

