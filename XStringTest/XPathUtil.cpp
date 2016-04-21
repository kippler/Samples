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
///         부모 폴더 가져오기
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
		if (*p == '\\' || *p == '/')			// 맨 마지막에 있는 / 위치를 찾는다.
		{
			*(p + 1) = '\0';				// 경로명다음 문자열을 NULL
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
///         마지막에 SLASH 나 back slash 를 붙인다.
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
///         TAR 심볼릭 링크이름 찾기
/// @param  예)
/// 		linkFileName		"aa\sub\abc.txt"
/// 		linkTargetName		"..\src.txt"
/// 		==> 결과				"aa\src.txt"
/// @return 
/// @date   Tue Apr 19 18:55:31 2016
////////////////////////////////////////////////////////////////////////////////////////////////////
XString	 XPath_MakeDestPathNameFromTarLinkName(XString linkFileName, XString linkTargetName)
{
	XString folderPathName = XPath_GetParentFolderFromPath(linkFileName);		// "aa\sub\" 찾기
	XString ret;
	XString dotdot;

	ret = folderPathName;

	dotdot = L"..";
	dotdot += ARK_PATHSEP_CHAR;			// "..\" 문자열

	for (;;)
	{
		if (linkTargetName.Left(3) == dotdot)
		{
			linkTargetName = linkTargetName.Mid(3);		// "..\" 없애고
			ret = XPath_GetParentFolderFromPath(ret);	// 상위폴더로
		}
		else
		{
			ret = ret + linkTargetName;
			break;
		}
	}

	return ret;
}



