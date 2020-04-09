#include <opc/Variant.h>
#include <opc/types/LocalizedText.h>
#include <opc/types/StdTypes.h>
#include <ostream>

namespace opc
{

template <>
LocalizedText toStdType(UA_Variant *variant)
{
    return fromUALocalizedText(static_cast<UA_LocalizedText *>(variant->data));
}

template <>
std::vector<LocalizedText> toStdType(UA_Variant *var)
{
    std::vector<LocalizedText> vec;
    for (size_t i = 0; i < var->arrayLength; i++)
    {
        vec.emplace_back(fromUALocalizedText(
            &static_cast<UA_LocalizedText *>(var->data)[i]));
    }
    return vec;
}

void convertToUAVariantImpl(const LocalizedText &m, UA_Variant *var)
{
    UA_LocalizedText lt = fromLocalizedText(m);
    UA_Variant_setScalarCopy(var, &lt, getDataType<LocalizedText>());
    var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
    UA_LocalizedText_clear(&lt);
}

UA_LocalizedText fromLocalizedText(const LocalizedText &lt)
{
    return UA_LOCALIZEDTEXT_ALLOC(lt.locale().c_str(), lt.text().c_str());
}

LocalizedText fromUALocalizedText(const UA_LocalizedText *lt)
{
    std::string locale{reinterpret_cast<char *>(lt->locale.data),
                       lt->locale.length};
    std::string text{reinterpret_cast<char *>(lt->text.data), lt->text.length};
    return LocalizedText(locale, text);
}

std::ostream &operator<<(std::ostream &os, const LocalizedText &lt)
{
    os << "locale=" << lt.locale() << ";text=" << lt.text();
    return os;
}

} // namespace opc