#include <functional>
#include <iostream>
#include <memory>

template <typename T>
struct MethodTraits;

template <typename R, typename C, typename... Args>
struct MethodTraits<R (C::*)(Args...)>
{
    using type = std::function<R(C*, Args...)>;
};

class Service
{
  public:
    int run(int arg1, int arg2) { return arg1 + arg2; }
};

template <typename T>
class Call;

template <typename ClassType, typename R, typename... INARGS>
class Call<std::function<R(ClassType*, INARGS...)>>
{
  public:
    Call(std::function<R(ClassType *, INARGS...)> f) : m_f(f) {}

    std::function<R(ClassType *, INARGS...)> m_f;
};

template<typename M>
void getMemberFunctionInfo(const M& fn)
{
    typename MethodTraits<M>::type test{fn};
    std::make_unique<Call<decltype(test)>>(test);
}




int main() {
    //MethodTraits<decltype(&Service::run)>::type test{&Service::run};
    //new Call<decltype(test)>(test);

    using t1 = const std::string&;

    std::tuple<int, std::remove_reference_t<t1>> t;
    getMemberFunctionInfo(&Service::run);
 }