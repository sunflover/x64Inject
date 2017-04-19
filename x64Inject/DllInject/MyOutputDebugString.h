//MyOutputDebugString.h 
#pragma once
#define MYPRINT
//MyOutputDebugString(_T("%d,%s"),123,_T("hello"));

#ifdef _UNICODE
#define MyOutputDebugString MyOutputDebugStringW
#else
#define MyOutputDebugString MyOutputDebugStringA
#endif


//MyOutputDebugStringA("%d,%s",123,"hello");
void MyOutputDebugStringA(const char * szOutputString,...);

//MyOutputDebugStringW(L"%d,%s",456,L"world!");
void MyOutputDebugStringW(const wchar_t * szOutputString,...);
