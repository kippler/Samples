////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// AMSI test
/// 
/// @author   park
/// @date     Wed Dec 11 14:26:32 2024
/// 
/// Copyright(C) 2024 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once

#define  _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <inttypes.h>
#include <shobjidl.h>
#include <amsi.h>
#include <atlbase.h>
#include <wrl\implements.h>					// MakeAndInitialize


#define AMSI_SCAN_APP_NAME		L"Amsi_sample"




void TestAMSI(LPCWSTR filePathName);





