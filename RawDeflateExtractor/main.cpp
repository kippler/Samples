#include <Windows.h>
#include <atlbase.h>
#include "util/XStream.h"
#include "libz-1.2.11/zlib.h"
#include <inttypes.h>


#define LEN_SRC_BUF     4096
#define LEN_OUT_BUF     4096

bool DoExtract(z_stream& stream, XFileReadStream& src, XFileWriteStream& dst)
{
	BYTE bufSrc[LEN_SRC_BUF];
	BYTE bufOut[LEN_OUT_BUF];

	int64_t srcRemain = src.GetSize();
	DWORD read;

	while (srcRemain || stream.avail_out || stream.avail_in)
	{
		if (stream.avail_in == 0 && srcRemain > 0)
		{
			int toRead = min(LEN_SRC_BUF, srcRemain);
			if (src.Read(bufSrc, toRead, &read) == FALSE)
			{
				wprintf(L"Error. Cannot read src data.\n");
				return false;
			}
			if (read == 0)
				break;

			srcRemain -= toRead;
			stream.next_in = bufSrc;
			stream.avail_in = toRead;
		}


		stream.next_out = bufOut;
		stream.avail_out = LEN_OUT_BUF;
		const uLong totOutBefore = stream.total_out;

		// INFLATE
		const int err = inflate(&stream, Z_SYNC_FLUSH);
		if (err != Z_OK && err != Z_STREAM_END)
		{
			wprintf(L"Error. Cannot decode src data.\n");
			return false;
		}

		// WRITE
		const int toWrite = stream.total_out - totOutBefore ;
		if(dst.Write(bufOut, toWrite)==FALSE)
		{
			wprintf(L"Error. Cannot write out data.\n");
			return false;
		}

		// END OF ZSTREAM
		if (err == Z_STREAM_END)
			break;
	}

	return true;
}



bool DoExtract(LPCWSTR srcFileName, LPCWSTR dstFileName)
{
	XFileReadStream src;
	XFileWriteStream dst;

	if (src.Open(srcFileName) == FALSE)
	{
		wprintf(L"Error. Cannot open %ls\n", srcFileName);
		return false;
	}

	if (dst.Open(dstFileName) == FALSE)
	{
		wprintf(L"Error. Cannot open %ls\n", dstFileName);
		return false;
	}

	z_stream stream = { 0, };
	inflateInit2(&stream, -MAX_WBITS);

	bool ret = DoExtract(stream, src, dst);

	inflateEnd(&stream);

	return ret;
}




int __cdecl wmain(int argc, WCHAR* argv[])
{
	if (argc != 3)
	{
		printf("Usage: rawdeflateextractor.exe [src file name] [out file name]\n");
		return 0;
	}

	bool ret = DoExtract(argv[1], argv[2]);

	return ret ? 0 : 1;
}


