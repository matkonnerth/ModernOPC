#pragma once

typedef struct UA_Variant
{
    void *data;                 /* Points to the scalar or array data */
} UA_Variant;

namespace opc
{


template <class T>
struct HasconvertToUAVariantImpl_
{
    template <typename C>
    static std::true_type
    test(decltype(convertToUAVariantImpl(std::declval<C>(),
                                         std::declval<UA_Variant *>())) *);
    template <typename C>
    static std::false_type test(...);
    typedef decltype(test<T>(0)) type;
};

template <typename T>
using HasconvertToUAVariantImpl = typename HasconvertToUAVariantImpl_<T>::type;

template <typename T>
typename std::enable_if<HasconvertToUAVariantImpl<T>::value, void>::type
convertToUAVariant(T &&t, UA_Variant *var)
{
    convertToUAVariantImpl(std::forward<T>(t), var);
}

template <typename T>
typename std::enable_if<!HasconvertToUAVariantImpl<T>::value, void>::type
convertToUAVariant(T &&t, UA_Variant *var)
{
    static_assert(std::is_arithmetic<T>::value, "asdf");
    std::cout << "generic!\n";
}

} // namespace opc