// todo add the releative functionality in linux operating system
#define _PAGENATION_CONSTANT 1048576
#define _WRITER_DOUBLE_DIGIT 15
// hello world program
#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <winnt.h>
#else
#include <unistd.h>
#include <errno.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string>
#include <string.h>
#include <math.h>
using namespace std;
// struct FileComponent{
//     HANDLE hOut;
//     int fdDup;
// };

#ifdef _WIN32
#define GCHAR TCHAR
#define dword DWORD
#else
#define GCHAR char
#define _T(x) x
#define _TEXT(x) x
#define dword size_t
#endif
enum UNIFIED_FILE_IN_OUT_FLAGS
{
    STDIN_FLAG = 0,
    STDOUT_FLAG = 1,
    STDERR_FLAG = 2,
    NO_FLAG = -1
};

enum UNIFIED_FILE_ACCESS_MODE
{
    READ_ONLY = 1,
    WRITE_ONLY = 2,
    READ_WRITE = 3,
    WRITE_READ = 4,
    APPEND_ONLY = 5,
    APPEND_READ = 6,
    READ_APPEND = 7,
    WRITE_APPEND = 8,
    NO_ACCESS = -1
};
const int STD_FILE_HANDLE_LIST_SIZE = 3;
class FileHandle
{
public:
    FileHandle()
    {
#if _WIN32
        hWindowsHandle = INVALID_HANDLE_VALUE;
        dwOpenMode = 0;
        dwSharingMode = 0;
#else
        hLinuxFile = -1;
#endif
        this->accessMode = NO_ACCESS;
        this->flag = UNIFIED_FILE_IN_OUT_FLAGS::NO_FLAG;
        this->fileNumber = -1;
        this->duplicatedFileNumber = -1;
        this->bptr = 0;
        this->buffer = nullptr;
        this->pagnation = -1;
        this->fileLastLoad = 0;
        this->filePointerOffset = 0;
        this->redirectionParent = nullptr;
        this->stdFileHandle = nullptr;
        this->fileName = nullptr;
        memset(&stas, 0, sizeof(stas));
        fileSize = 0;
    }
    ~FileHandle()
    {
        this->restoreRedirection();
        this->closeFile();
    }

    void setFileName(const GCHAR *name)
    {
        this->fileName = name;
    }

    void check()
    {
#if _WIN32
        assert(this->accessMode != -1);
        assert(dwSharingMode != 0);
        assert(dwOpenMode != 0);
#else
        assert(this->accessMode != -1);
        assert(dwSharingMode != 0);
#endif
    }

    void getHandle()
    {
#ifdef _WIN32
        if (hWindowsHandle == INVALID_HANDLE_VALUE)
        {
            DWORD dwDesiredAccess = 0;
            getNativeAccessMode(&dwDesiredAccess);
            hWindowsHandle = CreateFile(fileName, dwDesiredAccess, dwSharingMode, NULL, dwOpenMode, FILE_ATTRIBUTE_NORMAL, NULL);
        }
#else
        if (hLinuxFile == -1)
        {
            int dwDesiredAccess = 0;
            getNativeAccessMode(&dwDesiredAccess);
            hLinuxFile = open(fileName, dwDesiredAccess | dwOpenMode, dwSharingMode);
            // hLinuxFile = open(fileName, O_RDONLY);
        }
#endif
    }
    // get file size
    size_t getFileSize()
    {
        getHandle();
        if (fileSize == 0)
        {
#ifdef _WIN32
            fileSize = GetFileSize(hWindowsHandle, NULL);
#else
            fileSize = ((struct stat *)this->getStats())->st_size;
#endif
        }

        return fileSize;
    }

    void restoreRedirection()
    {
        if (this->redirectionParent != nullptr)
        {
#ifdef _WIN32

            if (this->flag != UNIFIED_FILE_IN_OUT_FLAGS::NO_FLAG)
            {
                SetStdHandle(this->redirectionParent->getStdHandleFlag(), this->redirectionParent->hWindowsHandle);
            }
            this->redirectionParent->fileNumber = -1;
            _dup2(this->redirectionParent->duplicatedFIleNumberSingleAccess(), this->redirectionParent->deriveFileNumber());
            i
#else
            this->redirectionParent->fileNumber = -1;
            dup2(this->redirectionParent->duplicatedFIleNumberSingleAccess(), this->redirectionParent->deriveFileNumber());
#endif
                if (this->flag != UNIFIED_FILE_IN_OUT_FLAGS::NO_FLAG)
            {
                stdHandleList[flag] = this->redirectionParent;
            }
            this->redirectionParent = nullptr;
        }
    }

