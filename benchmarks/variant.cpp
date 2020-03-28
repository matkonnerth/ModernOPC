#include <benchmark/benchmark.h>
#include <vector>
#include <Variant.h>
#include <iostream>
#include <open62541/server.h>

static void IntVectorToVariant(benchmark::State &state)
{
    std::vector<int> vec{1000, 99};size_t cnt =0;
    for (auto _ : state)
    {
        opc::Variant var;
        var(vec);
    }
}

static void StringVectorToVariant(benchmark::State &state)
{
    std::vector<std::string> vec{1000, "blasfddfj"};
    size_t cnt = 0;
    for (auto _ : state)
    {
        opc::Variant var;
        var(vec);
    }
}

static void VariantToIntVector(benchmark::State &state)
{
    //UA_Variant uavar;
    //std::vector<int> vec{1000, 99};
    //UA_Variant_setArrayCopy(&uavar, vec.data(), vec.size(), &UA_TYPES[UA_TYPES_INT32]);
    //opc::Variant var(&uavar, true);
    opc::Variant var;
    var(std::vector<int>(1000,99));
    for (auto _ : state)
    {
        auto v = var.get<std::vector<int>>();
        benchmark::DoNotOptimize(v);
    }
}

static void VariantToStringVector(benchmark::State &state)
{
    // UA_Variant uavar;
    // std::vector<int> vec{1000, 99};
    // UA_Variant_setArrayCopy(&uavar, vec.data(), vec.size(),
    // &UA_TYPES[UA_TYPES_INT32]); opc::Variant var(&uavar, true);
    opc::Variant var;
    var(std::vector<std::string>(1000, "adsfasdf"));
    for (auto _ : state)
    {
        auto v = var.get<std::vector<std::string>>();
        benchmark::DoNotOptimize(v);
    }
}

BENCHMARK(IntVectorToVariant);
BENCHMARK(StringVectorToVariant);
BENCHMARK(VariantToIntVector);
BENCHMARK(VariantToStringVector);

BENCHMARK_MAIN();