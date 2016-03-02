#include "stdafx.h"
#include "Global.h"


UINT	WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));




// ����ڰ� �Է��� hotkey �� �Ķ���͸� RegisterHotKey() �� �°� �ٲپ ����ϱ� 
// �̻��ϰ� MOD_ �� ��Ű ��Ʈ���� modifier �� �Ųٷ� �Ǿ� �ִ�..
BOOL MyRegisterHotKey(HWND hWnd, DWORD cmd, DWORD key)
{
	DWORD mod1 = key >> 8;
	DWORD mod2 = 0;
	DWORD vk = key & 0xff;

	if (mod1 & HOTKEYF_SHIFT)	mod2 |= MOD_SHIFT;
	if (mod1 & HOTKEYF_CONTROL)	mod2 |= MOD_CONTROL;
	if (mod1 & HOTKEYF_ALT)		mod2 |= MOD_ALT;

	return RegisterHotKey(hWnd, cmd, mod2, vk);
}




BOOL	CopyToClipboardText(CString str, HWND hWnd)
{
	int		size = (str.GetLength() + 1) * sizeof(WCHAR);
	HGLOBAL	hMem = ::GlobalAlloc(GPTR, size);
	BOOL	ret = FALSE;

	if (hMem)
	{
		GlobalLock(hMem);
		memcpy((BYTE*)hMem, str.GetBuffer(), size);
		GlobalUnlock(hMem);

		if (::OpenClipboard(hWnd))
		{
			::EmptyClipboard();
			if (::SetClipboardData(CF_UNICODETEXT, hMem))
				ret = TRUE;
			else
			{
				ASSERT(0);
				GlobalFree(hMem);
			}
			::CloseClipboard();
		}
		else
			ASSERT(0);
	}

	return ret;
}



