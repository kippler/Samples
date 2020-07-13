#include "stdafx.h"
#include "XIni.h"

////////////////////////////////////////////////////////////////////
//
// ������ : parkkh
// ������ : ������ (gline123@naver.com)
// ������ : 2012. 1. 6 
//
// History:
// 2012/01/06
//    1. INI ���Ϸ� ���� ���� Section name�� Linked List�� �����ϰ�
//       Section ���� Key�� Value�� Linked List�� ������ ����
//       �˻� �ӵ��� ����ϱ� ���Ͽ� Hash Table�� �̿��ϵ��� ������.
//
//    2. Key�� Value�� ���� "HELLO" = "WORLD" �� ���� �����ο��ȣ��
//       ����� �� �ֵ��� �� ���� 'HELLO' = 'WORLD' �� ����� ��
//       �ֵ��� ��, ���� ��� "ABC"DEF" �� ���� ���� �ִ� ���
//       �������� "ABC" ���� ���� ABC�� �ν��ϰ� �Ǵµ� �� ��쿡
//       'ABC"DEF'�� �ϸ� ABC"DEF�� ��� �ν��� �� ����
//
//    3. ���� Section���� ���������� ã�� ��� Section�� ã������
//       Hash Table�� ã�� �ʰ� �ٷ� ���� �� �ֵ��� ��.
//
//    4. TEXT�� Hash Table�� ã�� �ʰ� �ٷ� ���� �� �ֵ��� ��
//
//    5. ����: ���� INI ������ SaveAs()�� ������ ��� Hash Table�� ����
//       �����ϱ� ������ ���� INI�� ����� ������ ���� �ٸ� �� ����
//
////////////////////////////////////////////////////////////////////

#define LEN_OF_BOM	2

#define FindData(pSection,szKey,strData)	((pSection)->Lookup(CString(szKey), strData))


XIni::XIni()
{
	Init();
}

XIni::XIni(LPCTSTR szPathName)
{
	Init();
	Open(szPathName);
}

void XIni::Init()
{
	m_bUnicodeIniFile = TRUE;
	m_nCodePage = CP_ACP;
	m_bModified = FALSE;
	m_bOpen = FALSE;
	m_pDataCurrent = NULL;
	m_nStrLen = 0;
	m_nStrPos = 0;

	m_pCurSection = NULL;
	m_pTextSection = NULL;
}

