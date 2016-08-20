/******************************************************************
*
* Project: Helper
* File: ProcessHelper.cpp
* License: GPL - See LICENSE in the top level directory
*
* Process helper functionality.
*
* Written by David Erceg
* www.explorerplusplus.com
*
*****************************************************************/

#include "stdafx.h"
#include "Helper.h"
#include "ProcessHelper.h"


DWORD GetProcessImageName(DWORD dwProcessId, TCHAR *szImageName, DWORD nSize)
{
	DWORD dwRet = 0;

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);

	if(hProcess != NULL)
	{
		dwRet = GetModuleFileNameEx(hProcess, NULL, szImageName, nSize);
		CloseHandle(hProcess);
	}

	return dwRet;
}

BOOL GetProcessOwner(DWORD dwProcessId, TCHAR *szOwner, size_t cchMax)
{
	BOOL success = FALSE;

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);

	if(hProcess != NULL)
	{
		HANDLE hToken;
		BOOL bRet = OpenProcessToken(hProcess, TOKEN_QUERY, &hToken);

		if(bRet)
		{
			DWORD dwSize = 0;
			bRet = GetTokenInformation(hToken, TokenUser, NULL, 0, &dwSize);

			if(!bRet && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				TOKEN_USER *pTokenUser = reinterpret_cast<TOKEN_USER *>(GlobalAlloc(GMEM_FIXED, dwSize));

				if(pTokenUser != NULL)
				{
					bRet = GetTokenInformation(hToken, TokenUser, reinterpret_cast<LPVOID>(pTokenUser), dwSize, &dwSize);

					if(bRet)
					{
						success = FormatUserName(pTokenUser->User.Sid, szOwner, cchMax);
					}

					GlobalFree(pTokenUser);
				}
			}

			CloseHandle(hToken);
		}

		CloseHandle(hProcess);
	}

	return success;
}

BOOL SetProcessTokenPrivilege(DWORD dwProcessId, const TCHAR *PrivilegeName, BOOL bEnablePrivilege)
{
	BOOL success = FALSE;

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);

	if(hProcess != NULL)
	{
		HANDLE hToken;
		BOOL bRet = OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken);

		if(bRet)
		{
			LUID luid;
			bRet = LookupPrivilegeValue(NULL, PrivilegeName, &luid);

			if(bRet)
			{
				TOKEN_PRIVILEGES tp;
				tp.PrivilegeCount = 1;
				tp.Privileges[0].Luid = luid;

				if(bEnablePrivilege)
				{
					tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
				}
				else
				{
					tp.Privileges[0].Attributes = 0;
				}

				bRet = AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL);

				success = bRet && (GetLastError() == ERROR_SUCCESS);
			}

			CloseHandle(hToken);
		}

		CloseHandle(hProcess);
	}

	return success;
}