#include "stdafx.h"
#include <iostream>
#include "TestAMSI.h"




int wmain(int argc, const wchar_t** argv)
{
    if (argc != 2)
    {
        printf("usage: amsi_sample.exe [file to test]\n");
        return 0;
    }



    const wchar_t* filePathName = argv[1];

    ::CoInitialize(nullptr);

    TestAMSI(filePathName);

    ::CoUninitialize();


    return 0;
}



