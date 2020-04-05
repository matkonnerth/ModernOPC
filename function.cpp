#include <string>
#include <type_traits>
#include <iostream>
#include <vector>

//std::string fooImpl(int i) { return "int!"; }

template <class T>
struct HasConversionImpl_
{
    template <typename C>
    static std::true_type test(decltype(conversionImpl(std::declval<C>())) *);
    template <typename C>
    static std::false_type test(...);
    typedef decltype(test<T>(0)) type;
};

template <typename T>
using HasConversionImpl = typename HasConversionImpl_<T>::type;

template <typename T>
typename std::enable_if<HasConversionImpl<T>::value, void>::type convert(T &&t)
{
    conversionImpl(std::forward<T>(t));
}

template <typename T>
typename std::enable_if<!HasConversionImpl<T>::value, void>::type convert(T &&t)
{
  //static_assert(std::is_arithmetic<T>::value, "asdf");
    std::cout << "generic!\n";
}

struct X
{
};

template <class T>
struct S
{
};

template <class T>
void conversionImpl(S<T> const &)
{
    std::cout << "convert S<T> \n";
}

template <typename T>
void conversionImpl(std::vector<T>&)
{
  std::cout << "convert vector\n";
}



#include <iostream>
int main()
{
    convert(1);
    convert("nope");
    convert(S<unsigned>{});

    S<int> s2;
    convert(std::move(s2));

    std::vector<int> a{1, 2, 3};
    convert(a);
}