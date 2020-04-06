
// #pragma once
// #include <array>
// #include <cassert>
// #include <memory>
// #include <opc/types/Types.h>
// #include <open62541/server.h>
// //have to add StdTypes, otherwise convertToUAVariantImpl is not found
// #include <opc/types/StdTypes.h>
// #include <string>
// #include <vector>
// #include <opc/DataType.h>

// namespace opc
// {

// template <class T>
// struct is_vector : public std::false_type
// {
// };

// template <class T, class Alloc>
// struct is_vector<std::vector<T, Alloc>> : public std::true_type
// {
// };

// template <class T>
// struct HasconvertToUAVariantImpl_
// {
//     template <typename C>
//     static std::true_type
//     test(decltype(convertToUAVariantImpl(std::declval<C>(),
//                                          std::declval<UA_Variant *>())) *);
//     template <typename C>
//     static std::false_type test(...);
//     typedef decltype(test<T>(0)) type;
// };

// template <typename T>
// using HasconvertToUAVariantImpl = typename HasconvertToUAVariantImpl_<T>::type;

// /* ADL (argument dependent lookup) is used for binding the
// convertToUAVariantImpl . If convertToUAVariantImpl is found in the namespace of the argument type, 
// this template function will be instantiated.

// That means, the conversion function has to reside in the namespace of the concerning type. For types in the
// standard library, this conversion function has to reside in namespace std.
// */
// template <typename T>
// typename std::enable_if<!std::is_arithmetic_v<T>, void>::type
// convertToUAVariant(T &&t, UA_Variant *var)
// {
//     convertToUAVariantImpl(std::forward<T>(t), var);
// }

// template <typename T>
// typename std::enable_if<std::is_arithmetic_v<T>, void>::type
// convertToUAVariant(T &&val, UA_Variant *var)
// {
//     static_assert(std::is_arithmetic_v<std::remove_reference_t<T>>,
//                   "Type not integral");
//     UA_Variant_init(var);
//     UA_Variant_setScalarCopy(var, &val,
//                              getDataType<std::remove_reference_t<T>>());
//     var->storageType = UA_VariantStorageType::UA_VARIANT_DATA;
// }






// } // namespace opc




