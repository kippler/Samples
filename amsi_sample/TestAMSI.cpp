#include "stdafx.h"
#include "TestAMSI.h"
#include "AmsiMemStream.h"
#include "XStream.h"


void TestAMSI(XReadStream& stream, LPCWSTR filePathName)
{
	CComPtr<IAntimalware>	antimalware;
	HRESULT hr = CoCreateInstance(__uuidof(CAntimalware), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&antimalware));
	if (FAILED(hr))
	{
		wprintf(L"Error at CoCreateInstance(), hr=0x%x\n", hr);
		return;
	}

	CComPtr<IAmsiStream> amsiStream;
	hr = MakeAndInitialize<CAmsiMemStream>(&amsiStream, stream, filePathName, AMSI_SCAN_APP_NAME);
	if (FAILED(hr))
	{
		wprintf(L"Error at MakeAndInitialize()\n");
		return;
	}


	CComPtr<IAntimalwareProvider> provider;
	AMSI_RESULT r;
	hr = antimalware->Scan(amsiStream, &r, &provider);
	if (FAILED(hr))
	{
		wprintf(L"Error at antimalware->Scan()\n");
		return;
	}


	const bool isMalware = AmsiResultIsMalware(r);
	wprintf(L"[%s] is %s.\n", 
		filePathName, 
		isMalware ? L"a malware" : L"not a malware");


}



void TestAMSI(LPCWSTR filePathName)
{
	XFileReadStream stream;

	
	if(stream.Open(filePathName)==false)
	{
		wprintf(L"Error. Cannot open %s\n", filePathName);
		return;
	}

	TestAMSI(stream, filePathName);
}


