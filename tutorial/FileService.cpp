#include <FileService.h>
#include <cstdio>

struct FileHandle
{
    FILE* f;
};

uintptr_t FileService::open(const std::string &path)
{
    FILE *f = fopen(path.c_str(), "r");
    if(f)
    {

        openHandles.push_back((uintptr_t)f);
        return (uintptr_t)f;
    }
    return 0;
}

void
FileService::close(uintptr_t handle) {
    for(auto hdl : openHandles)
    {
        if(handle==hdl)
        {
            fclose((FILE*)hdl);
        }
    }
}