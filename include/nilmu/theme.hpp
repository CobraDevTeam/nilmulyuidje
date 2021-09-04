#pragma once

#include<ostream>

struct AbstractTheme {
    virtual std::ostream& write(std::ostream& stream, size_t finished, size_t remaining) const = 0;
};

template <typename Derived>
struct BaseTheme : AbstractTheme
{
    virtual std::ostream& write(std::ostream& stream, size_t finished, size_t remaining) const override
    {
        return static_cast<const Derived*>(this)->write(stream, finished, remaining);
    }
};


struct AsciiTheme : public BaseTheme<AsciiTheme>
{
    std::ostream& write(std::ostream& stream, size_t finished, size_t remaining) const final override
    {
        size_t i = 0;
        for(; i < finished; i++)
             stream << '=';
        stream << '>';
        for (; i < remaining; i++) {
            stream << ' ';
        }

        return stream;
    }
};

struct RollTheme : public BaseTheme<AsciiTheme>
{
    std::ostream& write(std::ostream& stream, size_t finished, size_t remaining) const final override
    {
        size_t i = 0;
        for(; i < finished; i++)
            stream << ' ';
        stream << "\u15E7";
        for (;i < remaining; i++)
            stream << "\u00B7";

        return stream;
    }
};
