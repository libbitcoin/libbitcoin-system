/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_LOG_FEATURES_RATE_HPP
#define LIBBITCOIN_LOG_FEATURES_RATE_HPP

#include <boost/log/sources/features.hpp>
#include <boost/log/sources/threading_models.hpp>
#include <boost/log/utility/strictest_lock.hpp>

namespace libbitcoin {
namespace log {

namespace attributes {

BOOST_LOG_ATTRIBUTE_KEYWORD(rate, "Rate", float)

} // namespace attributes

namespace features {

template<typename BaseType>
class rate_feature
  : public BaseType
{
public:
    typedef typename BaseType::char_type char_type;
    typedef typename BaseType::threading_model threading_model;

public:
    rate_feature();
    rate_feature(const rate_feature& other);

    template<typename Arguments>
    rate_feature(const Arguments& arguments);

    typedef typename boost::log::strictest_lock<
        boost::lock_guard<threading_model>,
        typename BaseType::open_record_lock,
        typename BaseType::add_attribute_lock,
        typename BaseType::remove_attribute_lock
    >::type open_record_lock;

protected:
    template<typename Arguments>
    boost::log::record open_record_unlocked(const Arguments& arguments);

private:
    template<typename Value>
    boost::log::attribute_set::iterator add_rate_unlocked(
        boost::log::attribute_set& set, const Value& value);

    boost::log::attribute_set::iterator add_rate_unlocked(
        boost::log::attribute_set& set, boost::parameter::void_);
};

struct rate
{
    template<typename BaseType>
    struct apply
    {
        typedef rate_feature<BaseType> type;
    };
};

} // namespace features

namespace keywords {

BOOST_PARAMETER_KEYWORD(tag, rate)

} // namespace keywords

} // namespace log
} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/log/features/rate.ipp>

#endif
