#pragma once

#include<ostream>

struct BaseTheme
{
    virtual std::ostream& write(std::ostream& stream, size_t finished, size_t remaining) const =0;
};

struct AsciiTheme : public BaseTheme
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

struct RollTheme : public BaseTheme
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
