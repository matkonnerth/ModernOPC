#include <functional>
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <type_traits>
#include <variant>
#include <cassert>
#include <any>

template <typename Tuple,
          typename Indices = std::make_index_sequence<std::tuple_size<Tuple>::value>>
struct runtime_get_func_table;

template <typename Tuple, size_t... Indices>
struct runtime_get_func_table<Tuple, std::index_sequence<Indices...>> {
    using return_type = typename std::tuple_element<0, Tuple>::type &;
    using get_func_ptr = return_type (*)(Tuple &) noexcept;
    static constexpr get_func_ptr table[std::tuple_size<Tuple>::value] = {
        &std::get<Indices>...};
};

template <typename Tuple, size_t... Indices>
constexpr
    typename runtime_get_func_table<Tuple, std::index_sequence<Indices...>>::get_func_ptr
        runtime_get_func_table<
            Tuple, std::index_sequence<Indices...>>::table[std::tuple_size<Tuple>::value];

template <typename Tuple>
constexpr
    typename std::tuple_element<0, typename std::remove_reference<Tuple>::type>::type &
    runtime_get(Tuple &&t, size_t index) {
    using tuple_type = typename std::remove_reference<Tuple>::type;
    if(index >= std::tuple_size<tuple_type>::value)
        throw std::runtime_error("Out of range");
    return runtime_get_func_table<tuple_type>::table[index](t);
}

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

template <typename Tuple, typename F, std::size_t... Indices>
void
for_each_impl(Tuple &&tuple, F &&f, std::index_sequence<Indices...>) {
    using swallow = int[];
    (void)swallow{1,
                  (f(std::get<Indices>(std::forward<Tuple>(tuple))), void(), int{})...};
}

template <typename Tuple, typename F>
void
for_each(Tuple &&tuple, F &&f) {
    constexpr std::size_t N = std::tuple_size<std::remove_reference_t<Tuple>>::value;
    for_each_impl(std::forward<Tuple>(tuple), std::forward<F>(f),
                  std::make_index_sequence<N>{});
}

int main(int argc, char const* argv[]) {
   
    std::function<void(int)> f {f2};
    Functor myFunctor(f);

    IFunction* i = &myFunctor;

    std::variant<int, double> var {10};
    std::vector<std::variant<int,double>> args;
    args.push_back(var);

    i->call(args);

    std::tuple<int, int, double> tup {10, 12, 3.14};

    


    for_each(tup,
                [](auto& x) { std::cout << x << std::endl; });

    return 0;
}




