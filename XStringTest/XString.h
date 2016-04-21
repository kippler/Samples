////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// ���ڿ� Ŭ���� - copy on write �� ������� �ʴ� ������ ���ڿ� Ŭ����
/// 
/// @author   park
/// @date     04/19/16 17:00:49
/// 
/// Copyright(C) 2016 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once

class XString
{
public :
	XString();
	XString(const wchar_t* str);
	XString(const wchar_t* str, int count);
	XString(const XString& rhs);
	XString(const XString& str1, const XString& str2);
	~XString();

	const XString& operator=(const wchar_t* str);
	const XString& operator=(const XString& rhs);
	const XString& operator+=(const wchar_t* str);
	const XString& operator+=(const XString& rhs);
	const XString& operator+=(const wchar_t c);

	operator const wchar_t *() const { return m_str; }

	friend XString operator+(const XString& left, const XString& right);
	const wchar_t* GetString()const  { return m_str;  }

	wchar_t*	GetBuffer(int minSize=0);
	wchar_t*	GetLastBuffer();				// ������ ���� ��ġ ����
	BOOL		ReleseBuffer();
	int			GetLength() const { return m_len; }

	XString		Mid(int first) { return Mid(first, m_len - first); }
	XString		Mid(int first, int count);
	XString		Left(int count);

	int			Compare(const XString& str) const;
	int			Compare(const wchar_t* str) const;
	void		Empty();

private :
	void		_AddString(const wchar_t* str);
	void		_AddString(const XString& rhs);
	void		_AddChar(wchar_t c);
	void		STRCPY(wchar_t* dst, const wchar_t* src, int len);
	void		_FreeString();
	void		_CopyString(const wchar_t* str);
	void		_CopyString(const wchar_t* str, int count);
	void		_CopyString(const XString& rhs);

private :		// �޸� ���� �޽�� - �޸� ���� �޽��� len ������ �ǵ帮�� �ʴ´�.
	BOOL		_AllocSize(int size);
	BOOL		_Realloc(int newSize);
	BOOL		_Reserve(int minSize);

private :
	wchar_t*	m_str;
	int			m_len;		// ��ȿ ���ڼ�
	int			m_size;		// ���� ũ��(���ڼ� ����)

private :
	static wchar_t* EMPTY_STRING;
};


inline bool	operator==(const XString& lhs, const XString& rhs) { return (lhs.GetLength() == rhs.GetLength() && lhs.Compare(rhs) == 0) ? true : false; }
inline bool	operator==(const XString& lhs, const wchar_t* rhs) { return lhs.Compare(rhs) == 0 ? true : false; }
inline bool	operator==(const wchar_t* rhs, const XString& lhs) { return lhs.Compare(rhs) == 0 ? true : false; }

inline bool	operator!=(const XString& lhs, const XString& rhs) { return (lhs.GetLength() != rhs.GetLength() || lhs.Compare(rhs) != 0) ? true : false; }
inline bool	operator!=(const XString& lhs, const wchar_t* rhs) { return lhs.Compare(rhs) != 0 ? true : false;}
inline bool	operator!=(const wchar_t* rhs, const XString& lhs) { return lhs.Compare(rhs) != 0 ? true : false;}


