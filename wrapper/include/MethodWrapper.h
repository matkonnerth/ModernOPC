#include <functional>
#include <tuple>
#include "Variant.h"

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

namespace opc
{

class ICallable {
  public:
    virtual bool
    call(const std::vector<Variant>&inputArguments, std::vector<Variant>& outputArguments) = 0;
};

template <typename... ARGS> class CallWithOutOutputArgs : public ICallable {
  public:
    CallWithOutOutputArgs(std::function<void(ARGS...)> f) : m_f(f) {}
    virtual bool
    call(const std::vector<Variant> &inputArguments,
         std::vector<Variant> &outputArguments) override {
        std::tuple<ARGS...> calculatedArgs;
        size_t i=0;

      for_each(calculatedArgs, [&](auto &x) { x=inputArguments[i].get<typename std::remove_reference<decltype(x)>::type>();i++; });
      std::apply(m_f, calculatedArgs);
      return true;
    }

  private:
    std::function<void(ARGS...)> m_f;
    std::tuple<ARGS...> args;
    const size_t n = sizeof...(ARGS);
};
}