    // read from file
    char *readFromFile()
    {
        if (this->buffer)
        {
            return this->buffer;
        }
        getHandle();
        this->filePointerOffset += this->fileLastLoad;
        size_t size = this->getFileSize() - this->filePointerOffset;
        size_t readSize = min(size, pagnation);
        char *buffer = new char[readSize + 2];
#ifdef _WIN32
        DWORD dwRead = 0;
        memset(buffer, 0, readSize + 2);
        ReadFile(hWindowsHandle, buffer, readSize, &dwRead, NULL);
        fileLastLoad = dwRead;
        this->buffer = buffer;
#else
        read(hLinuxFile, buffer, readSize);
        fileLastLoad = readSize;
        this->buffer = buffer;
#endif
        return this->buffer;
    }
    // write to file
    size_t writeToFile(const char *buffer, int size)
    {
        getHandle();
#ifdef _WIN32
        DWORD dwWritten = 0;
        WriteFile(hWindowsHandle, buffer, size, &dwWritten, NULL);
        return dwWritten;
#else
        int status = write(hLinuxFile, buffer, size);
        int errorStatus = errno;
        if (status > 0)
        {
            return status;
        }
        else
        {
            return 0;
        }
#endif
    }
    void clear()
    {
        // clear file content
        getHandle();
#ifdef _WIN32
        SetEndOfFile(hWindowsHandle);
#else
        ftruncate(hLinuxFile, 0);
#endif
    }
    char getc()
    {
        assert(this != nullptr);
        assert(this->accessMode == READ_ONLY);
        if (this->bptr > pagnation - 1)
        {
            if (this->buffer != nullptr)
            {
                delete[] this->buffer;
            }
            this->buffer = nullptr;
            this->bptr = 0;
        }
        if (this->buffer == nullptr)
        {
            this->readFromFile();
        }
        if (this->eof())
        {
            return -1;
        }
        return this->buffer[this->bptr++];
    }

    void putc(char x)
    {
        assert(this != nullptr);
        assert(this->accessMode == WRITE_ONLY);
        if (this->bptr > pagnation - 1)
        {
            flush();
        }
        if (pagnation == -1)
        {
            this->writeToFile(&x, 1);
            return;
        }
        if (this->buffer == nullptr)
        {
            this->buffer = new char[pagnation + 2];
        }
        this->buffer[this->bptr++] = x;
    }
    void flush()
    {
        if (this->buffer != nullptr)
        {
            this->writeToFile(this->buffer, this->bptr);
            delete[] this->buffer;
            this->bptr = 0;
            this->buffer = nullptr;
        }
    }

    void rdFile(char *buffer, bool efficient = true)
    {
        if (efficient)
        {
            this->pagnation = -1;
            this->readFromFile();
            memcpy(buffer, this->buffer, this->getFileSize());
        }
        else
        {
            char c = 0;
            int i = 0;
            while ((c = this->getc()) != -1)
            {
                buffer[i++] = c;
            }
        }
    }
    template <typename T>
    inline void rdNumber(T &x, bool signedBy = true, bool isInteger = true)
    {
        int k;
        int m = 0;
        int p = 0;
        double r = 1;
        x = 0;
        bool isDouble = !isInteger;
        for (;;)
        {
            k = this->getc();
            if (k == '-' && signedBy)
            {
                m = 1;
                break;
            }
            if (k == '.' && isDouble)
            {
                p = 1;
                break;
            }
            if ('0' <= k && k <= '9')
            {
                x = k - '0';
                break;
            }
        }
        for (;;)
        {
            k = this->getc();
            if (k == '.' && isDouble)
            {
                p = 1;
                continue;
            }
            if (k < '0' || k > '9')
                break;
            if (p && isDouble)
            {
                r *= 0.1;
                x += (k - '0') * r;
            }
            else
            {
                x = x * 10 + k - '0';
            }
        }
        if (m && signedBy)
            x = -x;
    }

    inline void rd(int &x)
    {
        rdNumber(x);
    }

    inline void rd(unsigned &x)
    {
        rdNumber(x, false);
    }

    inline void rd(long long &x)
    {
        rdNumber(x);
    }

