#include "FIleProcessingFunction.h"
#include "FileHandle.h"
using namespace std;
void rdFile(char *buffer, bool efficient ){
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rdFile(buffer, efficient);
}
void rd(int &x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rd(x);
}
void rd(long long &x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rd(x);
}

void rd(unsigned &x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rd(x);
}

void rd(unsigned long long &x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rd(x);
}

void rd(double &x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rd(x);
}
void rd(float &x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rd(x);
}

void rd(char &c)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rd(c);
}

int rd(char c[])
{
    return FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rd(c);
}

void rd(string &x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rd(x);
}

int rdLine(char c[])
{
    return FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rdLine(c);
}
int rdLine(string &x)
{
    return FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rdLine(x);
}

void wt_L(const char c[])
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->wt_L(c);
}
void wt_L(string &x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->wt_L(x);
}

int writerDigit_double()
{
    return FileHandle::writerDigit_double();
}
void writerDigit_double(int d)
{
    FileHandle::writerDigit_double(d);
}
void wt_L(char a)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->wt_L(a);
}
void wt_L(float x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->wt_L(x);
}
void wt_L(double x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->wt_L(x);
}

void wt_L(int x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->wt_L(x);
}
void wt_L(unsigned x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->wt_L(x);
}
void wt_L(long long x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->wt_L(x);
}
void wt_L(unsigned long long x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->wt_L(x);
}

void dbwt_L(const char c[])
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->wt_L(c);
}
void dbwt_L(string &x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->wt_L(x);
}

void dbwt_L(char a)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->wt_L(a);
}
void dbwt_L(float x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->wt_L(x);
}
void dbwt_L(double x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->wt_L(x);
}

void dbwt_L(int x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->wt_L(x);
}
void dbwt_L(unsigned x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->wt_L(x);
}
void dbwt_L(long long x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->wt_L(x);
}
void dbwt_L(unsigned long long x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->wt_L(x);
}