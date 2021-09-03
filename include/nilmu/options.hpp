#pragma once

#include <chrono>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string>
#include <memory>

#include "theme.hpp"

namespace nilmu
{

/**
 *
 */
struct NilmuOptions
{
    // Convenient alias for the base duration we use
    using DurationType = std::chrono::microseconds;

    //
    short      depth = 0;
    //
    short      max_depth = 0;
    // Terminal width
    uint32_t _term_width = 80;
    //
    DurationType threshold = std::chrono::duration_cast<DurationType>(std::chrono::duration<long, std::ratio<1, 10>>(1));
    //
    const std::string backline = "\033[A";
    std::unique_ptr<BaseTheme> _theme;

    /**
     * Default constructor. Sets terminal width to 80 and theme to RollTheme
     */
    NilmuOptions();

    /**
     *
     */
    template <std::intmax_t Hertz>
    NilmuOptions& frequency();

    /**
     * Sets the new terminal width
     */
    NilmuOptions& term_width(uint32_t width);

    /**
     * Returns the current terminal width used by nilmu
     */
    uint32_t term_width() const;

    /**
     * Changes the theme
     * @param theme pointer to the theme struct to use
     */
    NilmuOptions& theme(BaseTheme* theme);
    /**
     * Returns a reference to the currently used theme
     */
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
    // Converts the desired frequency in time per frame
    auto frequency = std::chrono::duration<long, std::ratio<1, Hertz>>(1);
    threshold = std::chrono::duration_cast<DurationType>(frequency);
    return *this;
}

NilmuOptions& NilmuOptions::term_width(uint32_t width){
    // Get the current terminal size according to os
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    // User-chosen terminal width cannot be bigger than actual terminal width
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
