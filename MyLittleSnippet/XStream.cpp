#include "stdafx.h"
#include "xcore/XStream.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// 
/// READ STREAM
/// 
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XFileReadStream::XFileReadStream()
{
	InitMembers();
}

XFileReadStream::~XFileReadStream()
{
	if(m_bAutoCloseHandle && m_handle != INVALID_HANDLE_VALUE) 
	{ 
		CloseHandle(m_handle); 
		m_handle = INVALID_HANDLE_VALUE; 
	}
}


void XFileReadStream::InitMembers()
{
	m_left = m_right = m_buf;
	m_virtPos = 0;
	m_handle = INVALID_HANDLE_VALUE;
	m_size = 0;
	m_bAutoCloseHandle = FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         ���Ͽ��� �б�
/// @param  
/// @return 
/// @date   Monday, October 12, 2015  2:37:08 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XFileReadStream::Read(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead)
{
	BYTE*	userBuffer = (BYTE*)lpBuffer;
	BOOL	ret = TRUE;
	int		remain2copy = nNumberOfBytesToRead;
	DWORD&  read_tot = *lpNumberOfBytesRead;

	read_tot = 0;
	if(m_handle == INVALID_HANDLE_VALUE ){ASSERT(0); return FALSE;}

	while(remain2copy)
	{
		// ���ۿ� ����Ÿ ������... �����ϱ�
		if (m_left < m_right)
		{
			int valid_buf_len = (int)(m_right - m_left);
			int to_copy = min(valid_buf_len, (int)remain2copy);

			// �̰Ŵ� ��°�� �����ϱ�
			memcpy(userBuffer, m_left, to_copy);
			userBuffer += to_copy;
			m_left += to_copy;
			m_virtPos += to_copy;
			read_tot += to_copy;
			remain2copy -= to_copy;
		}
		else// ���� ���۰� �����. ���� ����.
		{
			DWORD dwRead = 0;
			
			// �о���� ũ�Ⱑ ���� ���ۺ��� ū ��� - ���� ���ۿ� ���� �־��ְ� ������.
			if(remain2copy>=LEN_BUF)
			{
				// ���� ���۴� �ϴ� ������
				m_right = m_left = m_buf;

				if(::ReadFile(m_handle, userBuffer, remain2copy, &dwRead, NULL)==FALSE)
				{
					if(dwRead!=0) ASSERT(0);			// �߻� �Ұ��� �˰� ����
					ASSERT(0); 
					ret = FALSE;
					break;
				}
				m_virtPos += dwRead;
				read_tot += dwRead;
				remain2copy -= dwRead;
			}
			else	// ���� ���ۿ� �о���̱�
			{
				// ���� ��ġ�� ���� ���۸� ���Ľ��Ѽ� �д´�.
				// ���� ��� ���� ��ġ�� 1 �̳� 8193 �̶�� 8192 ��� (8192-1) ��ŭ�� �д´�.
				int toread = LEN_BUF - (max(0,m_virtPos) % LEN_BUF);

				if(::ReadFile(m_handle, m_buf, toread, &dwRead, NULL)==FALSE)
				{
					ASSERT(0);
					ret = FALSE;
					break;
				}
				m_left = m_buf;
				m_right = m_buf + dwRead;
			}

			if(dwRead==0)		// EOF!
				break;
		}
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         ��ġ�� ��/�ڷ� �̵��Ҷ� ���� ���۹��� �̳���� ���� ���۸� �״�� Ȱ���Ѵ�.
/// @param  
/// @return 
/// @date   Thursday, October 15, 2015  2:04:47 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
INT64 XFileReadStream::SetPos(INT64 pos)
{
	INT64	distance = pos - m_virtPos;		// �̵��� �Ÿ�

	if(distance==0) 
		return m_virtPos;					// �ٲ�°� ����

	if(distance>0)							// ������ ����
	{
		int buf_remain = (int)(m_right - m_left);
		if(buf_remain>=distance)			// ���۰� �����ִ� ��� ( buf_remain==distance �̸� ���� ���۴� 0 �̴�)
		{
			m_left += distance;				// ������ OK
			m_virtPos += distance;
			return m_virtPos;			
		}
	}
	else if(distance<0)						// �ڷ� ����
	{
		int buf_passed = (int)(m_left - m_buf);	// ���� ���ۿ��� ����� ����
		if(buf_passed>=-distance)			// ���� ���ο��� ó�� ������ ���
		{
			m_left += distance;				// ������ OK
			m_virtPos += distance;
			return m_virtPos;			
		}
	}

	// ���۴� �� ������
	m_left = m_right = m_buf;

	LARGE_INTEGER li;
	li.QuadPart = pos;
	DWORD ret = ::SetFilePointer(m_handle, li.LowPart, &li.HighPart, FILE_BEGIN);	// ��¥�� ���� �̵�

	if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
	{
		ASSERT(0);
		m_virtPos = -1;
	}
	else
	{
		m_virtPos = pos;
		if(pos!=ret) ASSERT(0);		// Ȯ�ο�
	}

	return m_virtPos;
}


INT64 XFileReadStream::GetPos()
{
	return m_virtPos;			// ���� ��ġ�� �˷��ش�.
}

INT64 XFileReadStream::GetSize() 
{
	//if(m_size>=0) return m_size;			�̰� ���� ������� ������, ������ Ŀ���� ��쵵 ������ ������...
	LARGE_INTEGER size;
	BOOL ret = ::GetFileSizeEx(m_handle, &size);
	if(ret==FALSE) { ASSERT(0); return -1;}
	m_size = size.QuadPart;
	return m_size;
}


BOOL XFileReadStream::Open(LPCWSTR filePathName)
{
	// �ݱ� ���� ȣ�� �ʿ�
	if(m_handle!=INVALID_HANDLE_VALUE) {ASSERT(0); return FALSE;}

	InitMembers();

	// ����
	m_handle = CreateFileW(filePathName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	// ���� ����?
	if(m_handle==INVALID_HANDLE_VALUE)
	{
		DWORD err  = ::GetLastError();

		// ���ϸ��� �ʹ� �涧? - UNC �� �ٿ��� �ٽ� �����.
		if(err==ERROR_PATH_NOT_FOUND)
		{
			CStringW newPathName = CStringW(L"\\\\?\\") + filePathName;
			m_handle = CreateFileW(newPathName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
		}
		// access ����?
		else if(err==ERROR_SHARING_VIOLATION ||  err==ERROR_LOCK_VIOLATION)
		{
			// FILE_SHARE_READ �� ���� �����.
			m_handle = CreateFileW(filePathName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

			// �׷��� ����? - share �� read, write �Ѵ� �ְ� �����.
			if(m_handle==INVALID_HANDLE_VALUE)
			{
				m_handle = CreateFileW(filePathName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				err = ::GetLastError();
			}
			// �׷��� ����? 
			if(m_handle==INVALID_HANDLE_VALUE)
			{
				m_handle = CreateFileW(filePathName, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				err = ::GetLastError();
			}
		}

		// ���� ����
		if(m_handle==INVALID_HANDLE_VALUE)  {ASSERT(0); return FALSE;}
	}

	m_bAutoCloseHandle = TRUE;
	return TRUE;
}

void	XFileReadStream::Close()
{
	if(m_handle == INVALID_HANDLE_VALUE) return;
	CloseHandle(m_handle); 
	InitMembers();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         ������ ��ä�� �о XBuffer �� ����
/// @param  
/// @return 
/// @date   Mon Feb 22 18:00:43 2016
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XFileReadStream::ReadWhole(XBuffer& buffer, BOOL addNull)
{
	INT64 fileSize = GetSize();
	int size = (int)fileSize + (addNull ? 2 : 0);
	if (buffer.Alloc(size) == FALSE) return FALSE;
	SetPos(0);
	DWORD read;
	if (Read(buffer.data, (int)fileSize, &read) == FALSE || fileSize != read) return FALSE;
	if (addNull)				// ���ڿ� �Ľ̽� ó���� �����ϱ� ���ؼ� ���� null �ϳ� �־��ֱ�.
	{
		buffer.data[buffer.allocSize - 1] = 0;
		buffer.data[buffer.allocSize - 2] = 0;
	}
	buffer.dataSize = (int)fileSize;
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         
///  �޸� ����
///         
////////////////////////////////////////////////////////////////////////////////////////////////////

XMemoryReadStream::XMemoryReadStream()
{
	m_buf = NULL;
	m_size = 0;
	m_pos = 0;
	m_bSelfFree = FALSE;
}

XMemoryReadStream::~XMemoryReadStream()
{
	Free();
}

BOOL XMemoryReadStream::Read(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead)
{
	*lpNumberOfBytesRead = 0;
	if(m_buf==NULL) {ASSERT(0); return FALSE;}

	int remain = m_size - m_pos;
	int to_copy = min(remain, (int)nNumberOfBytesToRead);

	if(to_copy<=0) return TRUE;			// EOF!

	memcpy(lpBuffer, m_buf + m_pos, to_copy);
	m_pos += to_copy;
	*lpNumberOfBytesRead = to_copy;

	return TRUE;
}

INT64 XMemoryReadStream::SetPos(INT64 pos)
{
	m_pos = (int)pos;
	return m_pos;
}

INT64 XMemoryReadStream::GetPos()
{
	return m_pos;
}

INT64 XMemoryReadStream::GetSize()
{
	return m_size;
}

BOOL XMemoryReadStream::Alloc(int size)
{
	BYTE* buf = (BYTE*)realloc(m_buf, size);
	if(buf==NULL) {ASSERT(0); Free(); return FALSE;}

	m_buf = buf;
	m_size = size;
	m_pos = 0;
	m_bSelfFree = TRUE;

	return TRUE;
}

void XMemoryReadStream::Free()
{
	// �ܺο��� ���۸� attch �� ���� free() ���� �ʴ´�.
	if(m_buf && m_bSelfFree)
		free(m_buf);

	m_buf = NULL;
	m_size = 0;
	m_pos = 0;
	m_bSelfFree = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         ���� ��� ��ä�� ���ۿ� �����ϱ�
/// @param  
/// @return 
/// @date   Thursday, October 15, 2015  1:19:41 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XMemoryReadStream::OpenFile(LPCWSTR filePathName)
{
	Free();

	XFileReadStream file;
	if(file.Open(filePathName)==FALSE) {ASSERT(0); return FALSE;}
	if(Alloc((int)file.GetSize())==FALSE) {ASSERT(0); return FALSE;}
	DWORD read;
	if(file.Read(m_buf, m_size, &read)==FALSE) {ASSERT(0); return FALSE;}

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         ����Ÿ attach �ϱ�
/// @param  
/// @return 
/// @date   Friday, October 16, 2015  2:44:05 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
void XMemoryReadStream::Attach(BYTE* buf, int size, BOOL bSelfFree)
{
	Free();

	m_buf = buf;
	m_size = size;
	m_pos = 0;
	m_bSelfFree = bSelfFree;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// 
/// WRITE STREAM
/// 
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



XFileWriteStream::XFileWriteStream()
{
	m_handle = INVALID_HANDLE_VALUE;
	m_bufpos = 0;
	m_virtPos = 0;
	m_bAutoCloseHandle = FALSE;
	m_filePathName = NULL;
	m_buf = (BYTE*)malloc(LEN_BUF);
}

XFileWriteStream::~XFileWriteStream()
{
	Close();
	free(m_buf);
}


BOOL	XFileWriteStream::Write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite)
{
	if(m_handle==INVALID_HANDLE_VALUE) {ASSERT(0); return FALSE;}

	BYTE*	userBuffer = (BYTE*)lpBuffer;
	int		userbuf_remain = nNumberOfBytesToWrite;
	int		buf_remain = LEN_BUF - m_bufpos;


	// ���� ���ۿ� �����ϰ� ��
	if(buf_remain>=(int)nNumberOfBytesToWrite)
	{
		memcpy(m_buf + m_bufpos, userBuffer, nNumberOfBytesToWrite);
		m_bufpos += nNumberOfBytesToWrite;
	}
	else	// ���� ���ۿ� �� ����ֱ� ������ ���
	{
		if(FlushBuffer()==FALSE)		// ���� ����
			return FALSE;

		// �Է¹��� ����Ÿ�� �״�� ���Ͽ� ����.
		DWORD written;
		if(::WriteFile(m_handle, userBuffer, nNumberOfBytesToWrite, &written, NULL)==FALSE ||
			written!=nNumberOfBytesToWrite)								// written!=nNumberOfBytesToWrite �� �߻����� �ʴ´�!
		{ASSERT(0); return FALSE;}
	}
	m_virtPos += nNumberOfBytesToWrite;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         ���� ���۸� �� ���Ϸ� ���� ���������.
/// @param  
/// @return 
/// @date   Thursday, October 15, 2015  2:38:16 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XFileWriteStream::FlushBuffer()
{
	if(m_handle!=INVALID_HANDLE_VALUE && m_bufpos)
	{
		DWORD written;
		if(::WriteFile(m_handle, m_buf, m_bufpos, &written, NULL)==FALSE || written!=m_bufpos)	// written!=m_bufpos �� �߻����� �ʴ´�.
		{ASSERT(0); return FALSE;}
	}

	m_bufpos = 0;
	return TRUE;
}

INT64	XFileWriteStream::SetPos(INT64 pos)
{
	// ������ �÷����ϰ� �̵�
	FlushBuffer();

	LARGE_INTEGER li;
	li.QuadPart = pos;
	DWORD ret = ::SetFilePointer(m_handle, li.LowPart, &li.HighPart, FILE_BEGIN);	// ��¥�� ���� �̵�

	if(ret==INVALID_SET_FILE_POINTER)
	{	
		DWORD err = ::GetLastError();	// �Ƹ��� ERROR_NEGATIVE_SEEK 
		//ASSERT(0);
	}
	else
		m_virtPos = pos;

	return m_virtPos;
}

INT64	XFileWriteStream::GetPos()
{
	return m_virtPos;
}

INT64	XFileWriteStream::GetSize()
{
	FlushBuffer();

	LARGE_INTEGER size;
	BOOL ret = ::GetFileSizeEx(m_handle, &size);
	if(ret==FALSE) { ASSERT(0); return -1;}
	return size.QuadPart;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         ����� ���� �����ϱ� 
/// @param  
/// @return 
/// @date   Friday, October 16, 2015  10:21:35 AM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XFileWriteStream::Open(LPCWSTR filePathName)
{
	Close();

	m_handle = CreateFileW(filePathName, GENERIC_WRITE, FILE_SHARE_READ  , NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_handle==INVALID_HANDLE_VALUE) 
	{
		DWORD error = ::GetLastError();

		// ���� ����� �ٽ� �غ���
		if(error==ERROR_ACCESS_DENIED)
		{
			::SetFileAttributesW(filePathName, FILE_ATTRIBUTE_NORMAL);
			::DeleteFileW(filePathName);

			m_handle = CreateFileW(filePathName, GENERIC_WRITE, FILE_SHARE_READ , NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if(m_handle==INVALID_HANDLE_VALUE) 
			{
				error = ::GetLastError();
				return FALSE;
			}
		}
	}

	m_filePathName = _wcsdup(filePathName);
	m_bAutoCloseHandle = TRUE;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         ���� �ڵ��� attach �ϱ�
/// @param  
/// @return 
/// @date   Friday, October 16, 2015  2:39:24 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
void XFileWriteStream::Attach(HANDLE hFile, BOOL bAutoCloseHandle)
{
	Close();
	m_handle = hFile;
	m_bAutoCloseHandle = bAutoCloseHandle;
}

void XFileWriteStream::Close()
{
	FlushBuffer();

	if(m_bAutoCloseHandle && m_handle != INVALID_HANDLE_VALUE) 
		CloseHandle(m_handle); 

	m_handle = INVALID_HANDLE_VALUE; 
	m_bufpos = 0;
	m_virtPos = 0;
	m_bAutoCloseHandle = FALSE;
	if(m_filePathName) free((void*)m_filePathName);
	m_filePathName = NULL;
}



////////////////////////////////////////////////////////////////////////////////////////////////////
///         
///         �޸� ���� ��Ʈ��
///         
////////////////////////////////////////////////////////////////////////////////////////////////////
XMemoryWriteStream::XMemoryWriteStream()
{
	m_buf = NULL;
	m_allocSize = 0;
	m_size = 0;
	m_pos = 0;
	m_bSelfFree = FALSE;
	m_maxAllocSize = MAX_ALLOC_SIZE;
}

XMemoryWriteStream::~XMemoryWriteStream()
{
	Free();
}

BOOL XMemoryWriteStream::Write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite)
{
	int buf_remain = m_allocSize - m_pos;

	// �޸𸮰� ������ ���.. alloc ȣ��
	if(buf_remain<(int)nNumberOfBytesToWrite)
		if(IncreaseBuffer(m_pos+nNumberOfBytesToWrite)==FALSE)	// m_pos �� m_allocSize ���� Ŭ �� �ֱ� ������, m_allocSize + nNumberOfBytesToWrite �� �ƴϴ�.
			return FALSE;

	// �޸� ������ OK
	memcpy(m_buf+m_pos, lpBuffer, nNumberOfBytesToWrite);
	m_pos += nNumberOfBytesToWrite;
	m_size = max(m_pos, m_size);			// SetPos �� ȣ��Ǿ��� �� �ֱ� ������ m_size �� m_pos �� max ������ ����Ѵ�.
	return TRUE;
}

INT64 XMemoryWriteStream::SetPos(INT64 pos)
{
	if(pos<0) {/*ASSERT(0); */return FALSE;}	// ������ �����Ѵ�.
	m_pos = (int)pos;
	return m_pos;
}

INT64 XMemoryWriteStream::GetPos()
{
	return m_pos;
}

INT64 XMemoryWriteStream::GetSize()
{
	return m_size;
}

void XMemoryWriteStream::Free()
{
	// �ܺο��� ���۸� attch �� ���� free() ���� �ʴ´�.
	if(m_buf && m_bSelfFree)
		free(m_buf);

	m_buf = NULL;
	m_allocSize = 0;
	m_size = 0;
	m_pos = 0;
	m_bSelfFree = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         targetSize �̻����� �޸𸮸� �ø���.
/// @param  
/// @return 
/// @date   Thursday, October 15, 2015  6:46:08 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XMemoryWriteStream::IncreaseBuffer(int targetSize)
{
	int newSize = max(INITIAL_ALLOC_SIZE, m_allocSize);

	// ���ο� ũ�� ����ϰ�
	while(newSize<targetSize)
	{
		newSize = newSize*2;
		if(newSize>m_maxAllocSize)
		{ASSERT(0); return FALSE;}
	}

	// �޸� realloc
	BYTE* buf = (BYTE*)realloc(m_buf, newSize);
	if(buf==NULL) {ASSERT(0); return FALSE;}

	// ���ϰ� �۵��� ����ϰ� �ϱ� ���ؼ�, realloc �� �κ��� 0 ���� ä���
	memset(buf+m_allocSize, 0, newSize - m_allocSize);


	m_buf = buf;
	m_allocSize = newSize;
	m_bSelfFree = TRUE;


	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         Write �ϱ��� ũ�⸦ �̸� �� ��� Alloc �ؼ� ����ϰ��� �� �� ���
/// @param  
/// @return 
/// @date   Thursday, October 15, 2015  6:50:17 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XMemoryWriteStream::Alloc(int size)
{
	Free();
	m_buf = (BYTE*)malloc(size);
	if(m_buf==NULL) {ASSERT(0); return FALSE;}
	m_allocSize = size;
	m_bSelfFree = TRUE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         �ܺ� �޸� attach - Free() ȣ��� free() ���� �ʴ´�.
/// @param  
/// @return 
/// @date   Friday, October 23, 2015  2:42:22 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
void XMemoryWriteStream::Attach(BYTE* buf, int len)
{
	Free();
	m_buf = buf;
	m_allocSize = len;
	m_size = 0;
	m_pos = 0;
}
