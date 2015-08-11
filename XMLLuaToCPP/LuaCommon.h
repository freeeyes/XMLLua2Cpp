#ifndef _LUA_COMMON_H
#define _LUA_COMMON_H

#include <stdio.h>
#include <stdio.h>
#include <stdarg.h>

#ifndef WIN32
#include <time.h>
#else
#include "windows.h"
#endif

#define LOG_STRING_LENGTH 300

static unsigned long GetSystemTickCount()
{
#ifdef WIN32
	return GetTickCount();
#else
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);

	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}

static void sprintf_safe(char* pText, int nLen, const char* fmt ...)
{
	va_list ap;
	va_start(ap, fmt);

#ifdef WIN32
	vsnprintf_s(pText, nLen, _TRUNCATE, fmt, ap);
#else
	vsnprintf(pText, nLen, fmt, ap);
#endif
	pText[nLen - 1] = '\0';

	va_end(ap);
}

static void Lua_Print(const char* fmt ...)
{
	char szText[LOG_STRING_LENGTH] = {'\0'};

	va_list ap;
	va_start(ap, fmt);

#ifdef WIN32
	vsnprintf_s(szText, LOG_STRING_LENGTH, _TRUNCATE, fmt, ap);
#else
	vsnprintf(szText, LOG_STRING_LENGTH, fmt, ap);
#endif
	szText[LOG_STRING_LENGTH - 1] = '\0';

	va_end(ap);

#ifdef WIN32
	printf_s(szText);
#else
	printf(szText);
#endif
}

#endif