    inline void rd(unsigned long long &x)
    {
        rdNumber(x, false);
    }
    inline void rd(double &x)
    {
        rdNumber(x, true, false);
    }
    inline void rd(float &x)
    {
        rdNumber(x, true, false);
    }
    // void rd(__int128_t &x){
    //     rdNumber(x);
    // }
    bool specialChar(char c)
    {
        return c == ' ' || c == '\n' || c == '\t' || c == EOF || c == '\r';
    }

    bool eof()
    {
        return this->bptr + this->filePointerOffset > this->getFileSize();
    }

    inline void rd(char &c)
    {
        int i;
        for (;;)
        {
            i = this->getc();
            if (!specialChar(i))
                break;
        }
        c = i;
    }

    inline int rd(char c[])
    {
        int i;
        int sz = 0;
        rd(c[sz++]);
        for (;;)
        {
            i = this->getc();
            if (specialChar(i))
                break;
            c[sz++] = i;
        }
        c[sz] = '\0';
        return sz;
    }

    inline void rd(string &x)
    {
        x.reserve(100);
        char firstx = 0;
        rd(firstx);
        x.push_back(firstx);
        for (;;)
        {
            char i = this->getc();
            if (specialChar(i))
                break;
            x.push_back(i);
        }
        x.push_back('\0');
    }
    inline int rdLine(char c[])
    {
        int i;
        int sz = 0;
        for (;;)
        {
            i = this->getc();
            if (i == '\r')
                continue;
            if (i == '\n')
                break;
            if (i == EOF)
            {
                if (sz == 0)
                {
                    c[sz] = '\0';
                    return -1;
                }
                break;
            }
            c[sz++] = i;
        }
        c[sz] = '\0';
        return sz;
    }

    inline int rdLine(string &s)
    {
        s.reserve(100);
        char firstx = 0;
        rd(firstx);
        s.push_back(firstx);
        for (;;)
        {
            char i = this->getc();
            if (i == '\r')
                continue;
            if (i == '\n')
                break;
            if (i == EOF)
            {
                if (s.size() == 1)
                {
                    s.push_back('\0');
                    return -1;
                }
                break;
            }
            s.push_back(i);
        }
        s.push_back('\0');
        return s.size();
    }
    void closeFile()
    {
        if (accessMode == WRITE_ONLY)
        {
            flush();
        }
#ifdef _WIN32
        if (hWindowsHandle != INVALID_HANDLE_VALUE)
        {
            __try
            {
                CloseHandle(hWindowsHandle);
            }
            __except (EXCEPTION_INVALID_HANDLE)
            {
            }

            hWindowsHandle = INVALID_HANDLE_VALUE;
        }
#else
        if (hLinuxFile != -1)
        {
            close(hLinuxFile);
            hLinuxFile = -1;
        }
#endif
    }

    void setGeneralWriteMode()
    {
#ifdef _WIN32
        dwOpenMode = OPEN_ALWAYS;
        dwSharingMode = FILE_SHARE_WRITE | FILE_SHARE_READ;
#else
        dwOpenMode = O_CREAT;
        dwSharingMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
#endif
        this->accessMode = ::UNIFIED_FILE_ACCESS_MODE::WRITE_ONLY;
        this->pagnation = PAGENATION_CONSTANT;
    }
    void setGeneralReadMode()
    {
#ifdef _WIN32
        dwOpenMode = OPEN_EXISTING;
        dwSharingMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
#else
        dwOpenMode = 0;
        dwSharingMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
#endif
        this->accessMode = ::UNIFIED_FILE_ACCESS_MODE::READ_ONLY;
    }
    static void FileSystemInit()
    {
        PAGENATION_CONSTANT = _PAGENATION_CONSTANT;
        WRITER_DOUBLE_DIGIT = _WRITER_DOUBLE_DIGIT;
        stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG] = new FileHandle();
        stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG] = new FileHandle();
        stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG] = new FileHandle();

#ifdef _WIN32
        stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->hWindowsHandle = GetStdHandle(STD_INPUT_HANDLE);
        stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->hWindowsHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->hWindowsHandle = GetStdHandle(STD_ERROR_HANDLE);
#else
        stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->hLinuxFile = STDIN_FILENO;
        stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->hLinuxFile = STDOUT_FILENO;
        stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->hLinuxFile = STDERR_FILENO;
