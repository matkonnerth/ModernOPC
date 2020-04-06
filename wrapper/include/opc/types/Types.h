#pragma once
#include <string>
#include <iostream>
#include <open62541/types.h>


struct UA_Variant;
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

    LocalizedText(const LocalizedText& other) {
        loc = other.loc;
        txt = other.txt;
        std::cout << "copy ctor called\n";
    }

    LocalizedText& operator=(const LocalizedText& other)
    {
      if(&other==this)
      {
        return *this;
      }
      loc = other.loc;
      txt = other.txt;
      std::cout << "copy assign called\n";
      return *this;
    }

    LocalizedText(LocalizedText&& other) = default;
    LocalizedText& operator=(LocalizedText&& other)=default;

    std::string locale() const { return loc; }
    std::string text() const { return txt; }

  private:
    std::string loc{};
    std::string txt{};
};
std::ostream &operator<<(std::ostream &os, const LocalizedText &qn);
void convertToUAVariantImpl(const opc::types::LocalizedText &m,
                                UA_Variant *var);
opc::types::LocalizedText fromUALocalizedText(const UA_LocalizedText *lt);
UA_LocalizedText fromLocalizedText(const opc::types::LocalizedText &lt);

class QualifiedName
{
  public:
    QualifiedName(uint16_t namespaceIndex, const std::string &name)
        : nsIdx{namespaceIndex}, n{name}
    {
    }

    uint16_t namespaceIndex() const { return nsIdx; }
    const std::string &name() const { return n; }

  private:
    uint16_t nsIdx;
    std::string n;
};

std::ostream &operator<<(std::ostream &os, const QualifiedName& qn);
 void convertToUAVariantImpl(
    const opc::types::QualifiedName &qn, UA_Variant *var);
QualifiedName fromUAQualifiedName(const UA_QualifiedName *qn);
UA_QualifiedName fromQualifiedName(const opc::types::QualifiedName &qn);

} // namespace types
} // namespace opc