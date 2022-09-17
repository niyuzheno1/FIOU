#include "FIOUMemoryInterface.h"

class FIOUMemoryImplementor : public FIOUMemoryInterface {
public:
    void * allocate(size_t size) override;
    void * reallocate(void * pointer, size_t oldSize, size_t size) override;
    void * reallocate(void * pointer, size_t baseTypeSize, size_t oldSize, size_t newSize) override;
};