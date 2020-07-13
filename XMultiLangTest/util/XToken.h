////////////////////////////////////////////////////////////////////////////////////////////////////
/// 
/// xtoken - tokenizer
/// 
/// @author   parkkh
/// @date     Tuesday, February 08, 2011  2:11:13 PM
/// 
/// Copyright(C) 2010 Bandisoft, All rights reserved.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <windows.h>

#ifndef ASSERT
#include <atlbase.h>
#	define ASSERT(x) ATLASSERT(x)
#endif


template<typename CHAR_TYPE>
class XTokenT
{
public :
	XTokenT()
	{
		_str = NULL;
		_cur = NULL;
		_next = NULL;
		_sep = 0;
	}
	~XTokenT()
	{
		if(_str) free(_str);
	}

	void	Init(const CHAR_TYPE* str, CHAR_TYPE sep, int lenChar=-1)
	{
		if(_str) free(_str);
		//_str = _strdup(str);
		_str = my_strdup(str, lenChar);
		_sep = sep;
		if(_str==NULL){ASSERT(0); return; }
		_next = FindNext(_str);
		_cur  = NULL;
	}

	const CHAR_TYPE*	GetNextToken()
	{
		if(_cur==NULL)			// 처음 호출인 경우
		{
			_cur = _str;
			return _cur;
		}

		if(_next==NULL) return NULL;

		_cur = _next;
		_next = FindNext(_next);

		return _cur;
	}

private :
	CHAR_TYPE*	FindNext(CHAR_TYPE* p)		// 다음 토큰 시작 위치를 찾는다.
	{
		while(*p)
		{
			if(*p==0) return NULL;			// 끝에 도달했다.

			if(*p==_sep)					// seperator 를 찾았다.
			{
				while(*p)
				{
					*p = 0;					// seperator 를 0 으로 만든다.
					p++;
					if(*p==0) return NULL;	// 끝에 도달했다.

					if(*p!=_sep)
						return p;			// 새로운 토큰의 시작 위치
				}
			}
			p++;
		}
		return NULL;
	}

	CHAR_TYPE* my_strdup(const CHAR_TYPE* str, int lenChar)
	{
		if(str==NULL) return NULL;

		CHAR_TYPE* dest;
		size_t	   len = 0;

		if(lenChar<0)				// 길이가 지정되지 않은 경우 NULL terminated string 으로 간주하고 길이를 구한다.
		{
			const CHAR_TYPE	*p;
			for (p = str; *p; ++p);
			len = p - str;
		}
		else len = lenChar;


		dest = (CHAR_TYPE*)malloc(sizeof(CHAR_TYPE) * (len + 1));
		if(dest==NULL){ASSERT(0); return NULL;}

		CHAR_TYPE* q = dest;
		while(len)					// 길이만큼만 복사하고
		{
			*q = *str;
			q++;
			str++;
			len--;
		}
		*q = 0;						// 마지막에는 NULL

		//for (; (*q = *str) != 0; ++q, ++str);			--> NULL 까지 복사하기 때문에 NULL 로 끝나지 않는 문자열에서는 문제 발생

		return dest;
	}


private :
	CHAR_TYPE*	_str;
	CHAR_TYPE*	_cur;
	CHAR_TYPE*	_next;
	CHAR_TYPE	_sep;
};

typedef XTokenT<char> XTokenA;
typedef XTokenT<wchar_t> XTokenW;


#ifdef _UNICODE
#	define XToken XTokenW
#else
#	define XToken XTokenA
#endif

/*

void	Test(TCHAR* str, TCHAR sep)
{
	_tprintf(_T("\nTest() - [%s] [%c]\n"), str, sep);

	XToken			tok;
	const TCHAR*	p;

	tok.Init(str, sep);

	p = tok.GetNextToken();
	while(p)
	{
		_tprintf(_T("====== %s \n"), p);
		p = tok.GetNextToken();
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	Test(_T("Hello:world:!!"), ':');

	Test(_T("Hello:world:!!"), 0);

	Test(NULL, ':');							// assert 테스트

	Test(_T("Hello::::::world:::::_:!!"), ':');

	return 0;
}

*/

