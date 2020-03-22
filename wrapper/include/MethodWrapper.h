#pragma once
#include "Variant.h"
#include <functional>
#include <tuple>

template <typename Tuple, typename F, std::size_t... Indices>
void for_each_impl(Tuple &&tuple, F &&f, std::index_sequence<Indices...>)
{
    using swallow = int[];
    (void)swallow{1, (f(std::get<Indices>(std::forward<Tuple>(tuple))), void(),
                      int{})...};
}

template <typename Tuple, typename F>
void for_each(Tuple &&tuple, F &&f)
{
    constexpr std::size_t N =
        std::tuple_size<std::remove_reference_t<Tuple>>::value;
    for_each_impl(std::forward<Tuple>(tuple), std::forward<F>(f),
                  std::make_index_sequence<N>{});
}

namespace opc
{

class ICallable
{
  public:
    virtual bool call(void* obj, const std::vector<Variant> &inputArguments,
                      std::vector<Variant> &outputArguments) = 0;
    virtual ~ICallable() = default;
};

template <typename ClassType, typename R, typename... INARGS>
class Call : public ICallable
{
  public:
    
    Call(std::function<R(ClassType*)>f) : m_f(f) {}
    virtual bool call(void* obj, const std::vector<Variant> &inputArguments,
                      std::vector<Variant> &outputArguments) override
    {
      /*
        std::tuple<INARGS...> inputArgs;
        size_t i = 0;

        for_each(inputArgs, [&](auto &x) {
            x = inputArguments[i]
                    .get<typename std::remove_reference<decltype(x)>::type>();
            i++;
        });
        R result = std::apply(m_f, inputArgs);*/

        R result = m_f(static_cast<ClassType*>(obj));

        Variant var;
        var(result);
        outputArguments.push_back(std::move(var));
        return true;
    }

  private:
    std::function<R(ClassType*)> m_f{};
};

template <typename R, typename... INARGS>
class Call<void, R, INARGS...> : public ICallable
{
  public:
    Call(std::function<R(INARGS...)> f) : m_f(f) {}
    virtual bool call(void *obj, const std::vector<Variant> &inputArguments,
                      std::vector<Variant> &outputArguments) override
    {
        std::tuple<INARGS...> inputArgs;
        size_t i = 0;

        for_each(inputArgs, [&](auto &x) {
            x = inputArguments[i]
                    .get<typename std::remove_reference<decltype(x)>::type>();
            i++;
        });
        R result = std::apply(m_f, inputArgs);
        Variant var;
        var(result);
        outputArguments.push_back(std::move(var));
        return true;
    }

  private:
    std::function<R(INARGS...)> m_f{};
};

template <typename... INARGS>
class Call<void, void, INARGS...> : public ICallable
{
  public:
    Call(std::function<void(INARGS...)> f) : m_f(f) {}
    virtual bool call(void* obj, const std::vector<Variant> &inputArguments,
                      std::vector<Variant> &outputArguments) override
    {
        std::tuple<INARGS...> inputArgs;
        size_t i = 0;

        for_each(inputArgs, [&](auto &x) {
            x = inputArguments[i]
                    .get<typename std::remove_reference<decltype(x)>::type>();
            i++;
        });
        std::apply(m_f, inputArgs);
        return true;
    }

  private:
    std::function<void(INARGS...)> m_f{};
};

} // namespace opc