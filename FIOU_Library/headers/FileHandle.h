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
#include "./FIOUMemoryInterface.h"
class FileHandle
{
public:
    FileHandle();
    virtual ~FileHandle();
    virtual void setFileName(const GCHAR *name);
    virtual void check();
    virtual void getHandle();
    virtual size_t getFileSize();
    virtual void restoreRedirection();
    virtual char *readFromFile();
    virtual size_t writeToFile(const char *buffer, int size);
    virtual void clear();
    virtual char getc();
    virtual void putc(char x);
    virtual void flush();
    virtual bool specialChar(char c);
    virtual bool eof();
    virtual void closeFile();
    virtual void setGeneralWriteMode();
    virtual void setGeneralReadMode();
    virtual void setStdFileHandle(FILE *stream);
    virtual int deriveFileNumber();
    virtual const char *getFilePointerMode();
    virtual int duplicateFileNumber();
    virtual int duplicatedFIleNumberSingleAccess();
    virtual void duplicatedOutOf(int fd);
    virtual int openFileNumber(int mode);
    virtual void *getStats();
    virtual void redirectInto(FileHandle *handle);
    virtual dword getStdHandleFlag();
    virtual void getNativeAccessMode(void *_ret);
    virtual void rdFile(char *buffer, bool efficient = true);
    virtual void rd(int &x);
    virtual void rd(unsigned &x);
    virtual void rd(long long &x);
    virtual void rd(unsigned long long &x);
    virtual void rd(double &x);
    virtual void rd(float &x);
    virtual void rd(char &c);
    virtual int rd(char c[]);
    virtual void rd(tstring &x);
    virtual int rdLine(char c[]);
    virtual int rdLine(tstring &s);
    virtual void wt_L(char a);
    virtual void wt_L(int x);
    virtual void wt_L(unsigned x);
    virtual void wt_L(long long x);
    virtual void wt_L(unsigned long long x);
    virtual void wt_L(float x);
    virtual void wt_L(double x);
    virtual void wt_L(const char c[]);
    virtual void wt_L(tstring &x);
    virtual  tstring getDerivedAccess();

    static void FileSystemInit();
    static void FileSystemClose();
    static int writerDigit_double();
    static void writerDigit_double(int d);

#if _WIN32
    HANDLE hWindowsHandle;
#else
    int hLinuxFile;
#endif
    static FileHandle *stdHandleList[STD_FILE_HANDLE_LIST_SIZE];
    size_t pagnation;
    static FIOUMemoryInterface *memoryInterface;
    

protected:
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