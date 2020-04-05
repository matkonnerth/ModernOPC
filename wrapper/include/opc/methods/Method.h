#pragma once
#include <opc/Conversion.h>
#include <functional>
#include <iostream>
#include <open62541/server.h>
#include <open62541/types.h>
#include <type_traits>
#include <typeinfo>
#include <vector>

namespace opc
{

template <typename ClassType, typename ReturnType, typename... Args>
struct MethodTraitsBase
{

    inline static size_t getNumArgs() { return sizeof...(Args); }

    inline static std::vector<UA_Argument> getInputArguments()
    {
        int iArg = 0;
        const size_t nArgs = getNumArgs();
        if (nArgs <= 0)
            return std::vector<UA_Argument>();
        return {getInputArgument<Args>(iArg++)...};
    }

    template <typename T>
    inline static UA_Argument getInputArgument(const int &iArg = 0)
    {

        UA_NodeId id = getUADataTypeId<std::remove_const_t<std::remove_reference_t<T>>>();
        UA_Argument inputArgument;
        UA_Argument_init(&inputArgument);
        inputArgument.name = UA_STRING_NULL;
        inputArgument.dataType = id;
        inputArgument.valueRank = UA_VALUERANK_SCALAR;
        return inputArgument;
    }

    inline static std::vector<UA_Argument> getOutputArguments()
    {

        return {getOutputArgument<ReturnType>()};
    }

    template <typename T>
    inline static UA_Argument getOutputArgument()
    {

        UA_NodeId id = getUADataTypeId<T>();
        UA_Argument inputArgument;
        UA_Argument_init(&inputArgument);
        inputArgument.name = UA_STRING_NULL;
        inputArgument.dataType = id;
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

} // namespace opc