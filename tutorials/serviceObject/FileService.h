#pragma once
#include <string>
#include <vector>

class FileService
{
  public:
    std::vector<std::string> browse(const std::string &path);
    std::vector<std::string> getBrowsedPaths(const std::string &filter);

  private:
    std::vector<std::string> knownPaths{};
};