/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_LOG_FEATURES_TIMER_HPP
#define LIBBITCOIN_LOG_FEATURES_TIMER_HPP

#include <chrono>
#include <boost/log/sources/features.hpp>
#include <boost/log/utility/strictest_lock.hpp>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {
namespace log {

namespace attributes {

BOOST_LOG_ATTRIBUTE_KEYWORD(timer, "Timer", std::chrono::milliseconds)

} // namespace attributes

namespace features {

template<typename BaseType>
class timer_feature : public BaseType
{
public:
    typedef typename BaseType::char_type char_type;
    typedef typename BaseType::threading_model threading_model;

public:
    timer_feature();
    timer_feature(timer_feature const& that);
    template<typename ArgsT>
    timer_feature(ArgsT const& args);

    typedef typename boost::log::strictest_lock<
        boost::lock_guard<threading_model>,
        typename BaseType::open_record_lock,
        typename BaseType::add_attribute_lock,
        typename BaseType::remove_attribute_lock
    >::type open_record_lock;

protected:
    template<typename ArgsT>
    boost::log::record open_record_unlocked(ArgsT const& args);

private:
    template<typename T>
    boost::log::attribute_set::iterator add_timer_unlocked(
        boost::log::attribute_set& attrs,
        T const& value);

    boost::log::attribute_set::iterator add_timer_unlocked(
        boost::log::attribute_set& attrs,
        boost::parameter::void_);
};

struct timer
{
    template<typename BaseType>
    struct apply
    {
        typedef timer_feature<BaseType> type;
    };
};

} // namespace features

namespace keywords {

BOOST_PARAMETER_KEYWORD(tag, timer)

} // namespace keywords

} // namespace log
} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/log/features/timer.ipp>

#endif
