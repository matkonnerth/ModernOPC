#pragma once
#include <stdexcept>
#include <string>

namespace modernopc
{
class OpcException : public std::runtime_error
{
  public:
    OpcException(const std::string &msg) : std::runtime_error{msg} {}

    virtual ~OpcException() = default;
};
} // namespace modernopc