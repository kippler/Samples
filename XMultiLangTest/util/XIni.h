////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// Ini file read & writer
///
/// ���̼��� : zlib license (http://www.kippler.com/etc/zlib_license/ ����)
/// 
/// @author   parkkh
/// @date     Monday, February 02, 2009  9:37:53 AM
/// 
/// Copyright(C) 2009 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// Number bases
#define BASE_BINARY			2
#define BASE_OCTAL			8
#define BASE_DECIMAL		10
#define BASE_HEXADECIMAL	16

#include <atlcoll.h>

// VS2008 ���Ϸ� �������Ҷ��� �Ʒ� �ּ��� ����
//#define UNDER_VS2008

#ifdef UNDER_VS2008
	#ifndef _In_
		#define _In_
	#endif
	#define CAtlString	CString
#endif

#define DEFAULT_TEXT_SECTION	_T("TEXT")

/// Modified string element trait which does case insensitive comparison
class CStringElementTraitsX : public CStringElementTraits< CString >
{
public:
    static ULONG __cdecl Hash( _In_ INARGTYPE str )
    {
        ATLENSURE( str != NULL );
        ULONG nHash = 0;
        const CString::XCHAR* pch = str;
        while( *pch != 0 )
        {
            CString::XCHAR ch = *pch++;
            if (_T('A') <= ch && ch <= _T('Z'))
                ch += (TCHAR)_T('a') - (TCHAR)_T('A');
            nHash = (nHash<<5)+nHash+ch;
        } 

        return( nHash );
    } 

    static bool __cdecl CompareElements( _In_ INARGTYPE str1, _In_ INARGTYPE str2 )
    {
        return( _tcsicoll( str1, str2 ) == 0 );
    }
}; 

class XIni  
{
public:
	XIni();
	XIni(LPCTSTR szPathName);
	~XIni();

	BOOL	Open(LPCTSTR szPathName);
	BOOL	OpenMemory(LPCTSTR szData, int nStrLen);
	BOOL	OpenMemory(const BYTE* pData, int nDataLen);

	BOOL	IsOpened() { return m_bOpen; }
	void	Clear();

	int		GetInt(LPCTSTR szSection, LPCTSTR szKey, int nDefault=0, int nBase=BASE_DECIMAL);
	BOOL	GetInt2(LPCTSTR szSection, LPCTSTR szKey, int& result, int nDefault=0, int nBase=BASE_DECIMAL);

	CString	GetStr(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szDefault=_T(""));
	BOOL	GetStr2(LPCTSTR szSection, LPCTSTR szKey, CString& sResult, LPCTSTR szDefault=_T(""));
	void	SetCodePage(int nCodePage) { m_nCodePage = nCodePage; }
	BOOL	SetStr(LPCTSTR szSection, LPCTSTR szKey, LPCTSTR szValue);
	BOOL	SetInt(LPCTSTR szSection, LPCTSTR szKey, int nValue);

	BOOL	SaveAs(LPCTSTR szFilePathName);

public :
	typedef CAtlMap<CAtlString, CString, CStringElementTraitsX> SectionMap;
	typedef CAtlMap<CAtlString, SectionMap *, CStringElementTraitsX> DataMap;

private :
	// ������ Ÿ��
	enum LINE_TYPE
	{
		LINE_TYPE_SECTION,			// [SECTION] ����
		LINE_TYPE_DATA,				// A=B ����
		LINE_TYPE_REMARK,			// ;REMARK ����
		LINE_TYPE_EMPTY,			// ����..
		LINE_TYPE_SINGLELINE,		// �׳� ���� �ؽ�Ʈ ( �׳� = �޺κ��� �����Ǿ��ٰ� �����Ѵ�.. )
	};

private :
	void			Init();
	BOOL			ReadALine();
	BOOL			IsUnicodeFileHeader(const BYTE* buf);
	LINE_TYPE		GetLineType(CString sLine);
	SectionMap*		FindSection(LPCTSTR szSection);
	SectionMap*		FindOrMakeSection(LPCTSTR szSection);
	CString			ParseSection(CString sLine);
	BOOL			ParseData(CString sLine, CString& sKey, CString& sStrVal);
	LPCTSTR			SkipWhiteSpace(LPCTSTR p);
	LPSTR			Unicode2Ascii(LPCWSTR szInput, int nCodePage, int& asciiLen);
	LPWSTR			Ascii2Unicode(LPCSTR szInput, int nCodePage);

private :
	DataMap			m_DataMap;
	CString			m_strCurSection;
	SectionMap		*m_pCurSection;
	SectionMap		*m_pTextSection;
	BOOL			m_bUnicodeIniFile;					// �����ڵ� ���� ����ΰ�?
	int				m_nCodePage;						// �����ڵ� ������ ���¿���, ASCII ����� ��� �ڵ�������
	BOOL			m_bAutoSave;						// ���°� ���Ҷ����� �����ұ�?
	CString			m_sLine;							// ����..
	BOOL			m_bModified;						// �����Ǿ���?
	BOOL			m_bOpen;							// ���ȳ�?
	LPCTSTR			m_pDataCurrent;						// ���� ó������ ����Ÿ ��ġ
	int				m_nStrLen;							// ���ڿ��� ũ��..
	int				m_nStrPos;							// ó���� ĳ���� ī��Ʈ
};

