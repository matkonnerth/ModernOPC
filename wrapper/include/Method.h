#pragma once
#include "TypeConverter.h"
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

    template <typename T>
    inline static int getTypeName()
    {
        std::cout << typeid(T).name() << std::endl;
        return 0;
    }

    inline static void getArgumentsName() { auto x = {getTypeName<Args>()...}; }

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

        UA_NodeId id = getUADataTypeId<T>();
        UA_Argument inputArgument;
        UA_Argument_init(&inputArgument);
        inputArgument.description =
            UA_LOCALIZEDTEXT((char *)"", (char *)"Method Argument");
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
        inputArgument.description =
            UA_LOCALIZEDTEXT((char *)"", (char *)"Method Argument");
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
struct MethodTraits : MethodTraits<decltype(&T::operator())>
{
};

template <typename R, typename... Args>
struct MethodTraits<R (*)(Args...)> : MethodTraitsBase<void, R, Args...>
{
};

template <typename R, typename... Args>
struct MethodTraits<std::function<R(Args...)>>
    : MethodTraitsBase<void, R, Args...>
{
};

template <typename R, typename ClassType, typename Arg1>
struct MethodTraits<R (ClassType::*)(Arg1)>
    : MethodTraitsBase<ClassType, R, Arg1>
{
    using ReturnType = R;
    using ThisPointerType = ClassType;
    using Argument1 = Arg1;
};

} // namespace opc