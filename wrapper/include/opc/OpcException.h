#pragma once
#include <stdexcept>
#include <string>

namespace opc
{
class OpcException : public std::runtime_error
{
  public:
    OpcException(const std::string &msg) : std::runtime_error{msg} {}

    virtual ~OpcException() = default;
};
} // namespace opc