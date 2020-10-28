#pragma once

#include <iostream>
#include <iterator>
#include <string>
#include <chrono>
#include <memory>
#include <cassert>
#include <cmath>
#include <sys/ioctl.h>
#include <unistd.h>

#include "util.hpp"

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

    template <std::intmax_t Hertz>
    NilmuOptions& frequency()
    {
        threshold = std::chrono::duration_cast<DurationType>(std::chrono::duration<long, std::ratio<1, Hertz>>(1));
        return *this;
    }
    NilmuOptions& term_width(uint32_t width){
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        _term_width = std::min<size_t>(width, w.ws_col);
        return *this;
    }
    uint32_t term_width() const
    {
        return _term_width;
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
            , _udata_ptr(std::make_unique<DataImpl>(total, finisher))
            , _total_nb_digits(nb_digits(static_cast<size_t>(total)))
            // 4 = bracket size open and close + arrow size + bar between _current and _total
            // TODO change the offset with the theme
            , _bar_offset(_total_nb_digits*2 + 4)
        {
            if(nil_options.depth++)
                std::cout << "\n";
            nil_options.max_depth = nil_options.depth;
            body();
            _udata_ptr->_current++;
        }

        explicit IteratorWrapper(Iterator iter)
            : _iter(iter)
            , _udata_ptr(nullptr)
            , _total_nb_digits(0)
            , _bar_offset(0)
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
            assert(_udata_ptr);
            write();
            ++_iter;

            return *this;
        }

        IteratorWrapper& operator++(int)
        {
            assert(_udata_ptr);
            IteratorWrapper& ret = *this;
            ++(*this);
            return ret;
        }

    private:
        inline bool has_waited_enough()
        {
            auto& _last = _udata_ptr->_last;

            auto now = Clock::now();
            _udata_ptr->_elapsed += std::chrono::duration_cast<decltype(_udata_ptr->_elapsed)>(now-_last);
            _udata_ptr->_last = now;
            bool waited_enough = (_udata_ptr->_elapsed > nil_options.threshold);
            if(waited_enough)
                _udata_ptr->_elapsed -= nil_options.threshold;
            return waited_enough;
        }

        inline void write()
        {
            if (has_waited_enough() || _udata_ptr->_current == _udata_ptr->_total)
                body();

            if (_udata_ptr->_current++ == _udata_ptr->_total)
                conclude(--nil_options.depth);
        }

        inline void body() const
        {
            auto _current = _udata_ptr->_current;
            auto _total = _udata_ptr->_total;

            short spacer_offset = _total_nb_digits - nb_digits(static_cast<size_t>(_current));
            int32_t current_step = (static_cast<float>(_current) / static_cast<float>(_total))
                                * (nil_options.term_width() - _bar_offset);

            std::cout << "\r[";

            int32_t i = 0;
            for (int e=current_step; i < e; ++i)
                std::cout << nil_options.arrow_shaft;
            std::cout << nil_options.arrow_head ;
            for (; i < nil_options.term_width()-_bar_offset+spacer_offset; ++i)
                std::cout << nil_options.spacer;

            std::cout << _udata_ptr->_current << "/" << _udata_ptr->_total << "] " << std::flush;
        }

        inline void conclude(size_t depth) const
        {
            if(!depth) {
                std::cout << std::string(nil_options.max_depth, '\n');
                std::cout << _udata_ptr->_finisher << std::endl;
            }
            else
                std::cout << nil_options.backline;
        }

    private:
        struct DataImpl {
            DataImpl(size_t total, const std::string& finisher)
                : _current(0)
                , _total(total)
                , _finisher(finisher)
                , _last(Clock::now())
            {}
            const size_t                  _total;
            size_t                        _current;
            const std::string              _finisher;
            std::chrono::time_point<Clock> _last;
            typename NilmuOptions::DurationType _elapsed;
            typename NilmuOptions::DurationType _threshold;
        };

        Iterator                  _iter;
        std::unique_ptr<DataImpl> _udata_ptr;
        const short               _total_nb_digits;
        const short               _bar_offset;
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
