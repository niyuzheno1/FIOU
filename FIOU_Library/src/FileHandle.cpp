#include "FileHandle.h"
#ifdef _WIN32
#include <io.h>
#else 
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#endif 
#include <assert.h>


using namespace std;

size_t FileHandle::WRITER_DOUBLE_DIGIT;
size_t FileHandle::PAGENATION_CONSTANT;
FileHandle *FileHandle::stdHandleList[STD_FILE_HANDLE_LIST_SIZE];
FIOUMemoryInterface *FileHandle::memoryInterface;

FileHandle::FileHandle()
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
FileHandle::~FileHandle()
{
    this->restoreRedirection();
}

void FileHandle::setFileName(const GCHAR *name)
{
    this->fileName = name;
}

void FileHandle::check()
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

void FileHandle::getHandle()
{
#ifdef _WIN32
    if (hWindowsHandle == INVALID_HANDLE_VALUE)
    {
        DWORD dwDesiredAccess = 0;
        getNativeAccessMode(&dwDesiredAccess);
        hWindowsHandle = CreateFile(fileName, dwDesiredAccess, dwSharingMode, NULL, dwOpenMode, FILE_ATTRIBUTE_NORMAL, NULL);
        DWORD error = GetLastError();
        if (hWindowsHandle == INVALID_HANDLE_VALUE)
        {
            throw error;
        }
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
size_t FileHandle::getFileSize()
{
    getHandle();
    if (fileSize == 0)
    {
#ifdef _WIN32
        assert(hWindowsHandle != INVALID_HANDLE_VALUE);
        fileSize = GetFileSize(hWindowsHandle, NULL);
#else
        fileSize = ((struct stat *)this->getStats())->st_size;
#endif
    }

    return fileSize;
}

void FileHandle::restoreRedirection()
{
    this->flush();
    if (this->redirectionParent != nullptr)
    {
#ifdef _WIN32

        if (this->flag != UNIFIED_FILE_IN_OUT_FLAGS::NO_FLAG)
        {
            SetStdHandle(this->redirectionParent->getStdHandleFlag(), this->redirectionParent->hWindowsHandle);
        }
        this->redirectionParent->fileNumber = -1;
        int leftx = this->redirectionParent->duplicatedFIleNumberSingleAccess();
        int rightx = this->redirectionParent->deriveFileNumber();
        if(leftx >= 0 && rightx >= 0) _dup2(leftx, rightx);
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
char *FileHandle::readFromFile()
{
    if (pagnation == -1 && this->buffer)
    {
        return this->buffer;
    }
    getHandle();
    this->filePointerOffset += this->fileLastLoad;
    size_t size = this->getFileSize() - this->filePointerOffset;
    size_t readSize = min(size, pagnation);
    char *buffer = this->buffer;
    if(buffer == nullptr){
        // buffer = new char[readSize + 2];
        assert(memoryInterface != nullptr);
        buffer = (char*)memoryInterface->allocate(readSize + 2);
        memset(buffer, 0, readSize + 2);
    } 
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
size_t FileHandle::writeToFile(const char *buffer, int size)
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
void FileHandle::clear()
{
    // clear file content
    getHandle();
#ifdef _WIN32
    SetEndOfFile(hWindowsHandle);
#else
    ftruncate(hLinuxFile, 0);
#endif
}
char FileHandle::getc()
{
    assert(this != nullptr);
    assert(this->accessMode == READ_ONLY);
    if (this->bptr > pagnation - 1 || this->buffer == nullptr)
    {
        this->bptr = 0;
        this->readFromFile();
    }
    if (this->eof())
    {
        return -1;
    }
    return this->buffer[this->bptr++];
}

void FileHandle::putc(char x)
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
        assert(memoryInterface != nullptr);
        this->buffer = (char*) memoryInterface->allocate( pagnation + 2);
    }
    this->buffer[this->bptr++] = x;
}
void FileHandle::flush()
{
    if (accessMode != WRITE_ONLY)
        return;

    if (this->bptr != 0)
    {
        this->writeToFile(this->buffer, this->bptr);
        this->bptr = 0;
        this->buffer = nullptr;
    }
}

void FileHandle::rdFile(char *buffer, bool efficient )
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


void FileHandle::rd(int &x)
{
    rdNumber(x);
}

void FileHandle::rd(unsigned &x)
{
    rdNumber(x, false);
}

void FileHandle::rd(long long &x)
{
    rdNumber(x);
}

void FileHandle::rd(unsigned long long &x)
{
    rdNumber(x, false);
}
void FileHandle::rd(double &x)
{
    rdNumber(x, true, false);
}
void FileHandle::rd(float &x)
{
    rdNumber(x, true, false);
}
// void rd(__int128_t &x){
//     rdNumber(x);
// }
bool FileHandle::specialChar(char c)
{
    return c == ' ' || c == '\n' || c == '\t' || c == EOF || c == '\r';
}

bool FileHandle::eof()
{
    #ifdef _WIN32
        return this->bptr + this->filePointerOffset > this->getFileSize();
    #else
        return this->bptr + this->filePointerOffset >= this->getFileSize();
    #endif
}

void FileHandle::rd(char &c)
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

int FileHandle::rd(char c[])
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

void FileHandle::rd(tstring &x)
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
int FileHandle::rdLine(char c[])
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

int FileHandle::rdLine(tstring &s)
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
void FileHandle::closeFile()
{

    flush();
#ifdef _WIN32
    if (hWindowsHandle != INVALID_HANDLE_VALUE)
    {

        CloseHandle(hWindowsHandle);

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

void FileHandle::setGeneralWriteMode()
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
void FileHandle::setGeneralReadMode()
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
void FileHandle::FileSystemInit()
{
    PAGENATION_CONSTANT = _PAGENATION_CONSTANT;
    WRITER_DOUBLE_DIGIT = _WRITER_DOUBLE_DIGIT;
    
    assert(memoryInterface != nullptr);
    stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG] = memoryInterface->newInstance<FileHandle>();
    stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG] = memoryInterface->newInstance<FileHandle>();
    stdHandleList[UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG] = memoryInterface->newInstance<FileHandle>();

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

void FileHandle::FileSystemClose()
{
    for (int i = 0; i < 3; i++)
    {
        if (stdHandleList[i] != nullptr)
        {
            stdHandleList[i]->flush();
        }
    }
}
void FileHandle::setStdFileHandle(FILE *stream)
{
    if (stdFileHandle == nullptr)
    {
        stdFileHandle = stream;
    }
}
int FileHandle::deriveFileNumber()
{
#ifdef _WIN32
    assert(stdFileHandle != nullptr);
#else
    // stdFileHandle = fdopen(hLinuxFile, getFilePointerMode());
    fileNumber = hLinuxFile;
#endif
    if (fileNumber < 0)
    {
#ifdef _WIN32
        fileNumber = _fileno(stdFileHandle);
#else
        fileNumber = fileno(stdFileHandle);
#endif
    }

    return fileNumber;
}

const char *FileHandle::getFilePointerMode()
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
int FileHandle::duplicateFileNumber()
{
    deriveFileNumber();
#ifdef _WIN32
    duplicatedFileNumber = _dup(fileNumber);
#else
    duplicatedFileNumber = dup(fileNumber);
#endif
    return duplicatedFileNumber;
}
int FileHandle::duplicatedFIleNumberSingleAccess()
{
    if (duplicatedFileNumber < 0)
    {
        return duplicateFileNumber();
    }
    return duplicatedFileNumber;
}
void FileHandle::duplicatedOutOf(int fd)
{
    deriveFileNumber();
#ifdef _WIN32
    _dup2(fd, fileNumber);
#else
    dup2(fd, fileNumber);
#endif
}

int FileHandle::openFileNumber(int mode)
{
#ifdef _WIN32
    // reinterpret_cast  !!!!???
    fileNumber = _open_osfhandle((intptr_t)hWindowsHandle, mode);
#else
    fileNumber = hLinuxFile;
#endif
    return fileNumber;
}

void *FileHandle::getStats()
{
    deriveFileNumber();
#ifdef _WIN32
    _fstat(fileNumber, &stas);
#else
    fstat(fileNumber, &stas);
#endif
    return &stas;
}

tstring FileHandle::getDerivedAccess(){
    switch(this->flag){
        case UNIFIED_FILE_IN_OUT_FLAGS::STDIN_FLAG:
            return "r";
        case UNIFIED_FILE_IN_OUT_FLAGS::STDOUT_FLAG:
            return "w";
        case UNIFIED_FILE_IN_OUT_FLAGS::STDERR_FLAG:
            return "w";
    }
    return "rw";
}

void FileHandle::redirectInto(FileHandle *handle)
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
dword FileHandle::getStdHandleFlag()
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
void FileHandle::getNativeAccessMode(void *_ret)
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

void FileHandle::wt_L(char a)
{
    this->putc(a);
}
void FileHandle::wt_L(int x)
{
    wtNumber_L(x);
}
void FileHandle::wt_L(unsigned x)
{
    wtNumber_L(x, false);
}
void FileHandle::wt_L(long long x)
{
    wtNumber_L(x);
}
void FileHandle::wt_L(unsigned long long x)
{
    wtNumber_L(x, false);
}

int FileHandle::writerDigit_double()
{
    return WRITER_DOUBLE_DIGIT;
}
void FileHandle::writerDigit_double(int d)
{
    WRITER_DOUBLE_DIGIT = d;
}
void FileHandle::wt_L(float x)
{
    wt_L((double)x);
}
void FileHandle::wt_L(double x)
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
void FileHandle::wt_L(const char c[])
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
void FileHandle::wt_L(tstring &x)
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