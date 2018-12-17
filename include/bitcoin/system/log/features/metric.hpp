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
#ifndef LIBBITCOIN_SYSTEM_LOG_FEATURES_METRIC_HPP
#define LIBBITCOIN_SYSTEM_LOG_FEATURES_METRIC_HPP

#include <boost/log/sources/features.hpp>
#include <boost/log/sources/threading_models.hpp>
#include <boost/log/utility/strictest_lock.hpp>

namespace libbitcoin {
namespace system {
namespace log {

namespace attributes {

BOOST_LOG_ATTRIBUTE_KEYWORD(metric, "Metric", std::string)

} // namespace attributes

namespace features {

template<typename BaseType>
class metric_feature
  : public BaseType
{
public:
    typedef typename BaseType::char_type char_type;
    typedef typename BaseType::threading_model threading_model;

    typedef std::string metric_type;
    typedef boost::log::attributes::mutable_constant<metric_type>
        metric_attribute;

    typedef typename boost::log::strictest_lock<
        boost::lock_guard<threading_model>
    >::type open_record_lock;

    typedef typename boost::log::strictest_lock<
        typename BaseType::swap_lock,
        boost::log::aux::exclusive_lock_guard<threading_model>
    >::type swap_lock;

public:
    metric_feature();
    metric_feature(const metric_feature& other);

    template<typename Arguments>
    metric_feature(const Arguments& arguments);

    metric_type metric() const;
    void metric(const metric_type& value);

protected:
    const metric_attribute& get_metric_attribute() const;

    template<typename Arguments>
    boost::log::record open_record_unlocked(const Arguments& arguments);

    void swap_unlocked(metric_feature& other);

private:
    template<typename Arguments, typename Value>
    boost::log::record open_record_with_metric_unlocked(
        const Arguments& arguments, const Value& value);

    template<typename Arguments>
    boost::log::record open_record_with_metric_unlocked(
        const Arguments& arguments, boost::parameter::void_);

private:
    metric_attribute metric_attribute_;
};

struct metric
{
    template<typename BaseType>
    struct apply
    {
        typedef metric_feature<BaseType> type;
    };
};

} // namespace features

namespace keywords {

BOOST_PARAMETER_KEYWORD(tag, metric)

} // namespace keywords

} // namespace log
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/log/features/metric.ipp>

#endif
