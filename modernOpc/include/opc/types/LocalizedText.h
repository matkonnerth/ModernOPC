#pragma once
#include <open62541/types.h>
#include <string>
#include <opc/DataType.h>

namespace opc
{

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

    friend std::ostream &operator<<(std::ostream &os, const LocalizedText &qn);
    friend void toUAVariantImpl(const LocalizedText &m, UA_Variant *var);
    friend UA_LocalizedText fromLocalizedText(const LocalizedText &lt);
};

LocalizedText fromUALocalizedText(const UA_LocalizedText *lt);

template <>
inline const UA_DataType *getDataType<LocalizedText>()
{
    return &UA_TYPES[UA_TYPES_LOCALIZEDTEXT];
}

} // namespace opc