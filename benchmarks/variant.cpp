#include <benchmark/benchmark.h>
#include <modernopc/Variant.h>
#include <open62541/server.h>
#include <vector>

static void IntVectorToVariant(benchmark::State &state)
{
    std::vector<int> vec{1000, 99};size_t cnt =0;
    for (auto _ : state)
    {
        modernopc::Variant var;
        var(vec);
    }
}

static void StringVectorToVariant(benchmark::State &state)
{
    std::vector<std::string> vec{1000, "blasfddfj"};
    size_t cnt = 0;
    for (auto _ : state)
    {
        modernopc::Variant var;
        var(vec);
    }
}

static void VariantToIntVector(benchmark::State &state)
{
    modernopc::Variant var;
    var(std::vector<int>(1000,99));
    for (auto _ : state)
    {
        auto v = var.get<std::vector<int>>();
        benchmark::DoNotOptimize(v);
    }
}

static void VariantToStringVector(benchmark::State &state)
{
    modernopc::Variant var;
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