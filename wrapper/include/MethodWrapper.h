#include <functional>
#include <tuple>
#include <variant>

namespace opc
{
class ICallable {
  public:
    virtual void
    call(std::vector<std::variant<int, double>> &arguments) = 0;
};

template <typename... ARGS> class Functor : public ICallable {
  public:
    Functor(std::function<void(ARGS...)> f) : m_f(f) {}
    virtual void
    call(std::vector<std::variant<int, double>> &arguments) override {
        std::tuple<ARGS...> calculatedArgs;

        for(size_t i = 0; i < n; i++) {
        }
        using type = typename std::tuple_element<0, std::tuple<ARGS...>>::type;
        std::get<0>(calculatedArgs) = std::get<type>(arguments.at(0));

        std::apply(m_f, calculatedArgs);
    }

  private:
    std::function<void(ARGS...)> m_f;
    std::tuple<ARGS...> args;
    const size_t n = sizeof...(ARGS);
};
}  // namespace opc