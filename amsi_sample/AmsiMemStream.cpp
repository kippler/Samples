#include "stdafx.h"
#include "AmsiMemStream.h"


static HRESULT CopyAttribute(
	_In_ const void* resultData,
	_In_ size_t resultSize,
	_In_ ULONG bufferSize,
	_Out_writes_bytes_to_(bufferSize, *actualSize) PBYTE buffer,
	_Out_ ULONG* actualSize)
{
	*actualSize = (ULONG)resultSize;
	if (bufferSize < resultSize)
	{
		return E_NOT_SUFFICIENT_BUFFER;
	}
	memcpy_s(buffer, bufferSize, resultData, resultSize);
	return S_OK;
}


CAmsiMemStream::CAmsiMemStream() noexcept
{
	m_pStream = nullptr;
}


CAmsiMemStream::~CAmsiMemStream()
{
	Close();
}

void CAmsiMemStream::Close()
{
	m_file.Close();
	m_filePathName.clear();
	m_appName.clear();
	m_pStream = nullptr;
}

HRESULT CAmsiMemStream::RuntimeClassInitialize(LPCWSTR fileName, LPCWSTR appName)
{
	// 파일 닫고
	Close();

	if (m_file.Open(fileName) == FALSE)
	{
		ASSERT(0);
		return E_FAIL;
	}

	m_filePathName = fileName;
	m_appName = appName;

	// 현재 스트림은 파일 스트림
	m_pStream = &m_file;

	return S_OK;
}

HRESULT CAmsiMemStream::RuntimeClassInitialize(XReadStream& readStream, LPCWSTR fileName, LPCWSTR appName)
{
	// 파일 닫고
	Close();

	// 현재 스트림은 파라미터 넘어온 스트림
	m_pStream = &readStream;
	m_appName = appName;
	m_filePathName = fileName;

	return S_OK;
}



HRESULT CAmsiMemStream::GetAttribute(
	_In_ AMSI_ATTRIBUTE attribute,
	_In_ ULONG bufferSize,
	_Out_writes_bytes_to_(bufferSize, *actualSize) PBYTE buffer,
	_Out_ ULONG* actualSize)
{
	// 파라미터 체크
	if (actualSize == nullptr || (buffer == nullptr && bufferSize > 0)) {
		return E_INVALIDARG;
	}

	*actualSize = 0;

	switch (attribute)
	{
	case AMSI_ATTRIBUTE_CONTENT_SIZE:
	{
		INT64 fileSize = m_pStream->GetSize();
		return CopyAttribute(&fileSize, sizeof(fileSize), bufferSize, buffer, actualSize);
	}

	case AMSI_ATTRIBUTE_CONTENT_NAME:
	{
		if (m_filePathName.empty() == FALSE)
			return CopyAttribute(m_filePathName.c_str(), (m_filePathName.size() + 1) * sizeof(WCHAR), bufferSize, buffer, actualSize);
	}
	break;

	case AMSI_ATTRIBUTE_APP_NAME:
	{
		return CopyAttribute(m_appName.c_str(), (m_appName.size() + 1)*sizeof(WCHAR), bufferSize, buffer, actualSize);
	}

	case AMSI_ATTRIBUTE_SESSION:
	{
		constexpr HAMSISESSION session = nullptr; // no session for file stream
		return CopyAttribute(&session, sizeof(session), bufferSize, buffer, actualSize);
	}

	case AMSI_ATTRIBUTE_CONTENT_ADDRESS:			// Return the memory address if the content is fully loaded into memory.
	case AMSI_ATTRIBUTE_REDIRECT_CHAIN_ADDRESS :
	case AMSI_ATTRIBUTE_ALL_ADDRESS :
		// 무시
		break;

	default :
		ASSERT(0);
	}

	return E_NOTIMPL; // unsupport attribute
}



HRESULT CAmsiMemStream::Read(
	_In_ ULONGLONG position,
	_In_ ULONG size,
	_Out_writes_bytes_to_(size, *readSize) PBYTE buffer,
	_Out_ ULONG* readSize)
{

	//OVERLAPPED o = {};
	//o.Offset = LODWORD(position);
	//o.OffsetHigh = HIDWORD(position);



	//if (!ReadFile(m_fileHandle.Get(), buffer, size, readSize, &o))
	//{
	//	HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
	//	wprintf(L"ReadFile failed with 0x%x\n", hr);
	//	return hr;
	//}

	m_pStream->SetPos(position);

	DWORD read = 0;
	BOOL ret = m_pStream->Read(buffer, size, &read);

	if (ret == FALSE)
	{
		ASSERT(0);
		return E_FAIL;
	}

	if (readSize)
		*readSize = (ULONG)read;

	return S_OK;
}

