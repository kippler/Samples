#include "stdafx.h"
#include "XIni.h"

////////////////////////////////////////////////////////////////////
//
// 제작자 : parkkh
// 수정자 : 서광현 (gline123@naver.com)
// 수정일 : 2012. 1. 6 
//
// History:
// 2012/01/06
//    1. INI 파일로 부터 읽은 Section name을 Linked List로 구현하고
//       Section 내의 Key와 Value도 Linked List로 구현한 것을
//       검색 속도를 향상하기 위하여 Hash Table을 이용하도록 수정함.
//
//    2. Key나 Value의 값을 "HELLO" = "WORLD" 와 같이 이중인용부호만
//       사용할 수 있도록 한 것을 'HELLO' = 'WORLD' 도 사용할 수
//       있도록 함, 예를 들어 "ABC"DEF" 와 같은 값이 있는 경우
//       기존에는 "ABC" 내의 값인 ABC만 인식하게 되는데 이 경우에
//       'ABC"DEF'로 하면 ABC"DEF를 모두 인식할 수 있음
//
//    3. 동일 Section에서 연속적으로 찾을 경우 Section을 찾기위해
//       Hash Table을 찾지 않고 바로 얻을 수 있도록 함.
//
//    4. TEXT는 Hash Table을 찾지 않고 바로 얻을 수 있도록 함
//
//    5. 단점: 읽은 INI 파일을 SaveAs()로 저장할 경우 Hash Table의 것을
//       저장하기 때문에 원본 INI의 내용과 순서가 서로 다를 수 있음
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
///          .ini 파일 열기
/// @param   szPathName  
/// @return  
/// @date    2006-11-08 오전 11:01:14
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XIni::Open(LPCTSTR szPathName)
{
	// 이전 데이타는 지우고.
	Clear();

	BYTE*		pData;							// 데이타 내용
	int			nDataLen;						// 데이타 크기
	HANDLE		hFile;							// 파일 핸들
	DWORD		read;

	// 파일 열기
	hFile = ::CreateFile(szPathName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if(hFile==INVALID_HANDLE_VALUE) {ASSERT(0); return FALSE; }

	// 메모리 잡기.
	nDataLen = ::GetFileSize(hFile, NULL);			// 파일 크기
	if(nDataLen>1000*1000) {ASSERT(0); CloseHandle(hFile); return FALSE;}	// 너무 크다!!!
	if(nDataLen==0) {CloseHandle(hFile); return TRUE;}						// 0 바이트이다..
	pData = (BYTE*)calloc(1, nDataLen+sizeof(WCHAR));
	if(pData==NULL){ASSERT(0); CloseHandle(hFile); return FALSE;}

	// 파일 읽기
	if(::ReadFile(hFile, pData, nDataLen, &read, NULL)==FALSE || read!=(DWORD)nDataLen)
	{
		ASSERT(0);
		free(pData);
		CloseHandle(hFile);
		return FALSE;
	}

	// 메모리 열기로 처리
	BOOL ret;
	ret = OpenMemory(pData, nDataLen);

	// 해제
	CloseHandle(hFile);
	free(pData);

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///          메모리로 ini 내용을 읽는다... (BOM 이 있을경우 유니코드 처리 가능)
///          데이타 내용의 끝이 반드시 NULL로 끝나야 한다.!! -> 없어도 되도록 수정했따..
/// @param   pData  
/// @param   nDataLen  
/// @return  
/// @date    2006-11-20 오후 3:21:09
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XIni::OpenMemory(const BYTE* pData, int nDataLen)
{
	if(nDataLen<LEN_OF_BOM) {ASSERT(0); return FALSE;}

	LPCTSTR		pNewData = NULL;
	LPCTSTR		szData;
	int			nStrLen = 0;									// 문자열 카운트

	if(IsUnicodeFileHeader(pData))								// 유니코드 파일에서 읽은 데이타 인가?
	{
#ifdef _UNICODE													// unicode -> unicode
		szData = (LPCTSTR)(pData+LEN_OF_BOM);					// bom 빼기
		nStrLen = (nDataLen-LEN_OF_BOM) / sizeof(WCHAR);
#else															// unicode -> ascii
		pNewData = Unicode2Ascii((LPCWSTR)(pData+LEN_OF_BOM), m_nCodePage, nStrLen);
		if(pNewData==NULL){ASSERT(0); return FALSE;}
		szData = pNewData;
#endif
	}
	else														// BOM 이 없다? == ASII
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

	// 문자열로 데이타 열기
	m_bOpen = OpenMemory(szData, nStrLen);

	// 메모리 해제.
	if(pNewData) free((void*)pNewData);

	return m_bOpen;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          메모리로 열기 - 끝이 NULL 로 안끝나도 된다..
/// @param   szData  
/// @return  
/// @date    2006-11-20 오후 4:16:02
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
			if(sSectionName.IsEmpty())				// 문제가 있는 섹션!
			{
				ASSERT(0);
				pCurSection = NULL;					// 에러 상태로 만든다.
			}
			else
			{
				pCurSection = FindOrMakeSection(sSectionName);	// 현재 섹션 바꾸기.
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
				ASSERT(0);						// 에러 상황
			}
			else
			{
				if(ParseData(m_sLine, sKey, sStrVal)==FALSE)
				{
					ASSERT(0);					// 에러 상황..
				}
				else
				{
					(*pCurSection)[sKey] = sStrVal;
				}
			}
		}
		else if(nLineType==LINE_TYPE_SINGLELINE)		// = 없이 그냥 한줄짜리
		{
			if(pCurSection==NULL)
			{
				ASSERT(0);						// 에러 상황
			}
			else
			{
				(*pCurSection)[m_sLine] = CString(""); // 키값만 넣는다.
			}
		}
		else
		{
			// todo : 주석처리, 빈칸처리, 기타 등등 처리..
		}
	}

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///          내부 데이타 다 지우기.
/// @return  
/// @date    2006-11-08 오전 10:56:45
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
///          한줄 읽어서 결과를 m_sLine 에 저장하기
/// @return  EOF 이면 FALSE
/// @date    2006-11-08 오전 11:04:47
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

	// 라인의 공백은 인정하지 않는다!!!!!!
	m_sLine.TrimLeft(); 
	m_sLine.TrimRight();

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          BOM 체크하기.
/// @param   buf  
/// @return  
/// @date    2006-11-08 오전 11:06:04
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XIni::IsUnicodeFileHeader(const BYTE* buf)
{
	if(buf[0]==0xff && buf[1]==0xfe) return TRUE;	// little endian BOM 만 처리한다.
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          한줄이 무슨 내용인감?
/// @param   sLine  
/// @return  
/// @date    2006-11-08 오후 1:35:36
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
///          "[HELLO]" 에서 "HELLO" 를 추출한다.
/// @param   sLine  
/// @return  실패시 "" 리턴
/// @date    2006-11-08 오후 1:46:14
////////////////////////////////////////////////////////////////////////////////////////////////////
CString XIni::ParseSection(CString sLine)
{
	CString sRet = sLine.Mid(1, sLine.GetLength()-2);
	sRet.TrimLeft(); sRet.TrimRight();
	return sRet;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          이름으로 섹션을 찾는다.
/// @param   sSectionName  
/// @return  
/// @date    2006-11-08 오후 2:17:52
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
///          SECTION NAME 을 가지고, 섹션을 만들던가, 기존 섹션을 찾는다..
/// @param   sSectionName  
/// @return  
/// @date    2006-11-08 오후 1:45:27
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
/// HELLO=WORLD 와 같은 문장을 파싱한다.
///
/// 아래와 같은 형태도 파싱 가능하다.
///   1) HELLO=WORLD 
///   2) "HELLO"="WORLD"
///   3) HELLO=WORLD ; REMARK
///
/// @param   data  
/// @return  
/// @date    2006-11-08 오후 1:52:01
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XIni::ParseData(CString sLine, CString& sKey, CString& sStrVal)
{
	const TCHAR* p = sLine;
	CHAR chQuot;

	sKey.Empty();
	sStrVal.Empty();

	// LEFT 파싱
	chQuot = (*p=='"' || *p=='\'') ? *p : '\0';
	if(chQuot) p++;
	while(*p)
	{
		if(*p == chQuot) { p++; break; }// "로 끝났다.
		if(!chQuot && *p=='=') break;	// =를 찾았다.
		sKey += *p;
		p++;
	}
	if(*p==0) {ASSERT(0); return FALSE;}	// 파싱 실패

	if(!chQuot)							// 따옴표가 없으면
	{ sKey.TrimLeft(); sKey.TrimRight();}

	if(sKey.IsEmpty()) {ASSERT(0); return FALSE;}		// 왼쪽이 공백이다! 에러

	// "aa" = "b" 에서 = 이전의 공백 건너 띄기
	p = SkipWhiteSpace(p);
	if(*p!='=') {ASSERT(0); return FALSE;}					// = 가 없다?
	p++;					// = 건너 띄기

	// = 다음의 공백 건너 띄기
	p = SkipWhiteSpace(p);

	// RIGHT 파싱
	chQuot = (*p=='"' || *p=='\'') ? *p : '\0';
	if(chQuot) p++;
	while(*p)
	{
		if(*p == chQuot) break;			// "로 끝났다.
		if(!chQuot && *p==';') break;	// ;를 찾았다.
		sStrVal+= *p;
		p++;
	}
	if(!chQuot)							// 따옴표가 없으면
	{ sStrVal.TrimLeft(); sStrVal.TrimRight();}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          공백 건너 띄기
/// @param   p  
/// @return  
/// @date    2006-11-08 오후 2:02:03
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
///          INT 값을 찾아서 가져온다.
/// @param   szSection  
/// @param   szKey  
/// @param   nDefault  
/// @param   BASE_DECIMAL  
/// @return  
/// @date    2006-11-08 오후 2:16:16
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
///          STRING 값을 찾는다.
/// @param   szSection  
/// @param   szKey  
/// @param   szDefault  
/// @return  
/// @date    2006-11-08 오후 3:04:51
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

	asciilen = wlen;					// asciilen 은 wlen 보다 크지 않다.
	ret = (LPSTR)malloc(asciilen);		// ASCII 용 버퍼
	if(ret==NULL) {ASSERT(0); return NULL;}
	asciilen = WideCharToMultiByte(nCodePage, 0, szInput,  -1, ret, asciilen, NULL, NULL);
	if(asciilen==0)	// UCS2->ascii
	{ASSERT(0); free(ret); return NULL;}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///          ASCII TO UNICODE
/// @return  결과값은 반드시 free() 로 해제
/// @date    2006-07-20 오후 3:50:40
////////////////////////////////////////////////////////////////////////////////////////////////////
LPWSTR XIni::Ascii2Unicode(LPCSTR szInput, int nCodePage)
{
	LPWSTR ret;
	int	   ucs2len = int(strlen(szInput)+sizeof(CHAR));
	ret = (LPWSTR)malloc(ucs2len*sizeof(WCHAR));				// UCS2 용 버퍼 잡기
	if(ret==NULL){ASSERT(0); return NULL;}

	if(MultiByteToWideChar(nCodePage, 0, szInput, -1, ret, ucs2len)==0)	// MBCS -> UCS2
	{ASSERT(0); free(ret); return NULL;}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         파일로 저장하기.
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


	// bom 쓰기
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

