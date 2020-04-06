#include <opc/types/Types.h>
//#include <opc/Conversion.h>
#include <opc/Variant.h>
#include <opc/types/StdTypes.h>

namespace opc
{

template <>
const UA_DataType *getDataType<opc::types::LocalizedText>()
{
    return &UA_TYPES[UA_TYPES_LOCALIZEDTEXT];
}

template <>
types::LocalizedText toStdType(UA_Variant *variant)
{
    return types::fromUALocalizedText(
        static_cast<UA_LocalizedText *>(variant->data));
}

template <>
types::QualifiedName toStdType(UA_Variant *variant)
{
    return types::fromUAQualifiedName(
        static_cast<UA_QualifiedName *>(variant->data));
}

template <>
std::vector<types::LocalizedText> toStdType(UA_Variant *var)
{
    std::vector<types::LocalizedText> vec;
    for (size_t i = 0; i < var->arrayLength; i++)
    {
        vec.emplace_back(types::fromUALocalizedText(
            &static_cast<UA_LocalizedText *>(var->data)[i]));
    }
    return vec;
}

namespace types
{
void convertToUAVariantImpl(opc::types::LocalizedText m, UA_Variant *var)
{
    UA_LocalizedText lt = fromLocalizedText(m);
    UA_Variant_setScalarCopy(var, &lt,
                             getDataType<opc::types::LocalizedText>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

void convertToUAVariantImpl(const opc::types::LocalizedText &m, UA_Variant *var)
{
    UA_LocalizedText lt = fromLocalizedText(m);
    UA_Variant_setScalarCopy(var, &lt,
                             opc::getDataType<opc::types::LocalizedText>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
}

UA_QualifiedName fromQualifiedName(const opc::types::QualifiedName &qn)
{
    return UA_QUALIFIEDNAME_ALLOC(qn.namespaceIndex(),
                                  (char *)qn.name().c_str());
}

UA_LocalizedText fromLocalizedText(const opc::types::LocalizedText &lt)
{
    return UA_LOCALIZEDTEXT_ALLOC((char *)lt.locale().c_str(),
                                  (char *)lt.text().c_str());
}


LocalizedText fromUALocalizedText(const UA_LocalizedText *lt)
{
    std::string locale{reinterpret_cast<char *>(lt->locale.data),
                       lt->locale.length};
    std::string text{reinterpret_cast<char *>(lt->text.data), lt->text.length};
    return types::LocalizedText(locale, text);
}

QualifiedName fromUAQualifiedName(const UA_QualifiedName *qn)
{
    return QualifiedName(qn->namespaceIndex, opc::types::fromUAString(&qn->name));
}


} // namespace types

} // namespace opc