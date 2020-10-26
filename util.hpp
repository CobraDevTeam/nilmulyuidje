#pragma once

#include <cstdint>

namespace nilmu
{

short nb_digits(uint32_t x)
{
    return (x < 10 ? 1 :
           (x < 100 ? 2 :
           (x < 1000 ? 3 :
           (x < 10000 ? 4 :
           (x < 100000 ? 5 :
           (x < 1000000 ? 6 :
           (x < 10000000 ? 7 :
           (x < 100000000 ? 8 :
           (x < 1000000000 ? 9 :
            10)))))))));
}

// TODO Test on this function on edge cases
short nb_digits(uint64_t x)
{
    short digit_count = 0;

    uint64_t max_int(1);
    max_int <<= 32;
    while (x > max_int-1){
        x /= 10;
        digit_count++;
    }
    return digit_count + nb_digits(static_cast<uint32_t>(x));
}

}
// namespace nilmu
