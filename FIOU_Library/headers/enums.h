#pragma once
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