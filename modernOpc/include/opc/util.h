#pragma once
#include <type_traits>
#include <vector>
namespace opc
{

template <class T>
struct is_vector : public std::false_type
{
};

template <class T, class Alloc>
struct is_vector<std::vector<T, Alloc>> : public std::true_type
{
};

template <typename T>
struct removeConstRef
{
    using type = std::remove_const_t<std::remove_reference_t<T>>;
};

template <class T>
using removeConstRef_t = typename removeConstRef<T>::type;
} // namespace opc