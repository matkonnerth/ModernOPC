#pragma once
#include <vector>
#include <string>


class FileService
{
public:
    std::vector<std::string> browse(const std::string&path);

private:
  //std::vector<uintptr_t> openHandles{};
};