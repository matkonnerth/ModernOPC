#include <benchmark/benchmark.h>
#include <opc/Variant.h>
#include <open62541/server.h>
#include <vector>
#include <opc/Server.h>
#include <opc/nodes/VariableNode.h>
#include <opc/nodes/ObjectNode.h>

using opc::Server;
using opc::Variant;
static void readRaw(benchmark::State &state)
{
    Server s;

    auto var = s.getObjectsFolder()->addBaseDataTypeVariable(opc::NodeId(1, 123),
                                  opc::QualifiedName(1, "demoInt"), 27);


    UA_Server* server = s.getUAServer();
    UA_Variant v;
    const UA_NodeId id = UA_NODEID_NUMERIC(1, 123);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(UA_Server_readValue(server, id, &v));
        UA_Variant_clear(&v);
    }
}

static void readWrapped(benchmark::State &state)
{
    Server s;

    auto var = s.getObjectsFolder()->addBaseDataTypeVariable(
        opc::NodeId(1, 123), opc::QualifiedName(1, "demoInt"), 27);

    Variant v;
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(var->read(v));
    }
}

static void writeRaw(benchmark::State &state)
{
    Server s;

    auto var = s.getObjectsFolder()->addBaseDataTypeVariable(
        opc::NodeId(1, 123), opc::QualifiedName(1, "demoInt"), 27);

    UA_Server *server = s.getUAServer();
    UA_Variant v;
    UA_Int32 i = 1222;
    UA_Variant_setScalar(&v, &i, &UA_TYPES[UA_TYPES_INT32]);
    const UA_NodeId id = UA_NODEID_NUMERIC(1, 123);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(UA_Server_writeValue(server, id, v));
    }
}

static void writeWrapped(benchmark::State &state)
{
    Server s;

    auto var = s.getObjectsFolder()->addBaseDataTypeVariable(
        opc::NodeId(1, 123), opc::QualifiedName(1, "demoInt"), 27);

    Variant v{(int32_t)1222};
    const UA_NodeId id = UA_NODEID_NUMERIC(1, 123);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(var->write(v));
    }
}

BENCHMARK(readRaw);
BENCHMARK(readWrapped);
BENCHMARK(writeRaw);
BENCHMARK(writeWrapped);

BENCHMARK_MAIN();