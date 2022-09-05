#pragma once
//define misc platform specific constants/types/macros
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