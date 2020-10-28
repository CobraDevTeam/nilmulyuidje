#pragma once

#include <iterator>
#include <memory>
#include <iostream>
#include <cassert>
#include <cmath>

#include "options.hpp"
#include "util.hpp"

namespace nilmu
{

template <typename Iterator>
class IteratorWrapper
    : public std::iterator<std::forward_iterator_tag,
                           typename std::iterator_traits<Iterator>::value_type>
{
    public:
        using Base = std::iterator<std::forward_iterator_tag,
                                   typename std::iterator_traits<Iterator>::value_type>;
        using Clock = std::chrono::high_resolution_clock;

        explicit IteratorWrapper(Iterator iter, int64_t total, const std::string& finisher);
        explicit IteratorWrapper(Iterator iter);

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

        // Prefix increment operator
        IteratorWrapper& operator++();

        // Postfix increment operator
        IteratorWrapper& operator++(int);

    private:
        inline bool has_waited_enough();

        inline void write();

        inline void body() const;

        inline void conclude(size_t depth) const;

    private:
        struct DataImpl {
            DataImpl(size_t total, const std::string& finisher)
                : _current(0)
                , _total(total)
                , _finisher(finisher)
                , _last(Clock::now())
            {}
            const size_t                        _total;
            size_t                              _current;
            const std::string                   _finisher;
            std::chrono::time_point<Clock>      _last;
            typename NilmuOptions::DurationType _elapsed;
            typename NilmuOptions::DurationType _threshold;
        };

        Iterator                  _iter;
        std::unique_ptr<DataImpl> _udata_ptr;
        const short               _total_nb_digits;
        const short               _bar_offset;
};

/***************************** CONSTRUCTORS *********************************/

template <typename Iterator>
IteratorWrapper<Iterator>::IteratorWrapper(Iterator iter, int64_t total, const std::string& finisher)
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

template <typename Iterator>
IteratorWrapper<Iterator>::IteratorWrapper(Iterator iter)
    : _iter(iter)
    , _udata_ptr(nullptr)
    , _total_nb_digits(0)
    , _bar_offset(0)
{}

/***************************** OPERATORS  ***********************************/

template <typename Iterator>
IteratorWrapper<Iterator>& IteratorWrapper<Iterator>::operator++()
{
    assert(_udata_ptr);
    write();
    ++_iter;

    return *this;
}

template <typename Iterator>
IteratorWrapper<Iterator>& IteratorWrapper<Iterator>::operator++(int)
{
    assert(_udata_ptr);
    IteratorWrapper& ret = *this;
    ++(*this);
    return ret;
}

/***************************** DISPLAY FUNCTIONS ****************************/

template <typename Iterator>
bool IteratorWrapper<Iterator>::has_waited_enough()
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

template <typename Iterator>
void IteratorWrapper<Iterator>::write()
{
    if (has_waited_enough() || _udata_ptr->_current == _udata_ptr->_total)
        body();

    if (_udata_ptr->_current++ == _udata_ptr->_total)
        conclude(--nil_options.depth);
}

template <typename Iterator>
void IteratorWrapper<Iterator>::body() const
{
    auto _current = _udata_ptr->_current;
    auto _total = _udata_ptr->_total;

    const short spacer_offset = _total_nb_digits - nb_digits(static_cast<size_t>(_current));
    const int32_t current_step = (static_cast<float>(_current) / static_cast<float>(_total))
                        * (nil_options.term_width() - _bar_offset);

    std::cout << "\r[";

    nil_options.theme().write(std::cout,
                              current_step,
                              nil_options.term_width()-_bar_offset+spacer_offset);

    std::cout << _udata_ptr->_current << "/" << _udata_ptr->_total << "] " << std::flush;
}

template <typename Iterator>
void IteratorWrapper<Iterator>::conclude(size_t depth) const
{
    if(!depth) {
        std::cout << std::string(nil_options.max_depth, '\n');
        std::cout << _udata_ptr->_finisher << std::endl;
    }
    else
        std::cout << nil_options.backline;
}

}
// namespace nilmu
