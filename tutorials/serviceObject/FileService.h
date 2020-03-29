#pragma once
#include <vector>
#include <string>


class FileService
{
public:
    std::vector<std::string> browse(std::string path);
    std::vector<std::string> getBrowsedPaths(std::string filter);
private:
    std::vector<std::string> knownPaths;
};