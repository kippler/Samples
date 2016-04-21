// XStringTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "XString.h"

void Assign()
{
	XString str1(L"Hello");
	XString str2 = L"world";
	XString str3;
	str3 = L"!";
	wprintf(L"%s %s%s\n", str1.GetString(), str2.GetString(), str3.GetString());
}

void Add1()
{
	XString str1 = L"Hello";
	XString str2 = L"world";
	XString str3;

	str3 = str1 + L" " + str2;
	wprintf(L"%s\n", str3.GetString());
}

void Add2()
{
	XString str = L"Hello";
	str += L" ";
	str += L"World";
	wprintf(L"%s\n", str.GetString());
}

void GetBuffer()
{
	XString str = L"Hello";
	wchar_t* p = str.GetBuffer(20);
	wcscpy_s(p, 20, L"Hello World");
	str.ReleseBuffer();
	str += L"!";

	wprintf(L"%s\n", str.GetString());
}

void Mid()
{
	XString str2 = L"Hello World";
	for (int i = 0; i < 4; i++)
	{
		str2 = str2.Mid(3);
		wprintf(L"%s[%d]\n", str2.GetString(), str2.GetLength());
	}
}

void Compare()
{
	XString str1 = L"Hello";
	XString str2 = L"World";

	if (str1 == L"Hello")
		wprintf(L"str1 is Hello\n");

	if (str1 != str2)
		wprintf(L"str1 and str2 are differ\n");

	if(str2.Left(2)==L"Wo")
		wprintf(L"str2 is [Wo]rld\n");

}


void test()
{
	Assign();
	Add1();
	Add2();
	GetBuffer();
	Compare();
	Mid();
}


int main()
{
	test();

	if (_CrtDumpMemoryLeaks())
		printf(" - Memory Leak!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

    return 0;
}

