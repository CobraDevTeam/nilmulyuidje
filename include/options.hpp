#pragma once

#include <chrono>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string>

namespace nilmu
{

struct NilmuOptions
{
    using DurationType = std::chrono::microseconds;

    short      depth = 0;
    short      max_depth = 0;
    uint32_t _term_width = 80;
    DurationType threshold = std::chrono::duration_cast<DurationType>(std::chrono::duration<long, std::ratio<1, 10>>(1));
    const char  spacer = ' ';
    const char  arrow_head = '>';
    const char  arrow_shaft = '=';
    const char  bracket_open = '[';
    const char  bracket_close = ']';
    const std::string backline = "\033[A";

    NilmuOptions();

    template <std::intmax_t Hertz>
    NilmuOptions& frequency();

    NilmuOptions& term_width(uint32_t width);
    uint32_t term_width() const;

} nil_options;

NilmuOptions::NilmuOptions()
{
    term_width(80);
}

template <std::intmax_t Hertz>
NilmuOptions& NilmuOptions::frequency()
{
    auto frequency = std::chrono::duration<long, std::ratio<1, Hertz>>(1);
    threshold = std::chrono::duration_cast<DurationType>(frequency);
    return *this;
}

NilmuOptions& NilmuOptions::term_width(uint32_t width){
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    _term_width = std::min<size_t>(width, w.ws_col);
    return *this;
}

uint32_t NilmuOptions::term_width() const
{
    return _term_width;
}

}
// namespace nilmu
