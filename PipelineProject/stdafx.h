/**
* @file       stdafx.h
* @brief      Application header file
*
* Include file for standard system include header files, or project specific 
* include files that are used frequently, but are changed infrequently
*
* @author     Mark L. Short
* @date       Oct 30, 2014
*/

#pragma once

#include "targetver.h"

#define _CRT_SECURE_NO_WARNINGS // turn off silly warnings from using string methods

#include <stdio.h>
#include <tchar.h>

#ifndef _STRING_
    #include <string>
#endif

#ifndef _IOSTREAM_
    #include <iostream>
#endif

#ifndef _IOMANIP_
    #include <iomanip>
#endif

#if defined(UNICODE) || defined(_UNICODE)
    #define tcin    std::wcin
    #define tcout   std::wcout
    #define tstring std::wstring
#else
    #define tcin    std::cin 
    #define tcout   std::cout
    #define tstring std::string
#endif

