#pragma once
#include <string>
#include <open62541/types.h>
#include <opc/DataType.h>

namespace opc
{
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

    friend std::ostream &operator<<(std::ostream &os, const QualifiedName &qn);
    friend void convertToUAVariantImpl(const QualifiedName &qn,
                                       UA_Variant *var);
    friend const UA_QualifiedName fromQualifiedName(const QualifiedName &qn);
};

template <>
inline const UA_DataType *getDataType<QualifiedName>()
{
    return &UA_TYPES[UA_TYPES_QUALIFIEDNAME];
}

QualifiedName fromUAQualifiedName(const UA_QualifiedName *qn);
}
