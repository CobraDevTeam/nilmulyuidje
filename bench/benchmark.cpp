#include "nilmu/nilmu.hpp"

#include <vector>
#include <array>
#include <chrono>
#include <benchmark/benchmark.h>
#include <cstdio>
#include <iostream>


static void BM_nilmu(benchmark::State& state) {
    int n = 1000;
    auto vec = std::vector<int>(n);
    auto nil = nilmu::nilmu(vec, "");
    nilmu::nil_options.frequency<10>()
                      .term_width(80);
    for (auto _ : state) {
        for (auto itr = nil.begin(), end = nil.end(); itr != end; ++itr){}
    }
}

static void BM_classic(benchmark::State& state) {
    int n = 1000;
    auto vec = std::vector<int>(n);
    for (auto _ : state) {
        for (auto itr(vec.begin()), end = vec.end(); itr != end; itr++){}
    }
}

BENCHMARK(BM_nilmu);
BENCHMARK(BM_classic);

BENCHMARK_MAIN();
