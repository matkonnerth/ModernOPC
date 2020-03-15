#pragma once
#include <vector>
#include <string>


class FileService
{
public:
    uintptr_t open(const std::string& path);
    void close(uintptr_t handle);

private:
  std::vector<uintptr_t> openHandles{};
};