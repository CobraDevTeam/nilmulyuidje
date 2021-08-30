#pragma once

#include <chrono>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string>
#include <memory>

#include "theme.hpp"

namespace nilmu
{

struct NilmuOptions
{
    using DurationType = std::chrono::microseconds;

    short      depth = 0;
    short      max_depth = 0;
    uint32_t _term_width = 80;
    DurationType threshold = std::chrono::duration_cast<DurationType>(std::chrono::duration<long, std::ratio<1, 10>>(1));
    const std::string backline = "\033[A";
    std::unique_ptr<BaseTheme> _theme;

    NilmuOptions();

    template <std::intmax_t Hertz>
    NilmuOptions& frequency();

    NilmuOptions& term_width(uint32_t width);
    uint32_t term_width() const;

    NilmuOptions& theme(BaseTheme* theme);
    const BaseTheme& theme() const;


} nil_options;

NilmuOptions::NilmuOptions()
{
    term_width(80);
    theme(new RollTheme);
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

NilmuOptions& NilmuOptions::theme(BaseTheme* theme)
{
    _theme.reset(theme);
    return *this;
}

const BaseTheme& NilmuOptions::theme() const
{
   return *_theme;
}

}
// namespace nilmu