#endif
        stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->flag = UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG;
        stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->flag = UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG;
        stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->flag = UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG;
        stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->setStdFileHandle(stdin);
        stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->setStdFileHandle(stdout);
        stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->setStdFileHandle(stderr);
        stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->setGeneralReadMode();
        stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->setGeneralWriteMode();
        stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->setGeneralWriteMode();
    }

    static void FileSystemClose()
    {
        for (int i = 0; i < 3; i++)
        {
            if (stdHandleList[i] != nullptr)
            {
                stdHandleList[i]->closeFile();
                stdHandleList[i] = nullptr;
            }
        }
    }
    void setStdFileHandle(FILE *stream)
    {
        if (stdFileHandle == nullptr)
        {
            stdFileHandle = stream;
        }
    }
    int deriveFileNumber()
    {
#ifdef _WIN32
        assert(stdFileHandle != nullptr);
#else
        // stdFileHandle = fdopen(hLinuxFile, getFilePointerMode());
        fileNumber = hLinuxFile;
#endif
        if (fileNumber == -1)
        {
#ifdef _WIN32
            fileNumber = _fileno(stdFileHandle);
#else
            fileNumber = fileno(stdFileHandle);
#endif
        }

        return fileNumber;
    }

    const char *getFilePointerMode()
    {
        if (accessMode == READ_ONLY)
        {
            return "r";
        }
        else if (accessMode == WRITE_ONLY)
        {
            return "rw";
        }
        else if (accessMode == READ_WRITE)
        {
            return "r+";
        }
        else if (accessMode == WRITE_READ)
        {
            return "w+";
        }
        else if (accessMode == APPEND_ONLY)
        {
            return "a";
        }
        else if (accessMode == APPEND_READ)
        {
            return "a+";
        }
        else if (accessMode == READ_APPEND)
        {
            return "r+a";
        }
        else if (accessMode == WRITE_APPEND)
        {
            return "w+a";
        }
        else
        {
            return "r";
        }
    }
    int duplicateFileNumber()
    {
        deriveFileNumber();
#ifdef _WIN32
        duplicatedFileNumber = _dup(fileNumber);
#else
        duplicatedFileNumber = dup(fileNumber);
#endif
        return duplicatedFileNumber;
    }
    int duplicatedFIleNumberSingleAccess()
    {
        if (duplicatedFileNumber == -1)
        {
            return duplicateFileNumber();
        }
        return duplicatedFileNumber;
    }
    void duplicatedOutOf(int fd)
    {
        deriveFileNumber();
#ifdef _WIN32
        _dup2(fd, fileNumber);
#else
        dup2(fd, fileNumber);
#endif
    }

    int openFileNumber(int mode)
    {
#ifdef _WIN32
        fileNumber = _open_osfhandle((long)hWindowsHandle, mode);
#else
        fileNumber = hLinuxFile;
#endif
        return fileNumber;
    }

    void *getStats()
    {
        deriveFileNumber();
#ifdef _WIN32
        _fstat(fileNumber, &stas);
#else
        fstat(fileNumber, &stas);
#endif
        return &stas;
    }

    void redirectInto(FileHandle *handle)
    {
        getHandle();
        handle->duplicatedFIleNumberSingleAccess();
#ifdef _WIN32

        struct _stat *stas = (struct _stat *)handle->getStats();
        DWORD handleFlag = handle->getStdHandleFlag();
        if (handleFlag != -1)
        {
            SetStdHandle(handleFlag, this->hWindowsHandle);
        }
        int fd = this->openFileNumber(stas->st_mode);

        // dup2(hLinuxFile, stdHandle);
#else
        struct stat *stas = (struct stat *)handle->getStats();
        int fd = this->openFileNumber(stas->st_mode);
#endif

        handle->duplicatedOutOf(fd);
        if (handle->flag != NO_FLAG)
        {
            stdHandleList[handle->flag] = this;
            this->flag = handle->flag;
        }
        this->redirectionParent = handle;
    }
    dword getStdHandleFlag()
    {
#ifdef _WIN32
        switch (flag)
        {
        case UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG:
            return STD_INPUT_HANDLE;
        case UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG:
            return STD_OUTPUT_HANDLE;
        case UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG:
            return STD_ERROR_HANDLE;
        default:
            break;
        }
        return -1;
#else
        return 0;
#endif
    }
    void getNativeAccessMode(void *_ret)
    {
#ifdef _WIN32
        DWORD *ret = (DWORD *)_ret;
        if (accessMode & UNIFIED_FILE_ACCESS_MODE::READ_ONLY)
        {
            *ret |= GENERIC_READ;
        }
        if (accessMode & UNIFIED_FILE_ACCESS_MODE::WRITE_ONLY)
        {
            *ret |= GENERIC_WRITE;
        }
#else
        int *ret = (int *)_ret;
        if (accessMode & UNIFIED_FILE_ACCESS_MODE::READ_ONLY)
        {
            *ret |= O_RDONLY;
        }
        if (accessMode & UNIFIED_FILE_ACCESS_MODE::WRITE_ONLY)
        {
            *ret |= O_RDWR;
        }
        if (accessMode & UNIFIED_FILE_ACCESS_MODE::READ_WRITE)
        {
            *ret |= O_RDWR;
        }

#endif
    }

