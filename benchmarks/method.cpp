#include <benchmark/benchmark.h>
#include <modernopc/Server.h>
#include <modernopc/nodes/MethodNode.h>
#include <modernopc/nodes/ObjectNode.h>
#include <modernopc/types/NodeId.h>
#include <open62541/client_config_default.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>

static UA_StatusCode helloWorldMethodCallback(
    UA_Server *server, const UA_NodeId *sessionId, void *sessionHandle,
    const UA_NodeId *methodId, void *methodContext, const UA_NodeId *objectId,
    void *objectContext, size_t inputSize, const UA_Variant *input,
    size_t outputSize, UA_Variant *output)
{
    auto *in = static_cast<UA_Int32*>(input->data);
    *in=*in+1;
    UA_Variant_setScalar(output, in, &UA_TYPES[UA_TYPES_INT32]);
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
                            &inputArgument, 1, &outputArgument, nullptr, nullptr);
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
    return val+1;
}

static void serverWrapped(benchmark::State &state)
{
    using modernopc::NodeId;

    modernopc::Server s;
    s.getObjectsFolder()->addMethod(NodeId(1, 62541), modernopc::QualifiedName(1,"test"),
                                    &callback);

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

int32_t manyArgs(int a0, int a1, int a2, int a3, int a4, int a5, int a6,
                  int a7, int a8, int a9, int a10, int a11, int a12, int a13,
                  int a14, int a15, int a16, int a17, int a18, int a19, int a20,
                  int a21, int a22, int a23, int a24, int a25, int a26, int a27,
                  int a28, int a29, int a30, int a31, int a32, int a33, int a34,
                  int a35, int a36, int a37, int a38, int a39, int a40, int a41,
                  int a42, int a43, int a44, int a45, int a46, int a47, int a48,
                  int a49, int a50, int a51, int a52, int a53, int a54, int a55,
                  int a56, int a57, int a58, int a59, int a60, int a61, int a62,
                  int a63, int a64, int a65, int a66, int a67, int a68, int a69,
                  int a70, int a71, int a72, int a73, int a74, int a75, int a76,
                  int a77, int a78, int a79, int a80, int a81, int a82, int a83,
                  int a84, int a85, int a86, int a87, int a88, int a89, int a90,
                  int a91, int a92, int a93, int a94, int a95, int a96, int a97,
                  int a98, int a99)
{
    return a99;
}

static void serverManyArgs(benchmark::State &state)
{
    using modernopc::NodeId;

    modernopc::Server s;
    s.getObjectsFolder()->addMethod(NodeId(1, 62541), modernopc::QualifiedName(1, "test"), &manyArgs);

    auto server = s.getUAServer();
    UA_StatusCode retval = UA_Server_run_startup(server);

    const auto cnt = 100;
    UA_Variant var[cnt];
    for (auto i = 0; i < cnt; i++)
    {
        UA_Variant_setScalarCopy(&var[i], &i, &UA_TYPES[UA_TYPES_INT32]);
    }

    UA_CallMethodRequest req;
    UA_CallMethodRequest_init(&req);
    req.objectId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    req.methodId = UA_NODEID_NUMERIC(1, 62541);
    req.inputArguments = var;
    req.inputArgumentsSize = cnt;

    UA_CallMethodResult res = UA_Server_call(server, &req);

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(UA_Server_call(server, &req));
    }
}

BENCHMARK(serverRaw);
BENCHMARK(serverWrapped);
BENCHMARK(serverManyArgs);

BENCHMARK_MAIN();