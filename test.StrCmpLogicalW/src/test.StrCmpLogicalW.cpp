#include <iostream>
#include <tchar.h>
#include <Windows.h>
#include <shlwapi.h>
#include <locale.h>
#include <fcntl.h>
#	include <io.h>
#pragma comment(lib, "Shlwapi.lib")


void Compare(LPCWSTR s1, LPCWSTR s2)
{
    const int ret = StrCmpLogicalW(s1, s2);

    if (ret == 0)
    {
        wprintf(L"%s == %s \n", s1, s2);
    }
    else if (ret > 0)
    {
        wprintf(L"%s > %s\n", s1, s2);
    }
    else if (ret < 0)
    {
        wprintf(L"%s < %s\n", s1, s2);
    }

}



int main()
{
    _setmode(_fileno(stdout), _O_U8TEXT);

    Compare(L"55.jpg", L"3.jpg");
    Compare(L"ƼƼ.jpg", L"3.jpg");

    Compare(L"6.jpg", L"55.jpg");
    Compare(L"6.jpg", L"ƼƼ.jpg");  // <<-- The problem!

    Compare(L"55.jpg", L"10.jpg");
    Compare(L"ƼƼ.jpg", L"10.jpg");

    system("pause");
    return 0;
}