#if _WIN32
    HANDLE hWindowsHandle;
#else
    int hLinuxFile;
#endif
    static FileHandle *stdHandleList[STD_FILE_HANDLE_LIST_SIZE];
    size_t pagnation;
    inline void wt_L(char a)
    {
        this->putc(a);
    }
    template <typename T>
    inline void wtNumber_L(T x, bool signedBy = true)
    {
        int s = 0;
        int m = 0;
        char f[50];
        for (int i = 0; i < 50; i++)
        {
            f[i] = 0;
        }
        if (x < 0 && signedBy)
        {
            m = 1;
            x = -x;
        }
        while (x)
        {
            f[s++] = x % 10;
            x /= 10;
        }

        if (!s)
        {
            f[s++] = 0;
        }
        if (pagnation == -1)
        {
            this->writeToFile(f, s);
            return;
        }
        if (m && signedBy)
        {
            this->putc('-');
        }
        while (s--)
        {
            this->putc(f[s] + '0');
        }
    }
    inline void wt_L(int x)
    {
        wtNumber_L(x);
    }
    inline void wt_L(unsigned x)
    {
        wtNumber_L(x, false);
    }
    inline void wt_L(long long x)
    {
        wtNumber_L(x);
    }
    inline void wt_L(unsigned long long x)
    {
        wtNumber_L(x, false);
    }
    // inline void wt_L(__int128_t x)
    // {
    //     wtNumber_L(x);
    // }
    static inline int writerDigit_double()
    {
        return WRITER_DOUBLE_DIGIT;
    }
    static inline void writerDigit_double(int d)
    {
        WRITER_DOUBLE_DIGIT = d;
    }
    inline void wt_L(float x)
    {
        wt_L((double)x);
    }
    inline void wt_L(double x)
    {
        const int d = WRITER_DOUBLE_DIGIT;
        int k;
        int r;
        double v;
        if (x != x || (x == x + 1 && x == 2 * x))
        {
            if (pagnation == -1)
            {
                this->writeToFile("Err", 3);
                return;
            }
            this->putc('E');
            this->putc('r');
            this->putc('r');
            return;
        }
        if (pagnation == -1)
        {
            char f[40];
            char format[10];
            sprintf(format, "%%.%dlf", d);
            sprintf(f, format, x);
            this->writeToFile(f, strlen(f));
            return;
        }
        if (x < 0)
        {
            this->putc('-');
            x = -x;
        }
        x += 0.5 * pow(0.1, d);
        r = 0;
        v = 1;
        while (x >= 10 * v)
        {
            v *= 10;
            r++;
        }
        while (r >= 0)
        {
            r--;
            k = floor(x / v);
            if (k >= 10)
            {
                k = 9;
            }
            if (k <= -1)
            {
                k = 0;
            }
            x -= k * v;
            v *= 0.1;
            this->putc(k + '0');
        }
        if (d > 0)
        {
            this->putc('.');
            v = 1;
            for (r = (0); r < (d); r++)
            {
                v *= 0.1;
                k = floor(x / v);
                if (k >= 10)
                {
                    k = 9;
                }
                if (k <= -1)
                {
                    k = 0;
                }
                x -= k * v;
                this->putc(k + '0');
            }
        }
    }
    inline void wt_L(const char c[])
    {
        if (pagnation == -1)
        {
            this->writeToFile(c, strlen(c));
            return;
        }
        int i = 0;
        for (i = 0; c[i] != '\0'; i++)
            this->putc(c[i]);
    }
    inline void wt_L(string &x)
    {
        if (pagnation == -1)
        {
            this->writeToFile(x.c_str(), x.length());
            return;
        }
        int i = 0;
        for (i = 0; x[i] != '\0'; i++)
            this->putc(x[i]);
    }

