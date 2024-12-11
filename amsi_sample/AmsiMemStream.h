////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// 
/// 
/// @author   park
/// @date     Thu Feb 13 18:04:11 2020
/// 
/// Copyright(C) 2020 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once

#undef new 
#pragma warning(push)
#pragma warning(disable: 28204)	
#include <amsi.h>
#pragma warning(pop)

#include <wrl\implements.h>		
#include <wrl\wrappers\corewrappers.h>
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

#include "XStream.h"


class CAmsiMemStream : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IAmsiStream>
{
public :
	CAmsiMemStream() noexcept;
	~CAmsiMemStream();

	HRESULT			RuntimeClassInitialize(LPCWSTR fileName, LPCWSTR appName);			// called by MakeAndInitialize 
	HRESULT			RuntimeClassInitialize(XReadStream& readStream, LPCWSTR fileName, LPCWSTR appName);	// called by MakeAndInitialize 
	void			Close();


public :			// IAmsiStream
	STDMETHOD(GetAttribute)(
		_In_ AMSI_ATTRIBUTE attribute,
		_In_ ULONG bufferSize,
		_Out_writes_bytes_to_(bufferSize, *actualSize) PBYTE buffer,
		_Out_ ULONG* actualSize);
	STDMETHOD(Read)(
		_In_ ULONGLONG position,
		_In_ ULONG size,
		_Out_writes_bytes_to_(size, *readSize) PBYTE buffer,
		_Out_ ULONG* readSize);


private :
	XFileReadStream		m_file;
	XReadStream*		m_pStream;
	std::wstring		m_filePathName;
	std::wstring		m_appName;


};

