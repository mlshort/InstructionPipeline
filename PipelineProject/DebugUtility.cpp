/**
 * @file       DebugUtility.cpp
 * @brief      Implementation of DebugUtility.cpp
 * @author     Mark L. Short
 * @date       November 24, 2014
 *
 */

#include "stdafx.h"

#include <stdlib.h>
#include <stdarg.h>
#include <Windows.h>
#include "DebugUtility.h"

#ifdef _DEBUG

    #if defined(UNICODE) || defined(_UNICODE)
        std::wofstream dbg;
    #else
        std::ofstream  dbg;
    #endif

#endif

int DebugTrace (const TCHAR* szFmt, ...)
{
    TCHAR szDebugMsg[512] = { 0 };

    va_list	vaArgs;
    va_start (vaArgs, szFmt);

    // use the format string and arguments to construct the debug output string
    int iReturnVal = _vsntprintf (szDebugMsg, _countof (szDebugMsg) - 1, szFmt, 
                                  vaArgs);
    va_end (vaArgs);

    ::OutputDebugString (szDebugMsg);
    return iReturnVal;

}

TCHAR* GetModulePath (TCHAR* szModulePath, size_t cchLen)
{
    // Get the executable file path
    TCHAR szModuleFileName[_MAX_PATH] = { 0 };

    // Note, if HANDLE is NULL, GetModuleFileName is supposed to return the file 
    // path to the current executable, but it appears that it is inconsistently 
    // returning filename as well....
    DWORD dwStrLen = ::GetModuleFileName (NULL, szModuleFileName, 
                                          _countof(szModuleFileName) );

    TCHAR szDir[_MAX_PATH] = {0};

    _tsplitpath(szModuleFileName, szDir, &szDir[2], NULL, NULL);

    return _tcsncpy(szModulePath, szDir, cchLen);
}
