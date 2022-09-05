//hello world program 
#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <stdio.h>
class FileHandle {
public:
    FileHandle(){
        #if defined(_WIN32)
            // invalid handle value
            hWindowsHandle =  INVALID_HANDLE_VALUE;
        #endif 
        hLinuxFile = -1;
        memset(fileName, 0, sizeof(fileName));
        memset(wFileName, 0, sizeof(wFileName));
        permission = 0;
        sharingMode = 0;
        openMode = 0;
    }

    void setFileName(const char * _fileName){
        strcpy(fileName, _fileName);
        mbstowcs(wFileName, _fileName, strlen(_fileName));
    }
    void setPermission(int permission){
        #if defined(_WIN32)
            this->permission = permission;
        #endif
    }
    void setSharingMode(int sharingMode){
        #if defined(_WIN32)
            this->sharingMode = sharingMode;
        #endif
    }
    void setOpenMode(int openMode){
        #if defined(_WIN32)
            this->openMode = openMode;
        #endif
    }

    void check(){
        #if defined(_WIN32)
            assert(permission != 0);
            assert(sharingMode != 0);
            assert(openMode != 0);
        #endif 
    }


    void getHandle(){
        #ifdef _WIN32
            if(hWindowsHandle == INVALID_HANDLE_VALUE){
                #ifdef _UNICODE
                    hWindowsHandle = CreateFile(wFileName, permission, sharingMode , NULL, openMode, FILE_ATTRIBUTE_NORMAL, NULL);
                #else
                    hWindowsHandle = CreateFile(fileName, permission, sharingMode , NULL,  openMode, FILE_ATTRIBUTE_NORMAL, NULL);
                #endif
            }
        #else
            // if(hLinuxFile == -1){
            //     hLinuxFile = open(fileName, O_RDONLY);
            // }
        #endif
    }
    //get file size 
    size_t getFileSize(){
        getHandle();
        #ifdef _WIN32
            DWORD dwFileSize = GetFileSize(hWindowsHandle, NULL);
            return dwFileSize;
        #else
            struct stat st;
            stat(fileName, &st);
            return st.st_size;
        #endif
    }
    //redirect file
    void redirectFile(DWORD stdHandle){
        getHandle();
        #ifdef _WIN32
            SetStdHandle(stdHandle, hWindowsHandle);
        #else
            dup2(hLinuxFile, stdHandle);
        #endif
    }
    //read from file 
    void readFromFile(){
        getHandle();
        #ifdef _WIN32
            DWORD dwFileSize = GetFileSize(hWindowsHandle, NULL);
            char * buffer = new char[dwFileSize];
            DWORD dwRead = 0;
            ReadFile(hWindowsHandle, buffer, dwFileSize, &dwRead, NULL);
            printf("%s", buffer);
            delete [] buffer;
        #else
            struct stat st;
            stat(fileName, &st);
            char * buffer = new char[st.st_size];
            read(hLinuxFile, buffer, st.st_size);
            printf("%s", buffer);
            delete [] buffer;
        #endif
    }
    // write to file
    size_t writeToFile(const char * buffer, int size){
        getHandle();
        #ifdef _WIN32
            DWORD dwWritten = 0;
            WriteFile(hWindowsHandle, buffer, size, &dwWritten, NULL);
            return dwWritten;
        #else
            write(hLinuxFile, buffer, size);
            return 0;
        #endif
    }
private: 
#if defined(_WIN32)
    HANDLE hWindowsHandle;
#endif
    DWORD openMode;
    DWORD sharingMode;
    DWORD permission;
    char fileName[4096];
    wchar_t wFileName[4096];
    int hLinuxFile;
};
int main()
{
    return 0;
}