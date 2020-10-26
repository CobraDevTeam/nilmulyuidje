#pragma once

#include <iostream>
#include <iterator>
#include <string>
#include <chrono>

namespace nilmu
{

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
    DurationType threshold = std::chrono::duration_cast<DurationType>(std::chrono::duration<long, std::ratio<1, 10>>(1));

    template <std::intmax_t Hertz>
    NilmuOptions& frequency()
    {
        threshold = std::chrono::duration_cast<DurationType>(std::chrono::duration<long, std::ratio<1, Hertz>>(1));
        return *this;
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
            int i = 0;
            for (int e=_current; i < e; ++i)
                std::cout << nil_options.arrow_shaft;
            std::cout << nil_options.arrow_head ;
            for (; i < _total; ++i)
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
