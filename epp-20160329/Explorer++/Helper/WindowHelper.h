#pragma once

BOOL CenterWindow(HWND hParent, HWND hChild);
void GetWindowString(HWND hwnd, std::wstring &str);
BOOL lShowWindow(HWND hwnd, BOOL bShowWindow);
BOOL AddWindowStyle(HWND hwnd, UINT fStyle, BOOL bAdd);
int GetRectHeight(const RECT *rc);
int GetRectWidth(const RECT *rc);