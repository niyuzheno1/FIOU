#include "FIOUMemoryImplementor.h"
#include <cstdlib>
void * FIOUMemoryImplementor::allocate(size_t size){
    return malloc(size);
}
void * FIOUMemoryImplementor::reallocate(void * pointer, size_t oldSize, size_t size){
    return realloc(pointer, size);
}
void * FIOUMemoryImplementor::reallocate(void * pointer, size_t baseTypeSize, size_t oldSize, size_t newSize){
    return realloc(pointer, baseTypeSize * newSize);
}