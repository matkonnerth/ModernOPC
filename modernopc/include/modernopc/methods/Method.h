#pragma once
#include <functional>
#include <modernopc/DataType.h>
#include <modernopc/util.h>
#include <open62541/types.h>
#include <type_traits>
#include <typeinfo>
#include <vector>
#include "TupleHelper.h"

namespace modernopc
{

template <typename ClassType, typename ReturnType, typename... Args>
struct MethodTraitsBase
{

    inline static size_t getInputArgsSize() { return sizeof...(Args); }

    inline static std::vector<UA_Argument> getInputArguments()
    {
        const size_t nArgs = getInputArgsSize();
        if (nArgs <= 0)
            return std::vector<UA_Argument>();
        return {getInputArgument<Args>()...};
    }

    template <typename T>
    inline static UA_Argument getInputArgument()
    {
        UA_Argument inputArgument;
        UA_Argument_init(&inputArgument);
        inputArgument.name = UA_STRING_NULL;
        inputArgument.dataType =
            getDataType<std::remove_const_t<std::remove_reference_t<T>>>()
                ->typeId;
        inputArgument.valueRank = UA_VALUERANK_SCALAR;
        return inputArgument;
    }

    inline static std::vector<UA_Argument> getOutputArguments()
    {
        if constexpr(std::is_void_v<ReturnType>)
        {
            return {};
        }
        else if constexpr (is_tuple<ReturnType>::value)
        {
            ReturnType outputArgs;
            std::vector<UA_Argument> args;
            size_t i = 0;

            for_each(outputArgs, [&](auto &x) {
                args.push_back(getOutputArgument<typename std::remove_reference<decltype(x)>::type>());
                i++;
            });
            return args;
        }
        else
        {
            return {getOutputArgument<ReturnType>()};
        }
    }

    template <typename T>
    inline static UA_Argument getOutputArgument()
    {
        UA_Argument inputArgument;
        UA_Argument_init(&inputArgument);
        inputArgument.name = UA_STRING_NULL;
        inputArgument.dataType =
            getDataType<std::remove_const_t<std::remove_reference_t<T>>>()
                ->typeId;
        if constexpr (is_vector<T>::value)
        {
            inputArgument.valueRank = UA_VALUERANK_ONE_DIMENSION;

        }
        else
        {
            inputArgument.valueRank = UA_VALUERANK_SCALAR;
        }
        return inputArgument;
    }
};

template <typename T>
struct MethodTraits
{

};

template <typename R, typename... Args>
struct MethodTraits<R (*)(Args...)> : MethodTraitsBase<void, R, Args...>
{
    using type = std::function<R(Args...)>;
};

template <typename R, typename ClassType, typename...Args>
struct MethodTraits<R (ClassType::*)(Args...)>
    : MethodTraitsBase<ClassType, R, Args...>
{
    using type = std::function<R(ClassType *, Args...)>;
};

template <typename R, typename... Args>
struct MethodTraits<std::function<R(Args...)>>
    : MethodTraitsBase<void, R, Args...>
{
    using type = std::function<R(Args...)>;
};

} // namespace modernopc