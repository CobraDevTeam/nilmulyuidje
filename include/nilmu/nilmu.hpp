#pragma once

#include "wrapper.hpp"

namespace nilmu
{

template <typename Container, typename InputIt = typename Container::iterator>
class NilmuIter
{
    public:
        using iterator = IteratorWrapper<InputIt>;
        using const_iterator = const iterator;

    public:
        /**
         * Constructs the NilmuIter wrapper around a range defined by begin/end iterators
         * @param first beginning of the range
         * @param last end of the range (first-past-last)
         * @param finisher string to print at the end of the loop
         */
        NilmuIter(InputIt first, InputIt last, const std::string& finisher)
            : _it(first)
            , _last(last)
            , _total(last-first)
            , _finisher(finisher)
        {}

        /**
         * Constructs the NilmuIter wrapper around an rvalue reference of a container
         * Takes ownership of the container to avoid segfaults during iteration
         * @param container Container to wrap around
         * @param finisher string to print at the end of the loop
         */
        NilmuIter(Container&& container, const std::string& finisher)
            : NilmuIter(container.begin(), container.end(), finisher)
        {
            _container = std::move(container);
        }

        /*
         * Constructs the NilmuIter wrapper around an lvalue reference of a container
         * @param container Container to wrap around
         * @param finisher string to print at the end of the loop
         */
        template <typename OtherContainer>
        NilmuIter(OtherContainer& container, const std::string& finisher)
            : NilmuIter(container.begin(), container.end(), finisher)
        {}

        /**
         * Returns an iterator to the beginning
         */
        iterator begin()
        {
            return IteratorWrapper<InputIt>(_it, _total, _finisher);
        }

        /**
         * Returns an iterator to the beginning
         */
        const_iterator cbegin() const
        {
            return IteratorWrapper<InputIt>(_it, _total, _finisher);
        }

        /**
         * Returns an iterator to the end
         */
        iterator end()
        {
            return IteratorWrapper<InputIt>(_last);
        }

        /**
         * Returns an const iterator to the end
         */
        const_iterator cend() const
        {
            return IteratorWrapper<InputIt>(_last);
        }

    private:
        /// Current iterator
        InputIt _it;
        /// Last iterator
        InputIt _last;
        /// Length of the range to iterate over
        int64_t _total;
        /// string to print at the end of the loop
        const std::string _finisher;
        /// Container to take ownership for as long as required (when necessary)
        /// When ownership is not required, type will be EmptyType
        Container _container;
};

/**
 * Empty struct which is only used to override the Container template in NilmuIter
 * when we know that the container itself is not needed (no need to take ownership)
 */
struct EmptyType{};

/**
 * Wrapper function which returns the nilmu object around generic iterators
 * @param first,last range of elements wrap around
 * @param finisher string to print at the end of the loop
 */
template <typename Iterator,
          // Use EmptyType to avoid overhead, because container is not relevant here
          typename Nilmu = NilmuIter<EmptyType, Iterator>>
Nilmu nilmu(Iterator&& first, Iterator&& last, const std::string& finisher)
{
    return Nilmu(std::forward<Iterator>(first),
                 std::forward<Iterator>(last),
                 finisher);
}

/**
 * Wrapper function which returns the nilmu object around a generic container
 * @param container the iterable container (vector, array ...) to wrap around
 * @param finisher string to print at the end of the loop
 */
template <typename Container,
          // Remove-reference required because of universal reference
          typename Nilmu = NilmuIter<typename std::remove_reference_t<Container>>>
Nilmu nilmu(Container&& container, const std::string& finisher)
{
    return Nilmu(std::forward<Container>(container), finisher);
}

template <typename Container,
          // Remove-reference required because of universal reference
          typename Nilmu = NilmuIter<EmptyType, typename std::remove_reference_t<Container>::iterator>>
Nilmu nilmu(Container& container, const std::string& finisher)
{
    return Nilmu(container, finisher);
}

}

// namespace nil
