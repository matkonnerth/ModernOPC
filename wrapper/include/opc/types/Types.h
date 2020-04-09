#pragma once
#include <string>
#include <iostream>
#include <open62541/types.h>

namespace opc
{

class LocalizedText
{
  public:
    LocalizedText(const std::string &locale, const std::string &text)
        : loc{locale}, txt{text}
    {
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
void convertToUAVariantImpl(const LocalizedText &m,
                                UA_Variant *var);
LocalizedText fromUALocalizedText(const UA_LocalizedText *lt);
UA_LocalizedText fromLocalizedText(const LocalizedText &lt);

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
    const QualifiedName &qn, UA_Variant *var);
QualifiedName fromUAQualifiedName(const UA_QualifiedName *qn);
const UA_QualifiedName fromQualifiedName(const QualifiedName &qn);

} // namespace opc