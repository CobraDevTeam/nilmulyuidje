add_executable(Benchmark benchmark.cpp)

target_link_libraries(Benchmark
                      PUBLIC
                      benchmark
                      benchmark_main
                      pthread
                      )

target_include_directories(Benchmark
                           PUBLIC
                           "../include"
                           )
