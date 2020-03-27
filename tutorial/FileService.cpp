#include <FileService.h>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

std::vector<std::string> FileService::browse(const std::string &path)
{
    std::vector<std::string> files;
    for(auto& p : fs::directory_iterator(path))
    {
        files.emplace_back(p.path());
        std::cout << p.path() << std::endl;
    }
    return files;
}
