////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// ����/�޸� �� �������� �б�/���� ���ִ� ��Ʈ�� Ŭ����. ���� I/O �� ���۸� ó���� ���ش�.
/// 
/// @author   park
/// @date     10/08/15 17:30:07
/// 
/// Copyright(C) 2015 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef ASSERT
# include <atlbase.h>
# define ASSERT(x)  ATLASSERT(x)
#endif 

#ifndef PURE
# define PURE = 0
#endif

#define EOF    (-1)
class XBuffer;


////////////////////////////////////////////////////////////////////////////////////////////////////
///         �б� ��Ʈ�� �������̽�
/// @param  
/// @return 
/// @date   Thursday, October 08, 2015  5:31:15 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
struct XReadStream
{
	virtual ~XReadStream() {};
	virtual BOOL	Read(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead) PURE;
	virtual INT64	SetPos(INT64 pos) PURE;
	virtual INT64	GetPos() PURE;
	virtual INT64	GetSize() PURE;

	/////////////////////////////////////////////////////////////////
	//
	// ��ƿ �Լ�
	//

	// ���ϴ� ��ŭ �� ������ TRUE
	BOOL	Read(LPVOID buf, int len)
	{
		DWORD read;
		if(Read(buf, len, &read) && read==len) return TRUE;
		return FALSE;
	}
	// method �� ������ �̵��ϱ�
	INT64	SetPos(INT64 pos, DWORD method)
	{
		INT64 newPos;
		if(method==FILE_BEGIN) newPos = pos;
		else if(method==FILE_CURRENT) newPos = GetPos() + pos;
		else if(method==FILE_END) newPos = GetSize() + pos;
		else {ASSERT(0); return -1;}
		return SetPos(newPos);
	}
	BOOL	IsEOF()
	{
		return GetPos()>=GetSize() ? TRUE : FALSE;
	}
	long GetC()
	{
		BYTE b;
		return Read(&b, sizeof(BYTE))==FALSE ? EOF : b;
	}
	BOOL	ReadBYTE(BYTE& val)
	{
		return Read(&val, sizeof(BYTE));
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////
///         ���� ��Ʈ�� �������̽�
/// @param  
/// @return 
/// @date   Thursday, October 15, 2015  2:10:23 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
struct XWriteStream
{
	virtual ~XWriteStream() {};
	virtual BOOL	Write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite) PURE;
	virtual INT64	SetPos(INT64 pos) PURE;
	virtual INT64	GetPos() PURE;
	virtual INT64	GetSize() PURE;

	/////////////////////////////////////////////////////////////////
	//
	// ��ƿ �Լ�
	//
	// method �� ������ �̵��ϱ�
	INT64	SetPos(INT64 pos, DWORD method)
	{
		INT64 newPos;
		if(method==FILE_BEGIN) newPos = pos;
		else if(method==FILE_CURRENT) newPos = GetPos() + pos;
		else if(method==FILE_END) newPos = GetSize() + pos;
		else {ASSERT(0); return -1;}
		return SetPos(newPos);
	}
};


////////////////////////////////////////////////////////////////////////////////////////////////////
///         ���� ���۸��� �����ϴ� ���� �б� Ŭ����
/// @param  
/// @return 
/// @date   Thursday, October 08, 2015  5:33:54 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
class XFileReadStream : public XReadStream
{
public :
	XFileReadStream();
	virtual ~XFileReadStream();
	virtual BOOL	Read(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead);
	virtual INT64	SetPos(INT64 pos);
	virtual INT64	GetPos();
	virtual INT64	GetSize();

public :
	BOOL	Open(LPCWSTR filePathName);	// ���� ���� ����
	void	Close();
	BOOL	ReadWhole(XBuffer& buffer, BOOL addNull=FALSE);	// ��ü �� �о ���ۿ� �ֱ�

private :
	void	InitMembers();

public :
	enum { LEN_BUF = 8192 };
	HANDLE	m_handle;
	BYTE	m_buf[LEN_BUF];
	BYTE*	m_left;					// m_buf �� ���� ����Ÿ ���� ��ġ.
	BYTE*	m_right;				// m_buf �� ��ȿ�� ����Ÿ �� ��ġ-1

	INT64	m_virtPos;				// ������ ���� ������
	INT64	m_size;

