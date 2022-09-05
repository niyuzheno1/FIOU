#pragma once
#include "FileHandle.h"
template <typename T>
inline void FileHandle::rdNumber(T &x, bool signedBy , bool isInteger )
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

template <typename T>
inline void FileHandle::wtNumber_L(T x, bool signedBy )
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