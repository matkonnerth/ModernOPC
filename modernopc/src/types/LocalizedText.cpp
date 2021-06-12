#include <modernopc/Variant.h>
#include <modernopc/types/LocalizedText.h>
#include <modernopc/types/String.h>
#include <ostream>

namespace modernopc
{

template <>
LocalizedText Variant::get<LocalizedText>() const
{
    return fromUALocalizedText(static_cast<UA_LocalizedText *>(variant->data));
}

template <>
std::vector<LocalizedText> Variant::get<std::vector<LocalizedText>>() const
{
    std::vector<LocalizedText> vec;
    for (size_t i = 0; i < variant->arrayLength; i++)
    {
        vec.emplace_back(fromUALocalizedText(
            &static_cast<UA_LocalizedText *>(variant->data)[i]));
    }
    return vec;
}

void toUAVariantImpl(const LocalizedText &m, UA_Variant *var)
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

} // namespace modernopc