	BOOL	m_bAutoCloseHandle;		// ����� �ڵ����� �ڵ� ������ ����.
};




////////////////////////////////////////////////////////////////////////////////////////////////////
///         �޸� ����
/// @param  
/// @return 
/// @date   Thursday, October 08, 2015  5:33:54 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
class XMemoryReadStream : public XReadStream
{
public :
	XMemoryReadStream();
	virtual ~XMemoryReadStream();
	virtual BOOL	Read(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead);
	virtual INT64	SetPos(INT64 pos);
	virtual INT64	GetPos();
	virtual INT64	GetSize();

public :
	BOOL	Alloc(int size);
	BOOL	OpenFile(LPCWSTR filePathName);
	void	Free();
	void	Attach(BYTE* buf, int size, BOOL bSelfFree=FALSE);

public :
	BYTE*	m_buf;			// �޸� ����
	int		m_size;			// ���� alloc ũ��
	int		m_pos;			// ���� �д� ��ġ
	BOOL	m_bSelfFree;	// ����� �ڵ����� �޸� �����ұ� ����
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// 
/// WRITE STREAM
/// 
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////
///         ���۸� ó���ϴ� ���� ���� Ŭ����
/// @param  
/// @return 
/// @date   Thursday, October 15, 2015  6:36:13 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
class XFileWriteStream : public XWriteStream
{
public :
	XFileWriteStream();
	virtual ~XFileWriteStream();
	virtual BOOL	Write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite);
	virtual INT64	SetPos(INT64 pos);
	virtual INT64	GetPos();
	virtual INT64	GetSize();

public :
	BOOL	Open(LPCWSTR filePathName);
	void	Attach(HANDLE hFile, BOOL bAutoCloseHandle=FALSE);
	void	Close();
	HANDLE	GetHandle() { return m_handle; }
	LPCWSTR GetFilePathName() { return m_filePathName; }

private :
	BOOL	FlushBuffer();

private :
	enum { LEN_BUF = 8192 };
	HANDLE	m_handle;
	BYTE*	m_buf;			// ���� ����
	int		m_bufpos;				// ���� ���ۿ��� ������� ũ��
	INT64	m_virtPos;				// ���� �������� ���� ��ġ (���� ��ġ + m_bufpos)
	BOOL	m_bAutoCloseHandle;		// ����� �ڵ����� �ڵ� ������ ����.
	LPCWSTR	m_filePathName;			// ���ϸ� ����ϱ�

};




////////////////////////////////////////////////////////////////////////////////////////////////////
///         �޸𸮷� ������ ���� ��Ʈ�� Ŭ����
/// @param  
/// @return 
/// @date   Thursday, October 15, 2015  6:41:45 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
class XMemoryWriteStream : public XWriteStream
{
public :
	XMemoryWriteStream();
	virtual ~XMemoryWriteStream();
	virtual BOOL	Write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite);
	virtual INT64	SetPos(INT64 pos);
	virtual INT64	GetPos();
	virtual INT64	GetSize();

public :
	BOOL	Alloc(int size);			// �̸� �� ũ�⸦ �ƴ� ��� ����Ѵ�.
	void	Free();
	void	SetMaxAlloc(int size) { m_maxAllocSize = size; }
	BYTE*	GetBuffer() { return m_buf; }
	void	Attach(BYTE* buf, int len);

private :
	BOOL	IncreaseBuffer(int targetSize);

private :
	enum	{ INITIAL_ALLOC_SIZE = 4096	};		// �ʱ� alloc ũ��
	enum	{ MAX_ALLOC_SIZE = 1024*1024*100 };	// �޸� alloc �ִ� ũ�� - �ϴ� 100MB

public :
	BYTE*	m_buf;			// �޸� ����
	int		m_allocSize;	// ���� alloc ũ��
	int		m_size;			// ���ۿ��� ������� ũ��; ���� ũ�⿡ �ش�ȴ�.
	int		m_pos;			// ���� ��ġ ; m_size ���� Ŀ�� �� �ִ�.
	BOOL	m_bSelfFree;	// ����� �ڵ����� �޸� �����ұ� ����
	int		m_maxAllocSize;	// �ִ� alloc �� ũ��
};




////////////////////////////////////////////////////////////////////////////////////////////////////
///         dummy output stream - ��� ������ ũ�⸦ ��길 �ϰ��� �Ҷ� ����Ѵ�.
///			pos �� size �� �������� ������ ���� �ʿ�.
/// @param  
/// @return 
/// @date   Friday, October 16, 2015  4:17:46 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
class XDummyWriteStream : public XWriteStream
{
public :
	XDummyWriteStream() {m_size = 0; m_pos = 0;}
	virtual ~XDummyWriteStream() {} 
	virtual BOOL	Write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite) { m_pos += nNumberOfBytesToWrite; m_size = max(m_pos, m_size); return TRUE; }
	virtual INT64	SetPos(INT64 pos) { m_pos = pos; return m_pos ; }
	virtual INT64	GetPos() { return m_pos; }
	virtual INT64	GetSize() { return m_size; }

	INT64 m_size;
	INT64 m_pos;

};



/////////////////////////////////////////////////////////////////////////////////
//
// XBUFFER - �����ϰ� ����Ʈ ���۸� �ְ� ���� �� ����Ѵ�.
//
// XAutoByte �� ���� �ٸ����̶�� �ϸ�, size �� alloc ����� ���� �ִٴ���?
//



class XBuffer
{
public:
	XBuffer()
	{
		data = NULL;
		dataSize = allocSize = 0;
	}
	~XBuffer() { if (data) free(data); }
	BOOL	Alloc(int count)
	{
		if (data && allocSize == count) return TRUE;	// �����ϴ�. �Ҳ� ����.
		if (data) free(data);
		data = (BYTE*)malloc(count*sizeof(BYTE));
		allocSize = count;
		dataSize = 0;
		return data ? TRUE : FALSE;
	}
	BOOL	Calloc(int count)
	{
		if (data) free(data);
		data = (BYTE*)calloc(count, sizeof(BYTE));
		allocSize = count;
		dataSize = 0;
		return data ? TRUE : FALSE;
	}
	BOOL	Write(BYTE* _data, int _size)
	{
		if (Alloc(_size) == FALSE) return FALSE;
		memcpy(data, _data, _size);
		dataSize = _size;
		return TRUE;
	}
	operator BYTE*() { return data; }

public:
	BYTE*	data;
	int		dataSize;		// ������ ��ȿ�� ����Ÿ ũ�� - ���� ũ�⸦ �ְ������ ����Ѵ�.
	int		allocSize;		// ������ alloc�� ũ��
};
