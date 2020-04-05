#pragma once
#include <string>

namespace opc
{
namespace types
{
struct Range
{
    double min;
    double max;
};

class LocalizedText
{
  public:
    LocalizedText(const std::string &locale, const std::string &text)
        : loc{locale}, txt{text}
    {
    }

    std::string locale() const { return loc; }

    std::string text() const { return txt; }

  private:
    std::string loc{};
    std::string txt{};
};

class QualifiedName
{
  public:
    QualifiedName(uint16_t namespaceIndex, const std::string &name)
        : nsIdx{namespaceIndex}, n{name}
    {
    }

    uint16_t namespaceIndex() const { return nsIdx; }
    const std::string &name() const { return n; }
    //const std::string name() const {return n;}

  private:
    uint16_t nsIdx;
    std::string n;
};
} // namespace types
} // namespace opc