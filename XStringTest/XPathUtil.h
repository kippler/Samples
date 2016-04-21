////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// xstring 을 이용한 경로 처리 유틸 함수들
/// 
/// @author   park
/// @date     04/19/16 18:25:02
/// 
/// Copyright(C) 2016 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once

#include "XString.h"


XString		XPath_GetParentFolderFromPath(LPCWSTR szPathName);
void		XPath_AddEndSlash(XString& pathName);

XString		XPath_MakeDestPathNameFromTarLinkName(XString linkFileName, XString linkTargetName);


