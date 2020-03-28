#include <FileService.h>
#include <experimental/filesystem>
#include <iostream>

namespace fs = std::experimental::filesystem;

std::vector<std::string> FileService::browse(const std::string &path)
{
    std::vector<std::string> files;
    if(!fs::exists(path))
    {
        return files;
    }
    for(auto& p : fs::directory_iterator(path))
    {
        files.emplace_back(p.path());
        std::cout << p.path() << std::endl;
    }
    return files;
}
