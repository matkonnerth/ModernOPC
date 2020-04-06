#include <benchmark/benchmark.h>
#include <iostream>
#include <open62541/client_config_default.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <opc/Server.h>
#include <opc/types/NodeId.h>

static UA_StatusCode helloWorldMethodCallback(
    UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle,
    const UA_NodeId *methodId, void *methodContext, const UA_NodeId *objectId,
    void *objectContext, size_t inputSize, const UA_Variant *input,
    size_t outputSize, UA_Variant *output)
{
    UA_Int32 *in = (UA_Int32 *)input->data;
    *in=*in+1;
    UA_Variant_setScalarCopy(output, in, &UA_TYPES[UA_TYPES_INT32]);
    //UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Method called");
    return UA_STATUSCODE_GOOD;
}

static void addHellWorldMethod(UA_Server *server)
{
    UA_Argument inputArgument;
    UA_Argument_init(&inputArgument);
    inputArgument.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    inputArgument.valueRank = UA_VALUERANK_SCALAR;

    UA_Argument outputArgument;
    UA_Argument_init(&outputArgument);
    outputArgument.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    outputArgument.valueRank = UA_VALUERANK_SCALAR;

    UA_MethodAttributes helloAttr = UA_MethodAttributes_default;
    helloAttr.executable = true;
    helloAttr.userExecutable = true;
    UA_Server_addMethodNode(server, UA_NODEID_NUMERIC(1, 62541),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_HASORDEREDCOMPONENT),
                            UA_QUALIFIEDNAME(1, (char *)"hello world"),
                            helloAttr, &helloWorldMethodCallback, 1,
                            &inputArgument, 1, &outputArgument, NULL, NULL);
}

static void serverRaw(benchmark::State &state)
{
    UA_Server *server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));

    addHellWorldMethod(server);

    UA_StatusCode retval = UA_Server_run_startup(server);    

    UA_Variant var;
    UA_Variant_init(&var);
    UA_Int32 i = 1230;
    UA_Variant_setScalar(&var, &i, &UA_TYPES[UA_TYPES_INT32]);

    UA_CallMethodRequest req;
    UA_CallMethodRequest_init(&req);
    req.objectId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    req.methodId = UA_NODEID_NUMERIC(1, 62541);
    req.inputArguments = &var;
    req.inputArgumentsSize = 1;

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(UA_Server_call(server, &req));
    }

    UA_Server_delete(server);
}

static int32_t callback(int32_t val)
{
    //std::cout << val << std::endl;
    return val+1;
}

static void serverWrapped(benchmark::State &state)
{
    using opc::types::NodeId;

    opc::Server s;
    s.addMethod(NodeId(0, 85), NodeId(1, 62541), "test", &callback);

    auto server = s.getUAServer();
    UA_StatusCode retval = UA_Server_run_startup(server);

    UA_Variant var;
    UA_Variant_init(&var);
    UA_Int32 i = 1230;
    UA_Variant_setScalar(&var, &i, &UA_TYPES[UA_TYPES_INT32]);

    UA_CallMethodRequest req;
    UA_CallMethodRequest_init(&req);
    req.objectId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    req.methodId = UA_NODEID_NUMERIC(1, 62541);
    req.inputArguments = &var;
    req.inputArgumentsSize = 1;

    UA_CallMethodResult res= UA_Server_call(server, &req);

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(UA_Server_call(server, &req));
    }
}

BENCHMARK(serverRaw);
BENCHMARK(serverWrapped);

BENCHMARK_MAIN();