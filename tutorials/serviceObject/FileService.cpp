#include <FileService.h>
#include <experimental/filesystem>
#include <iostream>

namespace fs = std::experimental::filesystem;

std::vector<std::string> FileService::browse(const std::string& path)
{
    std::vector<std::string> files;
    if(!fs::exists(path))
    {
        return files;
    }
    for(auto& p : fs::directory_iterator(path))
    {
        files.emplace_back(p.path());
    }
    knownPaths.insert(knownPaths.end(), files.begin(), files.end());
    return files;
}

std::vector<std::string> FileService::getBrowsedPaths(const std::string& filter)
{
    return knownPaths;
}