private:
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
size_t FileHandle::WRITER_DOUBLE_DIGIT;
size_t FileHandle::PAGENATION_CONSTANT;
FileHandle *FileHandle::stdHandleList[STD_FILE_HANDLE_LIST_SIZE];

inline void rd(int &x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rd(x);
}
inline void rd(long long &x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rd(x);
}

inline void rd(unsigned &x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rd(x);
}

inline void rd(unsigned long long &x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rd(x);
}

inline void rd(double &x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rd(x);
}
inline void rd(float &x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rd(x);
}

inline void rd(char &c)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rd(c);
}

inline int rd(char c[])
{
    return FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rd(c);
}

inline void rd(string &x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rd(x);
}

inline int rdLine(char c[])
{
    return FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rdLine(c);
}
inline int rdLine(string &x)
{
    return FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]->rdLine(x);
}

inline void wt_L(const char c[])
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->wt_L(c);
}
inline void wt_L(string &x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->wt_L(x);
}

static inline int writerDigit_double()
{
    return FileHandle::writerDigit_double();
}
static inline void writerDigit_double(int d)
{
    FileHandle::writerDigit_double(d);
}
inline void wt_L(char a)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->wt_L(a);
}
inline void wt_L(float x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->wt_L(x);
}
inline void wt_L(double x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->wt_L(x);
}

inline void wt_L(int x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->wt_L(x);
}
inline void wt_L(unsigned x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->wt_L(x);
}
inline void wt_L(long long x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->wt_L(x);
}
inline void wt_L(unsigned long long x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]->wt_L(x);
}

inline void dbwt_L(const char c[])
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->wt_L(c);
}
inline void dbwt_L(string &x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->wt_L(x);
}

inline void dbwt_L(char a)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->wt_L(a);
}
inline void dbwt_L(float x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->wt_L(x);
}
inline void dbwt_L(double x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->wt_L(x);
}

inline void dbwt_L(int x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->wt_L(x);
}
inline void dbwt_L(unsigned x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->wt_L(x);
}
inline void dbwt_L(long long x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->wt_L(x);
}
inline void dbwt_L(unsigned long long x)
{
    FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]->wt_L(x);
}

// write test for the rd functions

char *readFrom(const char *fileName)
{
    FILE *file = fopen(fileName, "rb");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = new char[size + 2];
    memset(buffer, 0, size + 2);
    fread(buffer, 1, size, file);
    fclose(file);
    return buffer;
}

bool ok(const char *a, const char *b)
{
    if (strcmp(a, b) == 0)
    {
        return true;
    }
    return false;
}

template <typename T>
bool okValue(T a, T b)
{
    if (a == b)
    {
        return true;
    }
    if (abs((double)a - (double)b) < 0.0001)
    {
        return true;
    }
    return false;
}

bool testCase1()
{
    // reading from file hello.cpp
    FileHandle fh;
    fh.setFileName(_TEXT("hello.cpp"));
    fh.setGeneralReadMode();
    fh.check();
    char *buffer = new char[fh.getFileSize() + 2];
    fh.rdFile(buffer, true);
    fh.closeFile();
    return ok(buffer, readFrom("hello.cpp"));
}

bool testCase2()
{
    // test if we can output to a file hello.txt
    FileHandle fh;
    fh.setFileName(_TEXT("hello.txt"));
    fh.setGeneralWriteMode();
    fh.check();
    fh.clear();
    fh.writeToFile("hello world", strlen("hello world"));
    fh.closeFile();
    return ok(readFrom("hello.txt"), "hello world");
}

bool testCase3()
{
    // test if we can redirect input from a file hello.cpp
    FileHandle fh;
    fh.setFileName(_TEXT("hello.cpp"));
    fh.setGeneralReadMode();
    fh.check();
    fh.redirectInto(FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]);
    char buffer[4096];
    fgets(buffer, 4096, stdin);
    fh.restoreRedirection();
    fh.closeFile();
    return ok(buffer, "//hello world program \r\n");
}

bool testCase4()
{
    // test if we can redirect output to a file hello.txt
    FileHandle fh;
    fh.setFileName(_TEXT("hello.txt"));
    fh.setGeneralWriteMode();
    fh.check();
    fh.clear();
    fh.redirectInto(FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]);
    printf("test 4");
    // flush
    fflush(stdout);
    fh.restoreRedirection();
    fh.closeFile();
    return ok(readFrom("hello.txt"), "test 4");
}

