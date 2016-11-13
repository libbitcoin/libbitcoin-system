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
#ifndef LIBBITCOIN_LOG_FEATURES_METRIC_HPP
#define LIBBITCOIN_LOG_FEATURES_METRIC_HPP

#include <boost/log/sources/features.hpp>
#include <boost/log/utility/strictest_lock.hpp>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {
namespace log {

namespace attributes {

BOOST_LOG_ATTRIBUTE_KEYWORD(metric, "Metric", std::string)

} // namespace attributes

namespace features {

template<typename BaseType>
class metric_feature : public BaseType
{
public:
    typedef typename BaseType::char_type char_type;
    typedef typename BaseType::threading_model threading_model;

    typedef std::string metric_type;
    typedef boost::log::attributes::mutable_constant<metric_type> metric_attribute;

    typedef typename boost::log::strictest_lock<
        boost::lock_guard<threading_model>
    >::type open_record_lock;

    typedef typename boost::log::strictest_lock<
        typename BaseType::swap_lock,
        boost::log::aux::exclusive_lock_guard<threading_model>
    >::type swap_lock;

public:
    metric_feature();
    metric_feature(metric_feature const& that);
    template<typename ArgsT>
    metric_feature(ArgsT const& args);

    metric_type metric() const;
    void metric(metric_type const& ch);

protected:
    metric_attribute const& get_metric_attribute() const;

    template<typename ArgsT>
    boost::log::record open_record_unlocked(ArgsT const& args);

    void swap_unlocked(metric_feature& that);

private:
    template<typename ArgsT, typename T>
    boost::log::record open_record_with_metric_unlocked(ArgsT const& args,
        T const& ch);

    template<typename ArgsT>
    boost::log::record open_record_with_metric_unlocked(ArgsT const& args,
        boost::parameter::void_);

private:
    metric_attribute metric_attr_;
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
} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/log/features/metric.ipp>

#endif
