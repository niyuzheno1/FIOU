#pragma once
//define misc platform specific constants/types/macros
#define _USE_STD_STRING
#ifdef _WIN32
#define GCHAR TCHAR
#define dword DWORD
#ifdef UNICODE
#define to_char(x) [&] {  char ret[4500]; wcstombs(ret, x, sizeof(ret)); return string(ret); }()
#else 
#define to_char(x) x 
#endif

#else
#define GCHAR char
#define _T(x) x
#define _TEXT(x) x
#define dword size_t
#define to_char(x) (x)
#endif

const int STD_FILE_HANDLE_LIST_SIZE = 3;

#ifdef _USE_STD_STRING
#include <string>
using tstring = std::string;
#else
//user define tstring
//using tstring = std::string;
#endif