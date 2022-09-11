#pragma once
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#endif
#include <string>
#include "./utils.h"
#include "./enums.h"
#include "./macros.h"
class FileHandle
{
public:
    FileHandle();
    ~FileHandle();
    void setFileName(const GCHAR *name);
    void check();
    void getHandle();
    size_t getFileSize();
    void restoreRedirection();
    char *readFromFile();
    size_t writeToFile(const char *buffer, int size);
    void clear();
    char getc();
    void putc(char x);
    void flush();
    bool specialChar(char c);
    bool eof();
    void closeFile();
    void setGeneralWriteMode();
    void setGeneralReadMode();
    static void FileSystemInit();
    static void FileSystemClose();
    void setStdFileHandle(FILE *stream);
    int deriveFileNumber();
    const char *getFilePointerMode();
    int duplicateFileNumber();
    int duplicatedFIleNumberSingleAccess();
    void duplicatedOutOf(int fd);
    int openFileNumber(int mode);
    void *getStats();
    void redirectInto(FileHandle *handle);
    dword getStdHandleFlag();
    void getNativeAccessMode(void *_ret);
    static int writerDigit_double();
    static void writerDigit_double(int d);
    void rdFile(char *buffer, bool efficient = true);
    void rd(int &x);
    void rd(unsigned &x);
    void rd(long long &x);
    void rd(unsigned long long &x);
    void rd(double &x);
    void rd(float &x);
    void rd(char &c);
    int rd(char c[]);
    void rd(tstring &x);
    int rdLine(char c[]);
    int rdLine(tstring &s);
    void wt_L(char a);
    void wt_L(int x);
    void wt_L(unsigned x);
    void wt_L(long long x);
    void wt_L(unsigned long long x);
    void wt_L(float x);
    void wt_L(double x);
    void wt_L(const char c[]);
    void wt_L(tstring &x);
    tstring getDerivedAccess();
#if _WIN32
    HANDLE hWindowsHandle;
#else
    int hLinuxFile;
#endif
    static FileHandle *stdHandleList[STD_FILE_HANDLE_LIST_SIZE];
    size_t pagnation;

private:
    template <typename T>
    inline void rdNumber(T &x, bool signedBy = true, bool isInteger = true);
    template <typename T>
    inline void wtNumber_L(T x, bool signedBy = true);

    UNIFIED_FILE_IN_OUT_FLAGS flag;
#if _WIN32
    DWORD dwSharingMode;
    DWORD dwOpenMode;
    struct _stat stas;
#else
    struct stat stas;
    int dwSharingMode;
    int dwOpenMode;
    // TODO: we will set permissions, sharing mode and open mode in linux
#endif
    UNIFIED_FILE_ACCESS_MODE accessMode;
    char *buffer;
    const GCHAR *fileName;
    size_t bptr;
    int fileNumber;
    int duplicatedFileNumber;
    FILE *stdFileHandle;
    FileHandle *redirectionParent;
    size_t fileSize;
    size_t filePointerOffset;
    size_t fileLastLoad;
    static size_t PAGENATION_CONSTANT;
    static size_t WRITER_DOUBLE_DIGIT;
};

#include "FileHandleTP.h"