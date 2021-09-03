#include "nilmu/nilmu.hpp"
#include "nilmu/theme.hpp"

#include <vector>
#include <array>
#include <numeric>
#include <chrono>
#include <thread>

int main()
{

    // Nilmu options struct : fps & terminal width
    nilmu::nil_options.frequency<30>()
                      .term_width(80)
                      .theme(new AsciiTheme());

    // Amount of time to sleep at each loop iteration
    const auto sleep_time = std::chrono::milliseconds(2);
    auto sleep_time_f = [] (int x) {return std::chrono::milliseconds(x);};

    // Iterator-based nilmu wrapper
    auto arr = std::array<int, 15>();
    auto n = nilmu::nilmu(arr, "Done.");
    for (auto itr = n.begin(), end = n.end(); itr != end; ++itr) {
        for (auto itr2 = n.begin(); itr2 != n.end(); ++itr2) {
            for (auto itr2 = n.begin(); itr2 != n.end(); ++itr2) {
                std::this_thread::sleep_for(sleep_time);
            }
        }
    }

    // Range-based loop nilmu wrapper
    for (auto x : nilmu::nilmu(std::vector<int>(100, 2), "Finished")) {
        std::this_thread::sleep_for(sleep_time_f(x));;
    }

    auto truc = std::vector<int>(100);
    auto nn = nilmu::nilmu(truc.begin(), truc.end(), "The end");
    for (auto x : nn)
        std::this_thread::sleep_for(sleep_time_f(x));;

    return 0;
}
