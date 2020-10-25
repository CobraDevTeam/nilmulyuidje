#pragma once

#include <iostream>
#include <iterator>
#include <string>

namespace nilmu
{

struct NilmuOptions
{
    size_t      depth = 0;
    size_t      max_depth = 0;
    const char  spacer = ' ';
    const char  arrow_head = '>';
    const char  arrow_shaft = '=';
    const char  bracket_open = '[';
    const char  bracket_close = ']';
    const std::string backline = "\033[A";
} nil_options;

template <typename Iterator>
class IteratorWrapper
    : public std::iterator<std::forward_iterator_tag,
                           typename std::iterator_traits<Iterator>::value_type>
{
    public:
        using Base = std::iterator<std::forward_iterator_tag,
             typename std::iterator_traits<Iterator>::value_type>;

        explicit IteratorWrapper(Iterator iter, int64_t total, const std::string& finisher)
            : _iter(iter)
            , _current(0)
            , _total(total)
            , _finisher(finisher)
        {
            if(nil_options.depth++)
                std::cout << "\n";
            nil_options.max_depth = nil_options.depth;
            write();
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
        inline void write()
        {
            std::cout << "\r[";
            int i = 0;
            for (int e=_current; i < e; ++i)
                std::cout << nil_options.arrow_shaft;
            std::cout << nil_options.arrow_head ;
            for (; i < _total; ++i)
                std::cout << nil_options.spacer;

            std::cout << _current << "/" << _total << "] " << std::flush;

            if (_current++ == _total)
                conclude();
        }

        inline void conclude()
        {
            if(!--nil_options.depth) {
                std::cout << std::string(nil_options.max_depth, '\n');
                std::cout << _finisher << std::endl;
            }

            else
                std::cout << nil_options.backline;
        }

        Iterator _iter;
        int64_t _current;
        const int64_t _total;
        const std::string _finisher;

        // TODO make possibility to not store position
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
