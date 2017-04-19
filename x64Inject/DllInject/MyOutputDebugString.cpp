//MyOutputDebugString.cpp 
#include "stdafx.h"
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#pragma warning(disable:4996)

#define MYPRINT

//MyOutputDebugStringA("%d,%s",123,"hello");
void MyOutputDebugStringA(const char * szOutputString,...)
{
#ifdef MYPRINT
	char szBuffer[4096]={0};
	va_list vlArgs;
	va_start(vlArgs,szOutputString);
	_vsnprintf(szBuffer,sizeof(szBuffer)-1,szOutputString,vlArgs);
	//vsprintf(szBuffer,szOutputString,vlArgs);
	va_end(vlArgs);
	char szPrint[4096 + 100]={0};
	char *szFilter = "[sunflover] ";
	strcpy(szPrint,szFilter);
	strcat(szPrint,szBuffer);
	OutputDebugStringA(szPrint);
#endif
}

//MyOutputDebugStringW(L"%d,%s",456,L"world!");
void MyOutputDebugStringW(const wchar_t * szOutputString,...)
{
#ifdef MYPRINT
	wchar_t szBuffer[4096]={0};
	va_list vlArgs;
	va_start(vlArgs,szOutputString);
	_vsnwprintf(szBuffer,sizeof(szBuffer)-2,szOutputString,vlArgs);
	//vswprintf(szBuffer,szOutputString,vlArgs);
	va_end(vlArgs);
	wchar_t szPrint[4096 + 100]={0};
	wchar_t *szFilter = L"[sunflover] ";
	wcscpy(szPrint,szFilter);
	wcscat(szPrint,szBuffer);
	OutputDebugStringW(szPrint);
#endif
}
