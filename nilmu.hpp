#pragma once

#include <iostream>
#include <iterator>
#include <string>
#include <chrono>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cmath>



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
    while (x > (((uint64_t) 1) << 32)-1){
        x /= 10;
        digit_count++;
    }
    return digit_count + nb_digits(static_cast<uint32_t>(x));
}

struct NilmuOptions
{
    using DurationType = std::chrono::microseconds;
    size_t      depth = 0;
    size_t      max_depth = 0;
    const char  spacer = ' ';
    const char  arrow_head = '>';
    const char  arrow_shaft = '=';
    const char  bracket_open = '[';
    const char  bracket_close = ']';
    const std::string backline = "\033[A";
    const size_t term_width = set_term_width(80);
    DurationType threshold = std::chrono::duration_cast<DurationType>(std::chrono::duration<long, std::ratio<1, 10>>(1));

    template <std::intmax_t Hertz>
    NilmuOptions& frequency()
    {
        threshold = std::chrono::duration_cast<DurationType>(std::chrono::duration<long, std::ratio<1, Hertz>>(1));
        return *this;
    }
    static size_t set_term_width(size_t width){
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return std::min<size_t>(width, w.ws_col);
    }

} nil_options;

template <typename Iterator>
class IteratorWrapper
    : public std::iterator<std::forward_iterator_tag,
                           typename std::iterator_traits<Iterator>::value_type>
{
    public:
        using Base = std::iterator<std::forward_iterator_tag,
             typename std::iterator_traits<Iterator>::value_type>;
        using Clock = std::chrono::high_resolution_clock;

        explicit IteratorWrapper(Iterator iter, int64_t total, const std::string& finisher)
            : _iter(iter)
            , _current(0)
            , _total(total)
            , _total_nb_digits(nb_digits((size_t) total))
            , _bar_offset(_total_nb_digits*2 + 4)
            // 4 = bracket size open and close + arrow size + bar between _current and _total
            // TODO change the offset with the theme
            , _finisher(finisher)
            , _last(Clock::now())
        {
            if(nil_options.depth++)
                std::cout << "\n";
            nil_options.max_depth = nil_options.depth;
            body();
            _current++;
        }

        explicit IteratorWrapper(Iterator iter)
            : _iter(iter)
            , _current(0)
            , _total(-1)
            , _total_nb_digits(-1)
            , _bar_offset(-1)
        {}

        bool operator==(const IteratorWrapper& other)
        {
            return _iter == other._iter;
        }

        bool operator!=(const IteratorWrapper& other)
        {
            return _iter != other._iter;
        }

        typename Base::reference operator*()
        {
            return *_iter;
        }

        typename Base::pointer operator->()
        {
            return _iter.operator->();
        }

        IteratorWrapper& operator++()
        {
            write();
            ++_iter;

            return *this;
        }

        IteratorWrapper& operator++(int)
        {
            IteratorWrapper ret = *this;
            ++(*this);
            return ret;
        }

    private:
        inline bool has_waited_enough()
        {
            auto now = Clock::now();
            _elapsed += std::chrono::duration_cast<decltype(_elapsed)>(now-_last);
            _last = now;
            bool waited_enough = (_elapsed > nil_options.threshold);
            if(waited_enough)
                _elapsed -= nil_options.threshold;
            return waited_enough;
        }

        inline void write()
        {
            if (has_waited_enough() || _current == _total)
                body();

            if (_current++ == _total)
                conclude(--nil_options.depth);
        }

        inline void body() const
        {
            std::cout << "\r[";
            short spacer_offset = _total_nb_digits - nb_digits(static_cast<size_t>(_current));

            int i = 0;
            int current_step = (static_cast<float>(_current) / static_cast<float>(_total))
                * (nil_options.term_width - _bar_offset);
            for (int e=current_step; i < e; ++i)
                std::cout << nil_options.arrow_shaft;
            std::cout << nil_options.arrow_head ;
            for (; i < nil_options.term_width-_bar_offset+spacer_offset; ++i)
                std::cout << nil_options.spacer;

            std::cout << _current << "/" << _total << "] " << std::flush;
        }

        inline void conclude(size_t depth) const
        {
            if(!depth) {
                std::cout << std::string(nil_options.max_depth, '\n');
                std::cout << _finisher << std::endl;
            }
            else
                std::cout << nil_options.backline;
        }

        Iterator                       _iter;
        int64_t                        _current;
        const int64_t                  _total;
        const short                    _total_nb_digits;
        const short                    _bar_offset;
        const std::string              _finisher;
        std::chrono::time_point<Clock> _last;
        typename NilmuOptions::DurationType _elapsed;
};

template <typename InputIt>
class NilmuIter
{
    public:
        NilmuIter(InputIt first, InputIt last, const std::string& finisher)
            : _it(first)
            , _last(last)
            , _total(last-first)
            , _finisher(finisher)
        {}

        template <typename Container>
        NilmuIter(Container& container, const std::string& finisher)
            : NilmuIter(container.begin(), container.end(), finisher)
        {}

        IteratorWrapper<InputIt> begin()
        {
            return IteratorWrapper<InputIt>(_it, _total, _finisher);
        }

        IteratorWrapper<InputIt> end()
        {
            return IteratorWrapper<InputIt>(_last);
        }

    private:
        InputIt _it;
        InputIt _last;
        int64_t _total;
        const std::string _finisher;
};

template <typename Iterator, typename Nilmu = NilmuIter<Iterator>>
Nilmu nilmu(Iterator&& begin, Iterator&& end)
{
    return Nilmu(std::forward<Iterator>(begin),
                 std::forward<Iterator>(end));
}

template <typename Container, typename Nilmu = NilmuIter<typename Container::iterator>>
Nilmu nilmu(Container& container, const std::string& finisher)
{
    return Nilmu(container, finisher);
}

}

// namespace nil
