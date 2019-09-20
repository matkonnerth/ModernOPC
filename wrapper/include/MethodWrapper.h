#include <functional>
#include <tuple>
#include "Variant.h"
#include "../tuple_getruntime.h"

namespace opc
{
  
class ICallable {
  public:
    virtual void
    call(const std::vector<Variant>&inputArguments, std::vector<Variant>& outputArguments) = 0;
};

template <typename... ARGS> class Functor : public ICallable {
  public:
    Functor(std::function<void(ARGS...)> f) : m_f(f) {}
    virtual void
    call(const std::vector<Variant> &inputArguments,
         std::vector<Variant> &outputArguments) override {
        std::tuple<ARGS...> calculatedArgs;

        for(size_t i = 0; i < n; i++) {
            //todo, get the correct type
            get<int>(calculatedArgs, i) = inputArguments[i].get<int>();
        }
        std::apply(m_f, calculatedArgs);
    }

  private:
    std::function<void(ARGS...)> m_f;
    std::tuple<ARGS...> args;
    const size_t n = sizeof...(ARGS);
};
}