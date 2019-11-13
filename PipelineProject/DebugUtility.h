/**
 * @file       DebugUtility.h
 *
 * @brief      Debugging and utility method declarations
 *
 * @author     Mark L. Short
 * @date       November 24, 2014
 *
 */
#pragma once

#ifndef __DEBUG_UTILITY_H__
#define __DEBUG_UTILITY_H__

#ifndef _INC_TCHAR
    #include <tchar.h>
#endif

#ifdef _DEBUG
    #ifndef _FSTREAM_
        #include <fstream>
    #endif

    #if defined(UNICODE) || defined(_UNICODE)
        extern std::wofstream dbg;
    #else
        extern std::ofstream  dbg;
    #endif
#endif

/**
  @brief Directs output to the IDE output window.

  @param [in] szFmt     printf-styled format string

  @retval int  the number of characters written if the number of characters to 
               write is less than or equal to count; if the number of characters 
               to write is greater than count, these functions return -1 
               indicating that output has been truncated. The return value does 
               not include the terminating null, if one is written.
*/
int DebugTrace (const TCHAR* szFmt, ...) noexcept;

/**
    @brief Retrieves the current executable directory

    @param [out] szModulePath   destination memory address used to write 
                                application's directory path
    @param [in]  cchLen         count of characters in available to be written 
                                in destination buffer

    @retval  TCHAR*             destination address
    @retval  NULL               on error
*/
TCHAR* GetModulePath(TCHAR* szModulePath, size_t cchLen) noexcept;

#endif
