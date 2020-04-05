#pragma once
#include <opc/Variant.h>
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
    virtual bool call(void *obj, const std::vector<Variant> &inputArguments,
                      std::vector<Variant> &outputArguments) = 0;
    virtual ~ICallable() = default;
};

template <typename T>
class Call;

template <typename ClassType, typename R, typename... INARGS>
class Call<std::function<R(ClassType *, INARGS...)>> : public ICallable
{
  public:
    Call(std::function<R(ClassType *, INARGS...)> f) : m_f(f) {}
    virtual bool call(void *obj, const std::vector<Variant> &inputArguments,
                      std::vector<Variant> &outputArguments) override
    {

        std::tuple<std::remove_const_t<std::remove_reference_t<INARGS>>...>
            inputArgs;
        size_t i = 0;

        for_each(inputArgs, [&](auto &x) {
            x = inputArguments[i]
                    .get<typename std::remove_reference<decltype(x)>::type>();
            i++;
        });

        auto t1 = std::make_tuple(static_cast<ClassType *>(obj));
        auto t2 = std::tuple_cat(t1, inputArgs);
        R result = std::apply(m_f, t2);

        Variant var;
        var(result);
        outputArguments.push_back(std::move(var));
        return true;
    }

  private:
    std::function<R(ClassType *, INARGS...)> m_f{};
};

template <typename R, typename... INARGS>
class Call<std::function<R(INARGS...)>> : public ICallable
{
  public:
    Call(std::function<R(INARGS...)> f) : m_f(f) {}
    virtual bool call(void *obj, const std::vector<Variant> &inputArguments,
                      std::vector<Variant> &outputArguments) override
    {
        std::tuple<std::remove_const_t<std::remove_reference_t<INARGS>>...>
            inputArgs;
        size_t i = 0;

        for_each(inputArgs, [&](auto &x) {
            x = inputArguments[i]
                    .get<typename std::remove_reference<decltype(x)>::type>();
            i++;
        });
        if constexpr (std::is_void_v<R>)
        {
            std::apply(m_f, inputArgs);
        }
        else
        {
            R result = std::apply(m_f, inputArgs);
            Variant var;
            var(result);
            outputArguments.push_back(std::move(var));
        }
        return true;
    }

  private:
    std::function<R(INARGS...)> m_f{};
};
} // namespace opc