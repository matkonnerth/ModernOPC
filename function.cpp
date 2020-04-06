#include <string>
#include <type_traits>
#include <iostream>
#include <vector>
#include "conversion.h"







namespace std
{


void convertToUAVariantImpl(int i, UA_Variant *var)
{
    std::cout << "convert int! \n";
}

void convertToUAVariantImpl(std::string &&s, UA_Variant *var)
{
    std::cout << "convert string rvalue ref" << std::endl;
}

void convertToUAVariantImpl(std::string &s, UA_Variant *var)
{
    std::cout << "convert string ref" << std::endl;
}
} // namespace std

namespace opc
{


struct X
{
};

template <class T>
struct S
{
};

template <class T>
void convertToUAVariantImpl(S<T> const &, UA_Variant *)
{
    std::cout << "convert S<T> \n";
}
} // namespace opc

namespace std
{

template <typename T>
void convertToUAVariantImpl(const std::vector<T> &, UA_Variant *)
{
    std::cout << "convert const vector<T>&\n";
}

template <typename T>
void convertToUAVariantImpl(std::vector<T> &, UA_Variant *)
{
  std::cout << "convert vector<T>&\n";
}

template <typename T>
void convertToUAVariantImpl(std::vector<T> &&, UA_Variant *)
{
    std::cout << "convert vector<T>&&\n";
}

} // namespace std

#include <iostream>
int main()
{
    //static_assert(std::is_arithmetic<std::string>::value, "string is not arithmetic");
    UA_Variant ua;
    opc::convertToUAVariant(1, &ua);
    opc::S<int> s2;
    opc::convertToUAVariant(s2, &ua);
    std::vector<int> a{1, 2, 3};
    opc::convertToUAVariant(std::move(a), &ua);

    std::string st{"asdf"};
    opc::convertToUAVariant(st, &ua);
}