bool testCase5()
{
    // test if we can redirect error to a file hello.txt
    FileHandle fh;
    fh.setFileName(_TEXT("hello.txt"));
    fh.setGeneralWriteMode();
    fh.check();
    fh.clear();
    fh.redirectInto(FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]);
    fprintf(stderr, "test 5");
    fh.restoreRedirection();
    return ok(readFrom("hello.txt"), "test 5");
}

bool testCase6()
{
    // test if we can truncate file content
    FileHandle fh;
    fh.setFileName(_TEXT("hello.txt"));
    fh.setGeneralWriteMode();
    fh.check();
    fh.clear();
    fh.redirectInto(FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]);
    fprintf(stderr, "test 6");
    fh.restoreRedirection();
    return ok(readFrom("hello.txt"), "test 6");
}

bool testCase7()
{
    // test if we can redirect input from a file i1.txt and read the integer from it
    FileHandle fh;
    fh.setFileName(_TEXT("i1.txt"));
    fh.setGeneralReadMode();
    fh.check();
    fh.redirectInto(FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]);
    int a;
    rd(a);
    fh.restoreRedirection();
    fh.closeFile();
    return a == 300;
}

bool testCase8()
{
    // test inefficient read
    // reading from file hello.cpp
    FileHandle fh;
    fh.setFileName(_TEXT("hello.cpp"));
    fh.setGeneralReadMode();
    fh.check();
    char *buffer = new char[fh.getFileSize() + 2];
    fh.rdFile(buffer, false);
    fh.closeFile();
    return ok(buffer, readFrom("hello.cpp"));
}

bool testCase9()
{
    // test read pagnation
    // reading from file hello.cpp
    FileHandle fh;
    fh.pagnation = 5;
    fh.setFileName(_TEXT("hello.cpp"));
    fh.setGeneralReadMode();
    fh.check();
    char *buffer = new char[fh.getFileSize() + 2];
    fh.rdFile(buffer, false);
    fh.closeFile();
    return ok(buffer, readFrom("hello.cpp"));
}

bool testCase10()
{
    // more reading test without pagnation
    FileHandle fh;
    fh.setFileName(_TEXT("i2.txt"));
    fh.setGeneralReadMode();
    fh.check();
    fh.redirectInto(FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]);
    int a;
    rd(a);
    long long b;
    rd(b);
    unsigned int c;
    rd(c);
    unsigned long long d;
    rd(d);
    double e;
    rd(e);
    float f;
    rd(f);
    char g;
    rd(g);
    char h[100];
    rd(h);
    string i;
    rd(i);
    fh.restoreRedirection();
    fh.closeFile();
    bool isOkay = true;
    isOkay &= okValue(a, -3);
    isOkay &= okValue(b, -1000000000000000000LL);
    isOkay &= okValue(c, 2521U);
    isOkay &= okValue(d, 1000231231243021ULL);
    isOkay &= okValue(e, 3.141592647824812498124);
    isOkay &= okValue(f, 3.11111111f);
    isOkay &= okValue(g, 'a');
    isOkay &= ok(h, "ahahaha");
    isOkay &= ok(i.c_str(), "hahahah");
    return isOkay;
}

bool testCase11()
{
    // more reading test without pagnation
    FileHandle fh;
    fh.setFileName(_TEXT("i3.txt"));
    fh.setGeneralReadMode();
    fh.check();
    fh.redirectInto(FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]);
    string line1;
    char line2[100];
    rdLine(line1);
    rdLine(line2);
    fh.restoreRedirection();
    fh.closeFile();
    bool isOkay = true;
    isOkay &= ok(line1.c_str(), "Hello how are you?");
    isOkay &= ok(line2, "I am fine, thank you.");
    return isOkay;
}

