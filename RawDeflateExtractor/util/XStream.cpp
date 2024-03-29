﻿#include "stdafx.h"
#include "XStream.h"
#include <atlstr.h>

#pragma warning(disable: 26408)		// Avoid malloc() and free(), prefer the nothrow version of new with delete (r.10).
#pragma warning(disable: 26481 26486 26447)
#if (_MSC_VER == 1500 )		// 1500 == vs2008
#	define nullptr	NULL
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// 
/// READ STREAM
/// 
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XFileReadStream::XFileReadStream()
{
	m_buf = (BYTE*)malloc(LEN_BUF);
	m_left = m_right = m_buf;
	m_virtPos = 0;
	m_handle = INVALID_HANDLE_VALUE;
	m_size = 0;
	m_bAutoCloseHandle = FALSE;

	InitMembers();
}

XFileReadStream::~XFileReadStream()
{
	if(m_bAutoCloseHandle && m_handle != INVALID_HANDLE_VALUE) 
	{ 
		CloseHandle(m_handle); 
		m_handle = INVALID_HANDLE_VALUE; 
	}
	free(m_buf);
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
///         파일에서 읽기
/// @param  
/// @return 
/// @date   Monday, October 12, 2015  2:37:08 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XFileReadStream::Read(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead)
{
	if (m_handle == INVALID_HANDLE_VALUE || lpNumberOfBytesRead == nullptr || lpBuffer==nullptr) { ASSERT(0); return FALSE; }

	BYTE*	userBuffer = (BYTE*)lpBuffer;
	BOOL	ret = TRUE;
	int		remain2copy = nNumberOfBytesToRead;
	DWORD&  read_tot = *lpNumberOfBytesRead;

	read_tot = 0;

	while(remain2copy)
	{
		// 버퍼에 데이터 있으면... 복사하기
		if (m_left < m_right)
		{
			const int valid_buf_len = (int)(m_right - m_left);
			int to_copy = min(valid_buf_len, (int)remain2copy);

			// 이거는 통째로 복사하기
			memcpy(userBuffer, m_left, to_copy);
			userBuffer += to_copy;
			m_left += to_copy;
			m_virtPos += to_copy;
			read_tot += to_copy;
			remain2copy -= to_copy;
		}
		else// 내부 버퍼가 비었다. 직접 읽자.
		{
			DWORD dwRead = 0;
			
			// 읽어야할 크기가 내부 버퍼보다 큰 경우 - 유저 버퍼에 직접 넣어주고 끝낸다.
			if(remain2copy>=LEN_BUF)
			{
				// 내부 버퍼는 일단 날리고
				m_right = m_left = m_buf;

				if(::ReadFile(m_handle, userBuffer, remain2copy, &dwRead, nullptr)==FALSE)
				{
					if(dwRead!=0) ASSERT(0);			// 발생 불가로 알고 있음
					ASSERT(0); 
					ret = FALSE;
					break;
				}
				m_virtPos += dwRead;
				read_tot += dwRead;
				remain2copy -= dwRead;
			}
			else	// 내부 버퍼에 읽어들이기
			{
				// 현재 위치에 따라 버퍼를 정렬시켜서 읽는다.
				// 예를 들어 현재 위치가 1 이나 8193 이라면 8192 대신 (8192-1) 만큼만 읽는다.
				const int toread = LEN_BUF - (max(0,m_virtPos) % LEN_BUF);

				if(::ReadFile(m_handle, m_buf, toread, &dwRead, nullptr)==FALSE)
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
///         위치를 앞/뒤로 이동할 때 내부 버퍼범위 이내라면 내부 버퍼를 그대로 활용한다.
/// @param  
/// @return 
/// @date   Thursday, October 15, 2015  2:04:47 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
INT64 XFileReadStream::SetPos(INT64 pos)
{
	// 음수로 넘어가지 않는다.
	if (pos < 0)
		return m_virtPos;

	INT64	distance = pos - m_virtPos;		// 이동할 거리

	if(distance==0) 
		return m_virtPos;					// 바뀌는것 없음

	if(distance>0)							// 앞으로 전진
	{
		const int buf_remain = (int)(m_right - m_left);
		if(buf_remain>=distance)			// 버퍼가 남아있는 경우 ( buf_remain==distance 이면 남은 버퍼는 0 이다)
		{
			m_left += distance;				// 전진후 OK
			m_virtPos += distance;
			return m_virtPos;			
		}
	}
	else if(distance<0)						// 뒤로 후진
	{
		const int buf_passed = (int)(m_left - m_buf);	// 내부 버퍼에서 사용한 내역
		if(buf_passed>=-distance)			// 버퍼 내부에서 처리 가능한 경우
		{
			m_left += distance;				// 후진후 OK
			m_virtPos += distance;
			return m_virtPos;			
		}
	}

	// 버퍼는 다 버린다
	m_left = m_right = m_buf;

	LARGE_INTEGER li;
	li.QuadPart = pos;
	const DWORD ret = ::SetFilePointer(m_handle, li.LowPart, &li.HighPart, FILE_BEGIN);	// 진짜로 파일 이동

	if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
	{
		ASSERT(0);
		m_virtPos = -1;
	}
	else
	{
		m_virtPos = pos;
		if(pos!=ret) ASSERT(0);		// 확인용
	}

	return m_virtPos;
}


INT64 XFileReadStream::GetPos()
{
	return m_virtPos;			// 가상 위치를 알려준다.
}

INT64 XFileReadStream::GetSize() 
{
	//if(m_size>=0) return m_size;			이걸 쓰면 빠르기는 하지만, 파일이 커지는 경우도 있을수 있으니...
	LARGE_INTEGER size;
	const BOOL ret = ::GetFileSizeEx(m_handle, &size);
	if(ret==FALSE) { ASSERT(0); return -1;}
	m_size = size.QuadPart;
	return m_size;
}


BOOL XFileReadStream::Open(LPCWSTR filePathName)
{
	return Open(filePathName, FILE_SHARE_READ);
}

BOOL XFileReadStream::Open(LPCWSTR filePathName, DWORD dwShareMode)
{
	// 닫기 먼저 호출 필요
	if(m_handle!=INVALID_HANDLE_VALUE) {ASSERT(0); return FALSE;}

	InitMembers();

	// 열기
	m_handle = CreateFileW(filePathName, GENERIC_READ, dwShareMode, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
	// 열기 실패?
	if(m_handle==INVALID_HANDLE_VALUE)
	{
		DWORD err  = ::GetLastError();

		// 파일명이 너무 길때? - UNC 를 붙여서 다시 열어본다.
		if(err==ERROR_PATH_NOT_FOUND)
		{
			CStringW newPathName = CStringW(L"\\\\?\\") + filePathName;
			m_handle = CreateFileW(newPathName, GENERIC_READ, dwShareMode, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
		}
		// access 실패?
		else if(err==ERROR_SHARING_VIOLATION ||  err==ERROR_LOCK_VIOLATION)
		{
			// FILE_SHARE_READ 를 끄고 열어본다.
			m_handle = CreateFileW(filePathName, GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);

			// 그래도 실패? - share 를 read, write 둘다 주고 열어본다.
			if(m_handle==INVALID_HANDLE_VALUE)
			{
				m_handle = CreateFileW(filePathName, GENERIC_READ, dwShareMode | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
				err = ::GetLastError();
			}
			// 그래도 실패? 
			if(m_handle==INVALID_HANDLE_VALUE)
			{
				m_handle = CreateFileW(filePathName, GENERIC_READ, dwShareMode | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
				err = ::GetLastError();
			}
		}

		// 완전 실패
		if (m_handle == INVALID_HANDLE_VALUE) 
		{
			//MessageBox(nullptr, filePathName, L"", MB_OK);  
			//ASSERT(0); 
			return FALSE; 
		}
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
///         파일을 통채로 읽어서 XBuffer 에 쓴다
/// @param  
/// @return 
/// @date   Mon Feb 22 18:00:43 2016
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XFileReadStream::ReadWhole(XBuffer& buffer, BOOL addnullptr)
{
	const INT64 fileSize = GetSize();
	const int size = (int)fileSize + (addnullptr ? 2 : 0);
	if (buffer.Alloc(size) == FALSE) return FALSE;
	SetPos(0);
	DWORD read;
	if (Read(buffer.data, (int)fileSize, &read) == FALSE || fileSize != read) return FALSE;
	if (addnullptr)				// 문자열 파싱시 처리를 쉽게하기 위해서 끝에 nullptr 하나 넣어주기.
	{
		buffer.data[buffer.allocSize - 1] = 0;
		buffer.data[buffer.allocSize - 2] = 0;
	}
	buffer.dataSize = (int)fileSize;
	return TRUE;
}

BOOL XFileReadStream::Attach(HANDLE hFile, BOOL autoCloseHandle)
{
	// 닫기 먼저 호출 필요
	if (m_handle != INVALID_HANDLE_VALUE) { ASSERT(0); return FALSE; }
	InitMembers();
	m_handle = hFile;
	m_bAutoCloseHandle = autoCloseHandle;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         
///  메모리 버퍼
///         
////////////////////////////////////////////////////////////////////////////////////////////////////

XMemoryReadStream::XMemoryReadStream()
{
	m_buf = nullptr;
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
	if (m_buf == nullptr || lpNumberOfBytesRead == nullptr) { ASSERT(0); return FALSE; }

	*lpNumberOfBytesRead = 0;

	if (m_pos <0 ) { ASSERT(0); return FALSE; }				// 발생 불가

	const INT64 remain = m_size - m_pos;
	const INT64 to_copy = min(remain, nNumberOfBytesToRead);

	if (to_copy <= 0) return TRUE;							// EOF!
	if (to_copy > UINT_MAX) { ASSERT(0); return FALSE; }	// 발생 불가

	memcpy(lpBuffer, m_buf + m_pos, (DWORD)to_copy);
	m_pos += to_copy;
	*lpNumberOfBytesRead = (DWORD)to_copy;

	return TRUE;
}

// 메모리 포인터 이동하기 - 포인터가 메모리 밖으로 벗어날 수 있으므로 주의해야한다.
INT64 XMemoryReadStream::SetPos(INT64 pos)
{
	if (pos < 0) return m_pos; // 음수는 무시한다.
	m_pos = pos;
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

BOOL XMemoryReadStream::Alloc(INT64 size)
{
	BYTE* buf = (BYTE*)realloc(m_buf, (size_t)size);
	if(buf==nullptr) {ASSERT(0); Free(); return FALSE;}

	m_buf = buf;
	m_size = size;
	m_pos = 0;
	m_bSelfFree = TRUE;

	return TRUE;
}

void XMemoryReadStream::Free() noexcept
{
	// 외부에서 버퍼를 attch 한 경우는 free() 하지 않는다.
	if(m_buf && m_bSelfFree)
		free(m_buf);

	m_buf = nullptr;
	m_size = 0;
	m_pos = 0;
	m_bSelfFree = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         파일 열어서 통채로 버퍼에 저장하기
/// @param  
/// @return 
/// @date   Thursday, October 15, 2015  1:19:41 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XMemoryReadStream::OpenFile(LPCWSTR filePathName)
{
	Free();

	XFileReadStream file;
	if(file.Open(filePathName)==FALSE) {ASSERT(0); return FALSE;}
	if(Alloc(file.GetSize())==FALSE) {ASSERT(0); return FALSE;}
	if(m_size>=UINT_MAX) { ASSERT(0); return FALSE; }			// 4G 보다 큰 파일은 아니겠지?
	DWORD read;
	if(file.Read(m_buf, (DWORD)m_size, &read)==FALSE) {ASSERT(0); return FALSE;}

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         데이터 attach 하기
/// @param  
/// @return 
/// @date   Friday, October 16, 2015  2:44:05 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
void XMemoryReadStream::Attach(BYTE* buf, INT64 size, BOOL bSelfFree)
{
	Free();
	if (m_size < 0) { ASSERT(0); return; }

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
	m_filePathName = nullptr;
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

	const BYTE*	userBuffer = (BYTE*)lpBuffer;
	const int	buf_remain = LEN_BUF - m_bufpos;

	// 내부 버퍼에 복사하고 끝
	if ((DWORD)buf_remain >= nNumberOfBytesToWrite)
	{
		memcpy(m_buf + m_bufpos, userBuffer, nNumberOfBytesToWrite);
		m_bufpos += nNumberOfBytesToWrite;
	}
	else	// 내부 버퍼에 다 집어넣기 부족한 경우
	{
		if(FlushBuffer()==FALSE)		// 버퍼 비우고
			return FALSE;

		// 입력받은 데이터를 그대로 파일에 쓴다.
		DWORD written;
		if(::WriteFile(m_handle, userBuffer, nNumberOfBytesToWrite, &written, nullptr)==FALSE ||
			written!=nNumberOfBytesToWrite)								// written!=nNumberOfBytesToWrite 는 발생하지 않는다!
		{ASSERT(0); return FALSE;}
	}
	m_virtPos += nNumberOfBytesToWrite;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         내부 버퍼를 다 파일로 쓰고 비워버린다.
/// @param  
/// @return 
/// @date   Thursday, October 15, 2015  2:38:16 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XFileWriteStream::FlushBuffer() noexcept
{
	if(m_handle!=INVALID_HANDLE_VALUE && m_bufpos)
	{
		DWORD written;
		if(::WriteFile(m_handle, m_buf, m_bufpos, &written, nullptr)==FALSE || written!=m_bufpos)	// written!=m_bufpos 는 발생하지 않는다.
		{ASSERT(0); return FALSE;}
	}

	m_bufpos = 0;
	return TRUE;
}

INT64	XFileWriteStream::SetPos(INT64 pos)
{
	// 무조건 플러시하고 이동
	FlushBuffer();

	LARGE_INTEGER li;
	li.QuadPart = pos;
	const DWORD ret = ::SetFilePointer(m_handle, li.LowPart, &li.HighPart, FILE_BEGIN);	// 진짜로 파일 이동

	if(ret==INVALID_SET_FILE_POINTER)
	{	
		const DWORD err = ::GetLastError();	// 아마도 ERROR_NEGATIVE_SEEK 

		// 에러 상황
		if (err == ERROR_NEGATIVE_SEEK)
		{
			// 아무것도 하지 않는다.
		}
		else
		{
			// 정상 상황.
			m_virtPos = pos;
		}
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
	const BOOL ret = ::GetFileSizeEx(m_handle, &size);
	if(ret==FALSE) { ASSERT(0); return -1;}
	return size.QuadPart;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         쓰기용 파일 생성하기 
/// @param  
/// @return 
/// @date   Friday, October 16, 2015  10:21:35 AM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XFileWriteStream::Open(LPCWSTR filePathName)
{
	Close();

	m_handle = CreateFileW(filePathName, GENERIC_WRITE, FILE_SHARE_READ  , nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if(m_handle==INVALID_HANDLE_VALUE) 
	{
		DWORD error = ::GetLastError();

		// 파일 지우고 다시 해보기
		if(error==ERROR_ACCESS_DENIED)
		{
			::SetFileAttributesW(filePathName, FILE_ATTRIBUTE_NORMAL);
			::DeleteFileW(filePathName);

			m_handle = CreateFileW(filePathName, GENERIC_WRITE, FILE_SHARE_READ , nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
			if(m_handle==INVALID_HANDLE_VALUE) 
			{
				error = ::GetLastError();
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	m_filePathName = _wcsdup(filePathName);
	m_bAutoCloseHandle = TRUE;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         기존 핸들을 attach 하기
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

void XFileWriteStream::Close(const FILETIME* mt) noexcept
{
	FlushBuffer();

	if (m_bAutoCloseHandle && m_handle != INVALID_HANDLE_VALUE)
	{
		if(mt)
			::SetFileTime(m_handle, NULL, NULL, mt);
		CloseHandle(m_handle);
	}

	m_handle = INVALID_HANDLE_VALUE; 
	m_bufpos = 0;
	m_virtPos = 0;
	m_bAutoCloseHandle = FALSE;
	if(m_filePathName) free((void*)m_filePathName);
	m_filePathName = nullptr;
}

/*
////////////////////////////////////////////////////////////////////////////////////////////////////
///         파일 시간 세팅하기
/// @param  
/// @return 
/// @date   Fri Jun 28 11:08:43 2019
////////////////////////////////////////////////////////////////////////////////////////////////////
bool XFileWriteStream::SetFileTime(time_t t)
{
	if (m_handle == INVALID_HANDLE_VALUE) { ASSERT(0); return false; }
	if (t == 0) return false;

	FILETIME ft = { 0,0 };
	INT64 ll = t * 10000000 + 116444736000000000LL;	
	ft.dwLowDateTime = (DWORD)ll;
	ft.dwHighDateTime = (DWORD)(ll >> 32);
	return ::SetFileTime(m_handle, NULL, NULL, &ft);
}

bool XFileWriteStream::SetFileTime(FILETIME* ct, FILETIME* at, FILETIME* mt)
{
	return ::SetFileTime(m_handle, ct, at, mt);
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////
///         
///         메모리 쓰기 스트림
///         
////////////////////////////////////////////////////////////////////////////////////////////////////
XMemoryWriteStream::XMemoryWriteStream()
{
	m_buf = nullptr;
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
	const INT64 buf_remain = m_allocSize - m_pos;

	// 메모리가 부족한 경우.. alloc 호출
	if(buf_remain<(INT64)nNumberOfBytesToWrite)
		if(IncreaseBuffer(m_pos+nNumberOfBytesToWrite)==FALSE)	// m_pos 가 m_allocSize 보다 클 수 있기 때문에, m_allocSize + nNumberOfBytesToWrite 가 아니다.
			return FALSE;

	// 메모리 복사후 OK
	memcpy(m_buf+m_pos, lpBuffer, nNumberOfBytesToWrite);
	m_pos += nNumberOfBytesToWrite;
	m_size = max(m_pos, m_size);			// SetPos 가 호출되었을 수 있기 때문에 m_size 는 m_pos 의 max 값으로 계산한다.
	return TRUE;
}

INT64 XMemoryWriteStream::SetPos(INT64 pos)
{
	if(pos<0) {/*ASSERT(0); */return FALSE;}	// 음수는 ERROR_NEGATIVE_SEEK 에러와 동일 = 무시한다.
	m_pos = pos;
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

void XMemoryWriteStream::Free() noexcept
{
	// 외부에서 버퍼를 attch 한 경우는 free() 하지 않는다.
	if(m_buf && m_bSelfFree)
		free(m_buf);

	m_buf = nullptr;
	m_allocSize = 0;
	m_size = 0;
	m_pos = 0;
	m_bSelfFree = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         targetSize 이상으로 메모리를 늘린다.
/// @param  
/// @return 
/// @date   Thursday, October 15, 2015  6:46:08 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XMemoryWriteStream::IncreaseBuffer(INT64 targetSize)
{
	INT64 newSize = max(INITIAL_ALLOC_SIZE, m_allocSize);

	// 새로운 크기 계산하고
	while(newSize<targetSize)
	{
		newSize = newSize*2;
		if(newSize>m_maxAllocSize)
		{ASSERT(0); return FALSE;}
	}

	// 메모리 realloc
	BYTE* buf = (BYTE*)realloc(m_buf, (size_t)newSize);
	if(buf==nullptr) {ASSERT(0); return FALSE;}

	// 파일과 작동을 비슷하게 하기 위해서, realloc 한 부분을 0 으로 채우기
	memset(buf+m_allocSize, 0, (size_t)(newSize - m_allocSize));


	m_buf = buf;
	m_allocSize = newSize;
	m_bSelfFree = TRUE;


	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         Write 하기전 크기를 미리 알 경우 Alloc 해서 사용하고자 할 때 사용
/// @param  
/// @return 
/// @date   Thursday, October 15, 2015  6:50:17 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL XMemoryWriteStream::Alloc(INT64 size)
{
	Free();
	m_buf = (BYTE*)malloc((size_t)size);
	if(m_buf==nullptr) {ASSERT(0); return FALSE;}
	m_allocSize = size;
	m_bSelfFree = TRUE;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         외부 메모리 attach - Free() 호출시 free() 하지 않는다.
/// @param  
/// @return 
/// @date   Friday, October 23, 2015  2:42:22 PM
////////////////////////////////////////////////////////////////////////////////////////////////////
void XMemoryWriteStream::Attach(BYTE* buf, INT64 len)
{
	Free();
	if (len < 0) { ASSERT(0); return; }
	m_buf = buf;
	m_allocSize = len;
	m_size = 0;
	m_pos = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// 
/// XFILE 
/// 
/// 
///////////////////////////////////////////////////////////////////////////////////////////////////

XFile::XFile()
{
	m_handle = INVALID_HANDLE_VALUE;
	m_err = 0;
}


XFile::~XFile()
{
	Close();
}


void XFile::Close()
{
	if (m_handle != INVALID_HANDLE_VALUE)
		::CloseHandle(m_handle);
	m_handle = INVALID_HANDLE_VALUE;
}

bool XFile::Open(LPCWSTR filePathName, DWORD access, DWORD share, DWORD creation, DWORD flags)
{
	Close();

	m_handle = CreateFileW(filePathName, access, share, nullptr, creation, flags, nullptr);

	if (m_handle == INVALID_HANDLE_VALUE)
	{
		m_err = ::GetLastError();
		return false;
	}

	return true;
}

int XFile::GetSize32() const
{
	if (m_handle == INVALID_HANDLE_VALUE)
		return -1;
	return ::GetFileSize(m_handle, NULL);
}

int XFile::GetPos32() const
{
	if (m_handle == INVALID_HANDLE_VALUE)
		return -1;
	return ::SetFilePointer(m_handle, 0, NULL, FILE_CURRENT);
}

bool XFile::ReadWhole(XBuffer& buffer) const
{
	if (m_handle == INVALID_HANDLE_VALUE)
		return false;

	const int oldPos = GetPos32();
	const int size = GetSize32();

	if (buffer.Alloc(size) == false) return false;
	SetFilePointer(m_handle, 0, NULL, FILE_BEGIN);

	DWORD read;
	if (ReadFile(m_handle, buffer.data, size, &read, NULL) == FALSE || size != (int)read){ ASSERT(0); return false;}
	buffer.dataSize = size;

	SetFilePointer(m_handle, oldPos, NULL, FILE_BEGIN);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         임시 파일을 생성하고, 데이터를 쓴다.
/// @param  
/// @return 
/// @date   Thu Sep  6 15:01:42 2018
////////////////////////////////////////////////////////////////////////////////////////////////////
bool XFile::CreateTempFileAndWriteData(LPCWSTR filePathName, const XBuffer& data)
{
	Close();

	constexpr DWORD flags = FILE_FLAG_DELETE_ON_CLOSE | FILE_ATTRIBUTE_TEMPORARY;
	m_handle = CreateFileW(filePathName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, flags, nullptr);
	if (m_handle == INVALID_HANDLE_VALUE) { m_err = ::GetLastError();  ASSERT(0); return false; }

	DWORD written;
	if(::WriteFile(m_handle, data.data, data.dataSize, &written, NULL)==FALSE || (int)written!=data.dataSize){ m_err = ::GetLastError(); ASSERT(0); return false;}

	return true;
}





