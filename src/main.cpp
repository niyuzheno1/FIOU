
#ifdef _WIN32
    #include <windows.h>
    #include <tchar.h>

#else
    #include <unistd.h>
    #include <errno.h>
    #include <string.h>
    #include <linux/limits.h>
#endif
#include "utils.h"
#include "enums.h"
#include "macros.h"
#include "fiou_library/export.h"
#include "MMS/export.h"
using namespace std;
using namespace MMS;


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
    int value = 0;
    for(size_t i = 0; ; ++i){
        if(a[i] == 0 && b[i] == 0){
            break;
        }
        if(a[i] != b[i]){
            return false;
        }
    }
    return true;
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
    memset(buffer, 0, fh.getFileSize() + 2);
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
   
    char * buffer =  new char[fh.getFileSize() + 2];
    memset(buffer, 0, sizeof(buffer));
    rdLine(buffer);    
    fh.restoreRedirection();
    fh.closeFile();
    return ok(buffer, "//hello world program ");
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
    wt_L("test 4");
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
    dbwt_L("test 5");
    fh.restoreRedirection();
    fh.closeFile();
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
    dbwt_L("test 6");
    fh.restoreRedirection();
    fh.closeFile();
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
    memset(buffer, 0, fh.getFileSize() + 2);
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
    tstring i;
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
    tstring line1;
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
    tstring i = "hahahah";
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
    tstring _i;

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
    tstring i = "hahahah";
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
    tstring _i;

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

void PathRemoveFileSpec(GCHAR *path)
{
    GCHAR *last = NULL;
    GCHAR *p = path;
    while (*p)
    {
        if (*p == _TEXT('\\') || *p == _TEXT('/'))
            last = p;
        p++;
    }
    if (last)
        *last = _TEXT('\0');
}
SimpleMemoryManageInstance _memoryImplementor;
SophiscatedMMInstance memoryImplementor;
char tmem[1024 * 1024 * 96];
int main()
{
    memoryImplementor.setInit([&](void ** p) {
        *p = tmem;
    });
    memoryImplementor.memInit();
    //get current running directory from process
    #ifdef _WIN32
        TCHAR szPath[MAX_PATH];
        GetModuleFileName(NULL, szPath, MAX_PATH);
        PathRemoveFileSpec(szPath);
        SetCurrentDirectory(szPath);
    #else 
        char szPath[PATH_MAX];
        //change cwd to the directory of the executable 
        readlink("/proc/self/exe", szPath, PATH_MAX);
        PathRemoveFileSpec(szPath);
        chdir(szPath);

    #endif
    FileHandle::memoryInterface = &memoryImplementor;
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