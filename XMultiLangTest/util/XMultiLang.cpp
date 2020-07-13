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
// ������ : parkkh
// ������ : ������ (gline123@naver.com)
// ������ : 2012. 1. 6 
//
// History:
// 2012/01/06
//    1. Text�� ������ XMultiLang�� Hash Table�� ������ �װ��� ��� ������
//       XIni�� Linked List�� ã�Ƽ� ��� ����̾��µ� �ӵ� ����� ����
//       �ι��� ��ģ ã�⸦ ���� �ʵ��� ������(XIni�� Hash Table�� ����)
//
//    2. ���� ������� Text�� ���� ��� XIni�� �ִ� �ڷᰡ XMultiLang��
//       Hash Table���� �״�� ����Ǿ� Text Section�� �ڷᰡ ��������
//       ����Ǵ� ������ �־���
//
// 2012/01/07
//    1. Resource Header ���� �ε� �и� , Create() �Ҷ� �Ź� �ε�����
//       �ʰ� ó�� �ѹ��� �ϵ��� ������
//
//    2. Resource.h �� ���Ϸ� �θ� ������ �� �ֱ� ������ ���ҽ��� ����
//       �ϰ� �װ��� �ε��� �� �ִ� �Լ��� �߰���
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

	// �޸𸮷� ini ���� ����
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

	// ini ������ �� �� ����.
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

// ��뿹 : LoadResourceHeader(IDR_RESHEADER, _T("Binary"));
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

/* ���縦 �ؾ��� ��� free(pSrc) �� �̰��� �ּ��� Ǯ���
	// �޸𸮷� ���ν� ��� ���� ����
	// �޸� �����ϱ�
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
				if(m_mapid2str.Lookup(CString(szNumID), temp)) ASSERT(0);	// �̹� �ִ�??
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
					// �̹� �ִ�?? - ��ǥ�� �ڿ� ���δ�
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
///         ���̾˷α� �ڽ� ��Ʈ���� �ؽ�Ʈ�� ã�´�. (Ÿ��Ʋ�� ã���ش�.)
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

	// ��ǥ�� ���е� ��� ������ ����Ÿ�� ���ؼ� �� ������.
	XToken token;
	token.Init(sTextID, ',');

	sTextID = token.GetNextToken();
	while(sTextID.IsEmpty()==FALSE)
	{
		// "IDD_ABOUT" �� ���� ���ǿ��� ã�´�.
		// ù��° ��� ���Ͽ��� ã��
		sControlText = m_ini.GetStr(m_sCurDlgID, sTextID, _T(""));

		// ������� �⺻ ��� ���Ͽ��� ã��
		if(sControlText.IsEmpty())
			sControlText = m_iniDefault.GetStr(m_sCurDlgID, sTextID, _T(""));

		// �׷��� ���� ��� --- "DIALOG" ���ǿ��� ã�´�.
		if(sControlText.IsEmpty() && m_sCurDlgID.IsEmpty()==FALSE)
		{
			// ù��° ��� ���Ͽ��� ã��
			sControlText = m_ini.GetStr(_T("DIALOG"), sTextID, _T(""));

			// ������� �⺻ ��� ���Ͽ��� ã��
			if(sControlText.IsEmpty())
				sControlText = m_iniDefault.GetStr(_T("DIALOG"), sTextID, _T(""));
		}

		if(sControlText.IsEmpty()==FALSE)
			break;

		// ���� ��ū
		sTextID = token.GetNextToken();
	}


	// �ٹٲ� ó��
	sControlText.Replace(_T("\\n"), _T("\n"));
	sControlText.Replace(_T("\\r"), _T("\r"));


	return sControlText;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         enum �ݹ�
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

	// ��Ʈ�� �ؽ�Ʈ �ٲٱ�
	if(sControlText.IsEmpty()==FALSE)
		SetWindowText(hWnd, sControlText);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          ���̾˷α� �������� �ؽ�Ʈ �����ϱ�..
/// @param   hWnd  
/// @return  
/// @date    2005-03-12 ���� 4:54:35
////////////////////////////////////////////////////////////////////////////////////////////////////
void XMultiLang::SetDlgTexts(HWND hDlg, UINT nIDD)
{
	m_hWndParent = hDlg;

	// ���̾˷α� �ڽ��� ���ڿ� ���̵� ã��
	CString sIDD;
	sIDD.Format(_T("%d"), nIDD);
	m_sCurDlgID.Empty();
	m_mapid2str.Lookup(sIDD, m_sCurDlgID);

	// ���̾˷α� �ڽ� �� ��Ʈ�ѵ�
	EnumChildWindows(hDlg, EnumProc, (LPARAM)this);

	/*
	// ������ ȣ���ϴ� �ڵ�--> � �뵵���� �𸣰ڴ�..
	EnumProc(hDlg, (LPARAM)this);
	*/


	// ���̾˷α� �ڽ� Ÿ��Ʋ
	CString sControlText = FindDialogControlText(nIDD);

	if(sControlText.IsEmpty()==FALSE)
		SetWindowText(hDlg, sControlText);
}


CString XMultiLang::Lang(LPCTSTR szLangKey, LPCTSTR szSection)
{
	if(m_bCreated==FALSE) return _T("Can't load language file");		// �ʱ�ȭ ������ ���.


	CString buf;

	if(m_ini.GetStr2(szSection, szLangKey,buf)==FALSE || buf.IsEmpty())
	{
		if(m_iniDefault.GetStr2(szSection, szLangKey, buf)==FALSE)
			ASSERT(0);											// ������Ͽ� ����..
	}

	buf.Replace(_T("\\n"), _T("\n"));
	buf.Replace(_T("\\r"), _T("\r"));

	return buf;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         �޴� ���ڿ� �ٲٱ�
/// @param  
/// @return 
/// @date   2007�� 11�� 29�� �����  ���� 11:01:58
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

		// sub menu ó��
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

