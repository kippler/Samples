#include "stdafx.h"
#include "XString.h"
#include <malloc.h>


XString operator+(const XString& left, const XString& right)
{
	return XString(left, right);
}

wchar_t* XString::EMPTY_STRING = L"";



XString::XString()
{
	m_str = EMPTY_STRING;
	m_size = m_len = 0;
}

void XString::STRCPY(wchar_t* dst, const wchar_t* src, int len)
{
	memcpy(dst, src, len * sizeof(wchar_t));
	dst[len] = 0;
}

XString::XString(const wchar_t* str)
{
	m_str = EMPTY_STRING;
	m_size = m_len = 0;
	_CopyString(str);
}

XString::XString(const wchar_t* str, int count)
{
	m_str = EMPTY_STRING;
	m_size = m_len = 0;
	_CopyString(str, count);
}

XString::XString(const XString& rhs)
{
	m_str = EMPTY_STRING;
	m_size = m_len = 0;
	_CopyString(rhs);
}

XString::XString(const XString& str1, const XString& str2)
{
	m_str = EMPTY_STRING;
	m_size = m_len = 0;

	int newLen = str1.m_len + str2.m_len;
	if (_AllocSize(newLen+1) == FALSE) return;
	m_len = newLen;
	STRCPY(m_str, str1.m_str, str1.m_len);
	STRCPY(m_str+ str1.m_len, str2.m_str, str2.m_len);
}

const XString& XString::operator=(const wchar_t* str)
{
	_CopyString(str);
	return *this;
}

const XString& XString::operator=(const XString& rhs)
{
	_CopyString(rhs);
	return *this;
}

const XString& XString::operator+=(const wchar_t* str)
{
	_AddString(str);
	return *this;
}


const XString& XString::operator+=(const XString& rhs)
{
	_AddString(rhs);
	return *this;
}

const XString& XString::operator+=(const wchar_t c)
{
	_AddChar(c);
	return *this;
}

void XString::_FreeString()
{
	if (m_str && m_str!= EMPTY_STRING)
		free(m_str);
	m_str = EMPTY_STRING;
	m_len = 0;
	m_size = 0;
}
void XString::_CopyString(const wchar_t* str)
{
	_FreeString();
	if (str == NULL) return;
	int newLen = (int)wcslen(str);
	if (_AllocSize(newLen + 1) == FALSE) return;
	STRCPY(m_str, str, newLen);
	m_len = newLen;
}

void XString::_CopyString(const wchar_t* str, int count)
{
	_FreeString();
	if (str == NULL) return;
	if (_AllocSize(count + 1) == FALSE) return;
	STRCPY(m_str, str, count);
	m_len = count;
}

void XString::_CopyString(const XString& rhs)
{
	if (&rhs == this)
		return;
	_FreeString();
	if (rhs.m_str == NULL) return;
	int newLen = rhs.m_len;
	if (_AllocSize(newLen+1) == FALSE) return;
	STRCPY(m_str, rhs.m_str, newLen);
	m_len = newLen;
}


XString::~XString()
{
	_FreeString();
}

void XString::_AddString(const wchar_t* str)
{
	if (str == NULL) return;
	int len = (int)wcslen(str);
	if (_Reserve(m_len + len + 1) == FALSE) return;
	STRCPY(m_str + m_len, str, len);
	m_len += len;
}

void XString::_AddString(const XString& rhs)
{
	if (_Reserve(m_len + rhs.m_len + 1) == FALSE) return;
	STRCPY(m_str + m_len, rhs.m_str, rhs.m_len);
	m_len += rhs.m_len;
}

void XString::_AddChar(wchar_t c)
{
	int len = 1;
	if (_Reserve(m_len + len + 1) == FALSE) return;
	STRCPY(m_str + m_len, &c, len);
	m_len += len;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         지정한 크기만큼 alloc
/// @param  
/// @return 
/// @date   Tue Apr 19 17:56:44 2016
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XString::_AllocSize(int size)
{
	ASSERT(m_str == NULL || m_str== EMPTY_STRING);
	m_size = size;
	m_str = (wchar_t*)malloc(m_size * sizeof(wchar_t));
	if (m_str == NULL)
	{
		ASSERT(0);
		m_str = EMPTY_STRING;
		m_len = m_size = 0;
		return FALSE;
	}
	return TRUE;
}

BOOL XString::_Realloc(int newSize)
{
	ASSERT(m_size < newSize);
	wchar_t* newStr = (wchar_t*)realloc(m_str, newSize * sizeof(wchar_t));
	if (newStr == NULL){ASSERT(0); return FALSE;}
	m_str = newStr;
	m_size = newSize;
	return TRUE;
}


BOOL XString::_Reserve(int minSize)
{
	if (minSize <= m_size)		// 이미 충분하다.
		return TRUE;
	return _Realloc(minSize);
}


wchar_t* XString::GetBuffer(int minSize)
{
	if (_Reserve(minSize) == FALSE) { ASSERT(0); return EMPTY_STRING; }
	return m_str;
}

BOOL XString::ReleseBuffer()
{
	//if(m_str==NULL) { ASSERT(0); return FALSE; }
	int newLen = (int)wcslen(m_str);
	if(newLen+1>m_size) { ASSERT(0); return FALSE; }	// 버퍼 오버플로우
	m_len = newLen;
	return TRUE;
}

wchar_t* XString::GetLastBuffer()
{
	return m_str ? m_len ? (m_str + m_len - 1) : NULL : NULL;
}

XString XString::Mid(int first, int count)
{
	XString str;
	if (first >= m_len)
		return str;
	count = min(count, m_len - first);
	str._CopyString(m_str + first, count);
	return str;
}


XString	XString::Left(int count)
{
	XString str;
	count = min(count, m_len);
	str._CopyString(m_str, count);
	return str;
}


int XString::Compare(const wchar_t* str) const
{
	return wcscmp(m_str, str);
}

int XString::Compare(const XString& str) const
{
	return wcscmp(m_str, str.m_str);
}

void XString::Empty()
{
	_FreeString();
}
