#include "stdafx.h"
#include "XPathUtil.h"

#ifndef ARK_PATHSEP_CHAR
#	ifdef _WIN32
#		define ARK_PATHSEP_CHAR				'\\'
#	else
#		define ARK_PATHSEP_CHAR				'/'
#	endif
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
///         �θ� ���� ��������
/// @param  
/// @return 
/// @date   Tue Apr 19 18:45:50 2016
////////////////////////////////////////////////////////////////////////////////////////////////////
XString XPath_GetParentFolderFromPath(LPCWSTR szPathName)
{
	if (szPathName == NULL) { return NULL; }
	XString ret(szPathName);

	LPWSTR s = ret.GetBuffer();
	LPWSTR p = ret.GetLastBuffer();
	BOOL found = FALSE;

	p--;
	while (p >= s)
	{
		if (*p == '\\' || *p == '/')			// �� �������� �ִ� / ��ġ�� ã�´�.
		{
			*(p + 1) = '\0';				// ��θ���� ���ڿ��� NULL
			found = TRUE;
			break;
		}
		p--;
	}
	ret.ReleseBuffer();

	if (found == FALSE)
		ret.Empty();

	return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
///         �������� SLASH �� back slash �� ���δ�.
/// @param  
/// @return 
/// @date   Tue Apr 19 18:46:05 2016
////////////////////////////////////////////////////////////////////////////////////////////////////
void XPath_AddEndSlash(XString& pathName)
{
	int len = pathName.GetLength();
	wchar_t* last = pathName.GetLastBuffer();
	if (last==NULL || *last != ARK_PATHSEP_CHAR)
		pathName += ARK_PATHSEP_CHAR;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
///         TAR �ɺ��� ��ũ�̸� ã��
/// @param  ��)
/// 		linkFileName		"aa\sub\abc.txt"
/// 		linkTargetName		"..\src.txt"
/// 		==> ���				"aa\src.txt"
/// @return 
/// @date   Tue Apr 19 18:55:31 2016
////////////////////////////////////////////////////////////////////////////////////////////////////
XString	 XPath_MakeDestPathNameFromTarLinkName(XString linkFileName, XString linkTargetName)
{
	XString folderPathName = XPath_GetParentFolderFromPath(linkFileName);		// "aa\sub\" ã��
	XString ret;
	XString dotdot;

	ret = folderPathName;

	dotdot = L"..";
	dotdot += ARK_PATHSEP_CHAR;			// "..\" ���ڿ�

	for (;;)
	{
		if (linkTargetName.Left(3) == dotdot)
		{
			linkTargetName = linkTargetName.Mid(3);		// "..\" ���ְ�
			ret = XPath_GetParentFolderFromPath(ret);	// ����������
		}
		else
		{
			ret = ret + linkTargetName;
			break;
		}
	}

	return ret;
}