XIni::~XIni()
{
	Clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          .ini ���� ����
/// @param   szPathName  
/// @return  
/// @date    2006-11-08 ���� 11:01:14
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XIni::Open(LPCTSTR szPathName)
{
	// ���� ����Ÿ�� �����.
	Clear();

	BYTE*		pData;							// ����Ÿ ����
	int			nDataLen;						// ����Ÿ ũ��
	HANDLE		hFile;							// ���� �ڵ�
	DWORD		read;

	// ���� ����
	hFile = ::CreateFile(szPathName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if(hFile==INVALID_HANDLE_VALUE) {ASSERT(0); return FALSE; }

	// �޸� ���.
	nDataLen = ::GetFileSize(hFile, NULL);			// ���� ũ��
	if(nDataLen>1000*1000) {ASSERT(0); CloseHandle(hFile); return FALSE;}	// �ʹ� ũ��!!!
	if(nDataLen==0) {CloseHandle(hFile); return TRUE;}						// 0 ����Ʈ�̴�..
	pData = (BYTE*)calloc(1, nDataLen+sizeof(WCHAR));
	if(pData==NULL){ASSERT(0); CloseHandle(hFile); return FALSE;}

	// ���� �б�
	if(::ReadFile(hFile, pData, nDataLen, &read, NULL)==FALSE || read!=(DWORD)nDataLen)
	{
		ASSERT(0);
		free(pData);
		CloseHandle(hFile);
		return FALSE;
	}

	// �޸� ����� ó��
	BOOL ret;
	ret = OpenMemory(pData, nDataLen);

	// ����
	CloseHandle(hFile);
	free(pData);

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///          �޸𸮷� ini ������ �д´�... (BOM �� ������� �����ڵ� ó�� ����)
///          ����Ÿ ������ ���� �ݵ�� NULL�� ������ �Ѵ�.!! -> ��� �ǵ��� �����ߵ�..
/// @param   pData  
/// @param   nDataLen  
/// @return  
/// @date    2006-11-20 ���� 3:21:09
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XIni::OpenMemory(const BYTE* pData, int nDataLen)
{
	if(nDataLen<LEN_OF_BOM) {ASSERT(0); return FALSE;}

	LPCTSTR		pNewData = NULL;
	LPCTSTR		szData;
	int			nStrLen = 0;									// ���ڿ� ī��Ʈ

	if(IsUnicodeFileHeader(pData))								// �����ڵ� ���Ͽ��� ���� ����Ÿ �ΰ�?
	{
#ifdef _UNICODE													// unicode -> unicode
		szData = (LPCTSTR)(pData+LEN_OF_BOM);					// bom ����
		nStrLen = (nDataLen-LEN_OF_BOM) / sizeof(WCHAR);
#else															// unicode -> ascii
		pNewData = Unicode2Ascii((LPCWSTR)(pData+LEN_OF_BOM), m_nCodePage, nStrLen);
		if(pNewData==NULL){ASSERT(0); return FALSE;}
		szData = pNewData;
#endif
	}
	else														// BOM �� ����? == ASII
	{
#ifdef _UNICODE													// ascii -> unicode
		pNewData = Ascii2Unicode((LPCSTR)pData, m_nCodePage);
		if(pNewData==NULL){ASSERT(0); return FALSE;}
		szData = pNewData;
#else															// ascii -> ascii
		szData = (LPCTSTR)(pData);
#endif

		nStrLen = nDataLen;

	}

	// ���ڿ��� ����Ÿ ����
	m_bOpen = OpenMemory(szData, nStrLen);

	// �޸� ����.
	if(pNewData) free((void*)pNewData);

	return m_bOpen;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          �޸𸮷� ���� - ���� NULL �� �ȳ����� �ȴ�..
/// @param   szData  
/// @return  
/// @date    2006-11-20 ���� 4:16:02
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XIni::OpenMemory(LPCTSTR szData, int nStrLen)
{
	CString sKey, sStrVal;

	m_pDataCurrent = szData;

	m_nStrLen = nStrLen;
	m_nStrPos = 0;

	LINE_TYPE nLineType;
	SectionMap *pCurSection = NULL;
	for(;;)
	{
		if(ReadALine()==FALSE) break;				// read and check eof
		if(m_sLine.IsEmpty()) continue;

		nLineType = GetLineType(m_sLine);
		if(nLineType==LINE_TYPE_SECTION)
		{
			CString sSectionName = ParseSection(m_sLine);
			if(sSectionName.IsEmpty())				// ������ �ִ� ����!
			{
				ASSERT(0);
				pCurSection = NULL;					// ���� ���·� �����.
			}
			else
			{
				pCurSection = FindOrMakeSection(sSectionName);	// ���� ���� �ٲٱ�.
				if(!m_pTextSection && sSectionName.CompareNoCase(DEFAULT_TEXT_SECTION) == 0)
				{
					m_pTextSection = pCurSection;
				}
			}
		}
		else if(nLineType==LINE_TYPE_DATA)
		{
			if(pCurSection==NULL)
			{
				ASSERT(0);						// ���� ��Ȳ
			}
			else
			{
				if(ParseData(m_sLine, sKey, sStrVal)==FALSE)
				{
					ASSERT(0);					// ���� ��Ȳ..
				}
				else
				{
					(*pCurSection)[sKey] = sStrVal;
				}
			}
		}
		else if(nLineType==LINE_TYPE_SINGLELINE)		// = ���� �׳� ����¥��
		{
			if(pCurSection==NULL)
			{
				ASSERT(0);						// ���� ��Ȳ
			}
			else
			{
				(*pCurSection)[m_sLine] = CString(""); // Ű���� �ִ´�.
			}
		}
		else
		{
			// todo : �ּ�ó��, ��ĭó��, ��Ÿ ��� ó��..
		}
	}

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///          ���� ����Ÿ �� �����.
/// @return  
/// @date    2006-11-08 ���� 10:56:45
////////////////////////////////////////////////////////////////////////////////////////////////////
void XIni::Clear()
{
	SectionMap *pSection;

	POSITION p = m_DataMap.GetStartPosition();
	while(p)
	{
		pSection = m_DataMap.GetNextValue(p);
		pSection->RemoveAll();
		delete pSection;
	}
	m_DataMap.RemoveAll();

	m_bUnicodeIniFile = FALSE;
	m_bModified = FALSE;
	m_bOpen = FALSE;

	m_pCurSection = NULL;
	m_pTextSection = NULL;
	m_strCurSection = "";
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          ���� �о ����� m_sLine �� �����ϱ�
/// @return  EOF �̸� FALSE
/// @date    2006-11-08 ���� 11:04:47
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XIni::ReadALine()
{
	TCHAR	c;
	BOOL	bEof=FALSE;


	m_sLine.Empty();
	for(;;)
	{
		m_nStrPos++;
		if(m_nStrPos>m_nStrLen)
		{
			bEof = TRUE;									// eof!
			break;
		}

		c = *m_pDataCurrent;
		if(c==0)
		{
			bEof = TRUE;									// eof!
			break;
		}
		m_pDataCurrent ++;

		if(c=='\n') break;									// end of line
		if(c=='\r') continue;								// skip
		m_sLine += c;
	}

	if(m_sLine.GetLength()==0 && bEof==TRUE) return FALSE;		// end of file!

	// ������ ������ �������� �ʴ´�!!!!!!
	m_sLine.TrimLeft(); 
	m_sLine.TrimRight();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          BOM üũ�ϱ�.
/// @param   buf  
/// @return  
/// @date    2006-11-08 ���� 11:06:04
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XIni::IsUnicodeFileHeader(const BYTE* buf)
{
	if(buf[0]==0xff && buf[1]==0xfe) return TRUE;	// little endian BOM �� ó���Ѵ�.
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          ������ ���� �����ΰ�?
/// @param   sLine  
/// @return  
/// @date    2006-11-08 ���� 1:35:36
////////////////////////////////////////////////////////////////////////////////////////////////////
XIni::LINE_TYPE XIni::GetLineType(CString sLine)
{
	if(sLine.GetLength()==0) return LINE_TYPE_EMPTY;

	if(sLine.Left(1)==_T("[") && sLine.Right(1)==_T("]")) 
		return LINE_TYPE_SECTION;

	if(sLine.Left(1)==_T(";"))
		return LINE_TYPE_REMARK;

	if(sLine.Find('=')>=0) 
		return LINE_TYPE_DATA;

	return LINE_TYPE_SINGLELINE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          "[HELLO]" ���� "HELLO" �� �����Ѵ�.
/// @param   sLine  
/// @return  ���н� "" ����
/// @date    2006-11-08 ���� 1:46:14
////////////////////////////////////////////////////////////////////////////////////////////////////
CString XIni::ParseSection(CString sLine)
{
	CString sRet = sLine.Mid(1, sLine.GetLength()-2);
	sRet.TrimLeft(); sRet.TrimRight();
	return sRet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          �̸����� ������ ã�´�.
/// @param   sSectionName  
/// @return  
/// @date    2006-11-08 ���� 2:17:52
////////////////////////////////////////////////////////////////////////////////////////////////////
XIni::SectionMap* XIni::FindSection(LPCTSTR szSection)
{
	SectionMap *pSection;

	if(!szSection) return m_pTextSection;
	if(m_strCurSection.CompareNoCase(szSection) == 0) return m_pCurSection;
	if(!m_DataMap.Lookup(szSection, pSection)) return NULL;

	m_strCurSection = szSection;
	m_pCurSection = pSection;
	return pSection;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          SECTION NAME �� ������, ������ �������, ���� ������ ã�´�..
/// @param   sSectionName  
/// @return  
/// @date    2006-11-08 ���� 1:45:27
////////////////////////////////////////////////////////////////////////////////////////////////////
XIni::SectionMap* XIni::FindOrMakeSection(LPCTSTR szSection)
{
	SectionMap *pSection;

	if(!szSection) return m_pTextSection;
	if(m_strCurSection.CompareNoCase(szSection) == 0) return m_pCurSection;
	if(!m_DataMap.Lookup(szSection, pSection))
	{
		pSection = new SectionMap;
		m_DataMap[CString(szSection)] = pSection;
	}
	return pSection;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// HELLO=WORLD �� ���� ������ �Ľ��Ѵ�.
///
/// �Ʒ��� ���� ���µ� �Ľ� �����ϴ�.
///   1) HELLO=WORLD 
///   2) "HELLO"="WORLD"
///   3) HELLO=WORLD ; REMARK
///
/// @param   data  
/// @return  
/// @date    2006-11-08 ���� 1:52:01
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XIni::ParseData(CString sLine, CString& sKey, CString& sStrVal)
{
	const TCHAR* p = sLine;
	CHAR chQuot;

	sKey.Empty();
	sStrVal.Empty();

	// LEFT �Ľ�
	chQuot = (*p=='"' || *p=='\'') ? *p : '\0';
	if(chQuot) p++;
	while(*p)
	{
		if(*p == chQuot) { p++; break; }// "�� ������.
		if(!chQuot && *p=='=') break;	// =�� ã�Ҵ�.
		sKey += *p;
		p++;
	}
	if(*p==0) {ASSERT(0); return FALSE;}	// �Ľ� ����

	if(!chQuot)							// ����ǥ�� ������
	{ sKey.TrimLeft(); sKey.TrimRight();}

	if(sKey.IsEmpty()) {ASSERT(0); return FALSE;}		// ������ �����̴�! ����

	// "aa" = "b" ���� = ������ ���� �ǳ� ���
	p = SkipWhiteSpace(p);
	if(*p!='=') {ASSERT(0); return FALSE;}					// = �� ����?
	p++;					// = �ǳ� ���

	// = ������ ���� �ǳ� ���
	p = SkipWhiteSpace(p);

	// RIGHT �Ľ�
	chQuot = (*p=='"' || *p=='\'') ? *p : '\0';
	if(chQuot) p++;
	while(*p)
	{
		if(*p == chQuot) break;			// "�� ������.
		if(!chQuot && *p==';') break;	// ;�� ã�Ҵ�.
		sStrVal+= *p;
		p++;
	}
	if(!chQuot)							// ����ǥ�� ������
	{ sStrVal.TrimLeft(); sStrVal.TrimRight();}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          ���� �ǳ� ���
/// @param   p  
/// @return  
/// @date    2006-11-08 ���� 2:02:03
////////////////////////////////////////////////////////////////////////////////////////////////////
LPCTSTR XIni::SkipWhiteSpace(LPCTSTR p)
{
	while(*p)
	{
		if(*p==' ' || *p=='\t') p++;
		else break;
	}
	return p;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          INT ���� ã�Ƽ� �����´�.
/// @param   szSection  
/// @param   szKey  
/// @param   nDefault  
/// @param   BASE_DECIMAL  
/// @return  
/// @date    2006-11-08 ���� 2:16:16
////////////////////////////////////////////////////////////////////////////////////////////////////
int XIni::GetInt(LPCTSTR szSection, LPCTSTR szKey, int nDefault, int nBase) 
{
	SectionMap *pSection = FindSection(szSection);
	if(pSection==NULL) return nDefault;

	CString strData;
	if(FindData(pSection, szKey, strData)==FALSE) return nDefault;

	int ret;
	ret = _tcstoul(strData, NULL, nBase);

	return ret;
}

BOOL XIni::GetInt2(LPCTSTR szSection, LPCTSTR szKey, int& result, int nDefault, int nBase)
{
	result = nDefault;

	SectionMap* pSection = FindSection(szSection);
	if(pSection==NULL) return FALSE;

	CString strData;
	if(FindData(pSection, szKey, strData)==FALSE) return FALSE;

	result = _tcstoul(strData, NULL, nBase);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          STRING ���� ã�´�.
/// @param   szSection  
/// @param   szKey  
/// @param   szDefault  
/// @return  
/// @date    2006-11-08 ���� 3:04:51
////////////////////////////////////////////////////////////////////////////////////////////////////
CString	XIni::GetStr(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szDefault)
{
	SectionMap* pSection = FindSection(szSection);
	if(pSection==NULL) return szDefault;

	CString strData;
	if(FindData(pSection, szKey, strData)==FALSE) return szDefault;

	return strData;
}

BOOL XIni::GetStr2(LPCTSTR szSection, LPCTSTR szKey, CString& sResult, LPCTSTR szDefault)
{
	SectionMap* pSection = FindSection(szSection);
	if(pSection==NULL) { sResult= szDefault; return FALSE; }

	CString strData;
	if(FindData(pSection, szKey, strData)==FALSE) { sResult= szDefault; return FALSE; }

	sResult = strData;
	return TRUE;
}


LPSTR XIni::Unicode2Ascii(LPCWSTR szInput, int nCodePage, int& asciilen)
{
	LPSTR	ret=NULL;
	int		wlen = int((wcslen(szInput)+1)*sizeof(WCHAR));

	asciilen = wlen;					// asciilen �� wlen ���� ũ�� �ʴ�.
	ret = (LPSTR)malloc(asciilen);		// ASCII �� ����
	if(ret==NULL) {ASSERT(0); return NULL;}
	asciilen = WideCharToMultiByte(nCodePage, 0, szInput,  -1, ret, asciilen, NULL, NULL);
	if(asciilen==0)	// UCS2->ascii
	{ASSERT(0); free(ret); return NULL;}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          ASCII TO UNICODE
/// @return  ������� �ݵ�� free() �� ����
/// @date    2006-07-20 ���� 3:50:40
////////////////////////////////////////////////////////////////////////////////////////////////////
LPWSTR XIni::Ascii2Unicode(LPCSTR szInput, int nCodePage)
{
	LPWSTR ret;
	int	   ucs2len = int(strlen(szInput)+sizeof(CHAR));
	ret = (LPWSTR)malloc(ucs2len*sizeof(WCHAR));				// UCS2 �� ���� ���
	if(ret==NULL){ASSERT(0); return NULL;}

	if(MultiByteToWideChar(nCodePage, 0, szInput, -1, ret, ucs2len)==0)	// MBCS -> UCS2
	{ASSERT(0); free(ret); return NULL;}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         ���Ϸ� �����ϱ�.
/// @param  
/// @return 
/// @date   Friday, April 11, 2008  5:03:29 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XIni::SaveAs(LPCTSTR szFilePathName)
{
	//FILE* fp = _tfopen(szFilePathName, _T("wb"));
	FILE* fp;
	errno_t err = _tfopen_s(&fp, szFilePathName, _T("wb"));
	if(err!=0) {ASSERT(0); return FALSE;}
	if(fp==NULL){ASSERT(0); return FALSE;}


	// bom ����
#ifdef _UNICODE
	fputc(0xff, fp);
	fputc(0xfe, fp);
#endif

	POSITION p, q;
	SectionMap *pSection;
	DataMap::CPair *pDataPair;
	SectionMap::CPair *pSectionPair;

	p = m_DataMap.GetStartPosition();
	while(p)
	{
		pDataPair = m_DataMap.GetNext(p);
		::_ftprintf(fp, _T("[%s]\r\n"), (LPCTSTR)pDataPair->m_key);
		pSection = pDataPair->m_value;
		q = pSection->GetStartPosition();
		while(q)
		{
			pSectionPair = pSection->GetNext(q);
			::_ftprintf(fp, _T("%s = %s\r\n"), (LPCTSTR)pSectionPair->m_key, (LPCTSTR)pSectionPair->m_value);
		}
		::_ftprintf(fp, _T("\r\n"));
	}
	fclose(fp);
	return TRUE;
}


BOOL XIni::SetStr(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szValue)
{
	SectionMap* pSection = FindOrMakeSection(szSection);

	(*pSection)[CString(szKey)] = szValue;

	return TRUE;
}

BOOL XIni::SetInt(LPCTSTR szSection, LPCTSTR szKey, int nValue)
{
	CString str;
	str.Format(_T("%d"), nValue);
	return SetStr(szSection, szKey, str);
}

