#pragma once
#include <functional>
#include <modernOpc/Variant.h>
#include "TupleHelper.h"
#include <vector>

namespace modernopc
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
    bool call(void *obj, const std::vector<Variant> &inputArguments,
              std::vector<Variant> &outputArguments) final
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
        if constexpr (std::is_void_v<R>)
        {
            std::apply(m_f, t2);
        }
        else
        {
            R result = std::apply(m_f, t2);

            if constexpr (is_tuple<R>::value)
            {
                auto i = 0;
                for_each(result, [&](auto &x) {
                    Variant &var = outputArguments[i];
                    var(x);
                    i++;
                });
            }
            else
            {
                Variant &var = outputArguments[0];
                var(result);
            }
        }
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
    bool call(void *obj, const std::vector<Variant> &inputArguments,
              std::vector<Variant> &outputArguments) final
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
            
            if constexpr (is_tuple<R>::value)
            {
                auto i = 0;
                for_each(result, [&](auto &x) {
                    Variant &var = outputArguments[i];
                    var(x);
                    i++;
                });
            }
            else
            {
              Variant &var = outputArguments[0];
              var(result);
            }
        }
        return true;
    }

  private:
    std::function<R(INARGS...)> m_f{};
};

using FunctionCall =
    std::function<bool(const std::vector<Variant> &in, std::vector<Variant> &out)>;

template <>
class Call<FunctionCall> : public ICallable
{
  public:
    Call(FunctionCall f) : m_f(f) {}
    bool call(void *obj, const std::vector<Variant> &inputArguments,
              std::vector<Variant> &outputArguments) final
    {
        return m_f(inputArguments, outputArguments);
    }

  private:
    FunctionCall m_f{};
};
} // namespace modernopc