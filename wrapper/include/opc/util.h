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
} // namespace opc