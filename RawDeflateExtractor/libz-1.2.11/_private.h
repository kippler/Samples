#pragma once

#pragma warning(disable: 6385)
#pragma warning(disable: 4244)
#pragma warning(disable: 4267)
#pragma warning(disable: 6326)


#ifndef _WIN32


#	pragma GCC diagnostic ignored "-Wshift-negative-value"



#endif




// zconf.h 처리용
#define ZLIB_CONST


// GZIP 의 CRC 가 필요 없다.
#define NO_GZIP


