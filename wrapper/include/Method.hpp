#include <open62541/server.h>
#include <open62541/types.h>
#include <iostream>
#include <typeinfo>
#include <vector>
#include "TypeConverter.h"

template <typename ClassType, typename ReturnType, typename... Args>
struct MethodTraitsBase {
    inline static size_t
    getNumArgs() {
        return sizeof...(Args);
    }

    template <typename T>
    inline static int
    getTypeName() {
        std::cout << typeid(T).name() << std::endl;
        return 0;
    }

    inline static void
    getArgumentsName() {
        auto x = {getTypeName<Args>()...};
    }

    inline static std::vector<UA_Argument>
    getInputArguments() {
        int iArg = 0;
        const size_t nArgs = getNumArgs();
        if(nArgs <= 0)
            return std::vector<UA_Argument>();
        return {getInputArgument<Args>(iArg++)...};
    }

    template <typename T>
    inline static UA_Argument
    getInputArgument(const int &iArg = 0) {

        UA_NodeId id = TypeConverter::uaTypeNodeIdFromCpp<T>();
        UA_Argument inputArgument;
        UA_Argument_init(&inputArgument);
        // create n-th argument with name "Arg" + number
        inputArgument.description =
            UA_LOCALIZEDTEXT((char *)"", (char *)"Method Argument");
        inputArgument.name = UA_STRING_NULL;
        inputArgument.dataType = id;
        inputArgument.valueRank = UA_VALUERANK_SCALAR;
        // return
        return inputArgument;
    }
};

template <typename T> struct MethodTraits : MethodTraits<decltype(&T::operator())> {};

template <typename R, typename... Args>
struct MethodTraits<R (*)(Args...)> : MethodTraitsBase<void, R, Args...> {};