#include <benchmark/benchmark.h>
#include <modernOpc/DataSource.h>
#include <modernOpc/Server.h>
#include <modernOpc/Variant.h>
#include <modernOpc/nodes/ObjectNode.h>
#include <modernOpc/nodes/VariableNode.h>
#include <open62541/server.h>
#include <vector>

using modernopc::Server;
using modernopc::Variant;

static UA_StatusCode internalRead(UA_Server *server, const UA_NodeId *sessionId,
                                  void *sessionContext, const UA_NodeId *nodeId,
                                  void *nodeContext,
                                  UA_Boolean includeSourceTimeStamp,
                                  const UA_NumericRange *range,
                                  UA_DataValue *value)
{
    UA_Int32 i = 127;
    UA_Variant_setScalarCopy(&value->value, &i, &UA_TYPES[UA_TYPES_INT32]);
    value->hasValue = true;
    return UA_STATUSCODE_GOOD;
}

static UA_StatusCode
internalWrite(UA_Server *server, const UA_NodeId *sessionId,
              void *sessionContext, const UA_NodeId *nodeId, void *nodeContext,
              const UA_NumericRange *range, const UA_DataValue *value)
{
    return UA_STATUSCODE_GOOD;
}

static void readRaw(benchmark::State &state)
{
    modernopc::Server s;
    auto root = s.getObjectsFolder();
    auto var = root->addVariable(modernopc::NodeId(1, 123),
                                 modernopc::NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                 modernopc::QualifiedName(1, "demoInt"), 27);

    UA_Server *server = s.getUAServer();
    const UA_NodeId id = UA_NODEID_NUMERIC(1, 123);
    UA_DataSource source = {internalRead, internalWrite};
    UA_Server_setVariableNode_dataSource(server, id, source);
    UA_Variant v;

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(UA_Server_readValue(server, id, &v));
        UA_Variant_clear(&v);
    }
}

UA_StatusCode getValue(const modernopc::NodeId &id, modernopc::Variant &var)
{
    var(42);
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode setValue(const modernopc::NodeId &id, const modernopc::Variant &var)
{
    return UA_STATUSCODE_GOOD;
}

static void readWrapped(benchmark::State &state)
{
    modernopc::Server s;

    auto root = s.getObjectsFolder();
    auto var = root->addVariable(modernopc::NodeId(1, 123),
                                 modernopc::NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                 modernopc::QualifiedName(1, "demoInt"), 27);
    var->connectCallback(
        std::make_unique<modernopc::DataSource>("simpleVal", getValue, setValue));

    UA_Server *server = s.getUAServer();
    UA_Variant v;
    const UA_NodeId id = UA_NODEID_NUMERIC(1, 123);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(UA_Server_readValue(server, id, &v));
        UA_Variant_clear(&v);
    }
}

static void writeRaw(benchmark::State &state)
{
    modernopc::Server s;
    auto root = s.getObjectsFolder();
    auto var = root->addVariable(modernopc::NodeId(1, 123),
                                 modernopc::NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                 modernopc::QualifiedName(1, "demoInt"), 27);

    UA_Server *server = s.getUAServer();
    const UA_NodeId id = UA_NODEID_NUMERIC(1, 123);
    UA_DataSource source = {internalRead, internalWrite};
    UA_Server_setVariableNode_dataSource(server, id, source);
    UA_Variant v;
    UA_Int32 i = 1222;
    UA_Variant_setScalarCopy(&v, &i, &UA_TYPES[UA_TYPES_INT32]);

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(UA_Server_writeValue(server, id, v));
    }
    UA_Variant_clear(&v);
}

static void writeWrapped(benchmark::State &state)
{
    modernopc::Server s;
    auto root = s.getObjectsFolder();
    auto var = root->addVariable(modernopc::NodeId(1, 123),
                                 modernopc::NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                 modernopc::QualifiedName(1, "demoInt"), 27);

    var->connectCallback(
        std::make_unique<modernopc::DataSource>("simpleVal", getValue, setValue));

    UA_Server *server = s.getUAServer();
    const UA_NodeId id = UA_NODEID_NUMERIC(1, 123);
    UA_Variant v;
    UA_Int32 i = 1222;
    UA_Variant_setScalarCopy(&v, &i, &UA_TYPES[UA_TYPES_INT32]);

    for (auto _ : state)
    {
        benchmark::DoNotOptimize(UA_Server_writeValue(server, id, v));
    }
    UA_Variant_clear(&v);
    
}

BENCHMARK(readRaw);
BENCHMARK(readWrapped);
BENCHMARK(writeRaw);
BENCHMARK(writeWrapped);

BENCHMARK_MAIN();