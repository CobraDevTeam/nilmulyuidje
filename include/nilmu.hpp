#pragma once

#include "wrapper.hpp"

namespace nilmu
{

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
