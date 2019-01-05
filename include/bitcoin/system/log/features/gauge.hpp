/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_LOG_FEATURES_GAUGE_HPP
#define LIBBITCOIN_SYSTEM_LOG_FEATURES_GAUGE_HPP

#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/features.hpp>
#include <boost/log/utility/strictest_lock.hpp>
#include <boost/log/sources/threading_models.hpp>

namespace libbitcoin {
namespace system {
namespace log {

namespace attributes {

BOOST_LOG_ATTRIBUTE_KEYWORD(gauge, "Gauge", uint64_t)

} // namespace attributes

namespace features {

template<typename BaseType>
class gauge_feature
  : public BaseType
{
public:
    typedef typename BaseType::char_type char_type;
    typedef typename BaseType::threading_model threading_model;

public:
    gauge_feature();
    gauge_feature(const gauge_feature& other);

    template<typename Arguments>
    gauge_feature(const Arguments& arguments);

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
    boost::log::attribute_set::iterator add_gauge_unlocked(
        boost::log::attribute_set& set, const Value& value);

    boost::log::attribute_set::iterator add_gauge_unlocked(
        boost::log::attribute_set& set, boost::parameter::void_);
};

struct gauge
{
    template<typename BaseType>
    struct apply
    {
        typedef gauge_feature<BaseType> type;
    };
};

} // namespace features

namespace keywords {

BOOST_PARAMETER_KEYWORD(tag, gauge)

} // namespace keywords

} // namespace log
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/log/features/gauge.ipp>

#endif
