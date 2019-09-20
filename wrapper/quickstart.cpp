#include <functional>
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <type_traits>
#include <variant>
#include <cassert>
#include <any>
#include "tuple_getruntime.h"



void f2(int len)
{
    std::cout << len << std::endl;
}

class IFunction{
    public:
        virtual void call(std::vector<std::variant<int, double>>& arguments) = 0;
};

template <typename... ARGS>
class Functor : public IFunction
{
    public:
        Functor(std::function<void(ARGS...)> f): m_f(f) {}
        virtual void
        call(std::vector<std::variant<int, double>>& arguments) override {
            std::tuple<ARGS...> calculatedArgs;

            for(size_t i=0; i<n; i++)
            {
                
            }
            using type = typename std::tuple_element< 0, std::tuple<ARGS...>>::type;
            std::get<0>(calculatedArgs) = std::get<type>(arguments.at(0)) +19;

            std::apply(m_f, calculatedArgs);
        }

      private:
        std::function<void(ARGS...)> m_f;
        std::tuple<ARGS...> args;
        const size_t n = sizeof...(ARGS);
        
        
};

int main(int argc, char const* argv[]) {
   
    std::function<void(int)> f {f2};
    Functor myFunctor(f);

    IFunction* i = &myFunctor;

    std::variant<int, double> var {10};
    std::vector<std::variant<int,double>> args;
    args.push_back(var);

    i->call(args);

    std::tuple<int, int, double> tup {10, 12, 3.14};


    auto elem2 = get<int>(tup,1);
    
    int idx = 1;
    auto elem = get<int>(tup, idx);

    std::cout << elem << std::endl;
    return 0;
}




