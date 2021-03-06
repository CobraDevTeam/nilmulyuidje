#include "nilmu.hpp"

#include <vector>
#include <array>
#include <numeric>
#include <chrono>
#include <thread>

int main()
{
    auto vect = std::array<int, 1000>();
    auto n = nilmu::nilmu(vect, "Done.");

    nilmu::nil_options.frequency<10>()
                      .term_width(80);

    const auto x = 2;
    for (auto itr = n.begin(), end = n.end(); itr != end; ++itr) {
        std::this_thread::sleep_for(std::chrono::milliseconds(x));
        for (auto itr2 = n.begin(); itr2 != n.end(); ++itr2) {
            std::this_thread::sleep_for(std::chrono::milliseconds(x));
            for (auto itr2 = n.begin(); itr2 != n.end(); ++itr2) {
                std::this_thread::sleep_for(std::chrono::milliseconds(x));
            }
        }
    }
    for (auto itr2 = n.begin(); itr2 != n.end(); ++itr2) {
        std::this_thread::sleep_for(std::chrono::milliseconds(x));
    }

    return 0;
}