bool testCase12()
{
    // more reading test without pagnation
    FileHandle fh;
    fh.setFileName(_TEXT("i4.txt"));
    fh.setGeneralWriteMode();
    fh.check();
    fh.redirectInto(FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG]);
    int a = -3;
    long long b = -1000000000000000000L;
    unsigned int c = 2521U;
    unsigned long long d = 1000231231243021ULL;
    double e = 3.141592647824812498124;
    float f = 3.11111111f;
    char g = 'a';
    char h[100];
    strcpy(h, "ahahaha");
    string i = "hahahah";
    wt_L(a);
    wt_L(" ");
    wt_L(b);
    wt_L(" ");
    wt_L(c);
    wt_L(" ");
    wt_L(d);
    wt_L(" ");
    wt_L(e);
    wt_L(" ");
    wt_L(f);
    wt_L(" ");
    wt_L(g);
    wt_L(" ");
    wt_L(h);
    wt_L(" ");
    wt_L(i);
    wt_L(" ");
    fh.restoreRedirection();
    fh.closeFile();

    // assume that our std read is working properly
    FileHandle fh2;
    fh2.setFileName(_TEXT("i4.txt"));
    fh2.setGeneralReadMode();
    fh2.check();
    fh2.redirectInto(FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]);
    int _a;
    long long _b;
    unsigned int _c;
    unsigned long long _d;
    double _e;
    float _f;
    char _g;
    char _h[100];
    string _i;

    rd(_a);
    rd(_b);
    rd(_c);
    rd(_d);
    rd(_e);
    rd(_f);
    rd(_g);
    rd(_h);
    rd(_i);
    fh2.restoreRedirection();
    fh2.closeFile();

    bool isOkay = true;
    isOkay &= okValue(_a, a);
    isOkay &= okValue(_b, b);
    isOkay &= okValue(_c, c);
    isOkay &= okValue(_d, d);
    isOkay &= okValue(_e, e);
    isOkay &= okValue(_f, f);
    isOkay &= okValue(_g, g);
    isOkay &= ok(_h, h);
    isOkay &= ok(_i.c_str(), i.c_str());

    return isOkay;
}

bool testCase13()
{
    // more reading test without pagnation
    FileHandle fh;
    fh.setFileName(_TEXT("i5.txt"));
    fh.setGeneralWriteMode();
    fh.check();
    fh.clear();
    fh.redirectInto(FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG]);
    int a = -3;
    long long b = -1000000000000000000L;
    unsigned int c = 2521U;
    unsigned long long d = 1000231231243021ULL;
    double e = 3.141592647824812498124;
    float f = 3.11111111f;
    char g = 'a';
    char h[100];
    strcpy(h, "ahahaha");
    string i = "hahahah";
    dbwt_L(a);
    dbwt_L(" ");
    dbwt_L(b);
    dbwt_L(" ");
    dbwt_L(c);
    dbwt_L(" ");
    dbwt_L(d);
    dbwt_L(" ");
    dbwt_L(e);
    dbwt_L(" ");
    dbwt_L(f);
    dbwt_L(" ");
    dbwt_L(g);
    dbwt_L(" ");
    dbwt_L(h);
    dbwt_L(" ");
    dbwt_L(i);
    dbwt_L(" ");
    fh.restoreRedirection();
    fh.closeFile();

    // assume that our std read is working properly
    FileHandle fh2;
    fh2.setFileName(_TEXT("i5.txt"));
    fh2.setGeneralReadMode();
    fh2.check();
    fh2.redirectInto(FileHandle::stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG]);
    int _a;
    long long _b;
    unsigned int _c;
    unsigned long long _d;
    double _e;
    float _f;
    char _g;
    char _h[100];
    string _i;

    rd(_a);
    rd(_b);
    rd(_c);
    rd(_d);
    rd(_e);
    rd(_f);
    rd(_g);
    rd(_h);
    rd(_i);
    fh2.restoreRedirection();
    fh2.closeFile();

    bool isOkay = true;
    isOkay &= okValue(_a, a);
    isOkay &= okValue(_b, b);
    isOkay &= okValue(_c, c);
    isOkay &= okValue(_d, d);
    isOkay &= okValue(_e, e);
    isOkay &= okValue(_f, f);
    isOkay &= okValue(_g, g);
    isOkay &= ok(_h, h);
    isOkay &= ok(_i.c_str(), i.c_str());

    return isOkay;
}

typedef bool (*testFunc)();
testFunc tests[] = {
    testCase1, testCase2, testCase3, testCase4, testCase5, testCase6, testCase7, testCase8, testCase9, testCase10, testCase11, testCase12, testCase13};

bool results[] = {
    true, true, true, true, true, true, true, true, true, true, true, true, true};

FileHandle fhstdin, fhstdout, fhstderr;

int main()
{
    FileHandle::FileSystemInit();
    for (int i = 0; i < sizeof(tests) / sizeof(testFunc); i++)
    {
        wt_L("test case ");
        wt_L(i + 1);
        if (tests[i]())
        {
            wt_L(" passed");
        }
        else
        {
            wt_L(" failed");
            results[i] = false;
        }
        wt_L("\n");
    }
    FileHandle::FileSystemClose();
    return 0;
}
