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
#ifndef LIBBITCOIN_LOG_FEATURES_METRIC_IPP
#define LIBBITCOIN_LOG_FEATURES_METRIC_IPP

#include <bitcoin/bitcoin/log/features/metric.hpp>

namespace libbitcoin {
namespace log {
namespace features {

template<typename BaseType>
metric_feature<BaseType>::metric_feature()
  : BaseType(), metric_attr_(metric_type())
{
    BaseType::add_attribute_unlocked(attributes::metric.get_name(), metric_attr_);
}

template<typename BaseType>
metric_feature<BaseType>::metric_feature(
    metric_feature const& that)
  : BaseType(static_cast<BaseType const&>(that)), metric_attr_(that.metric_attr_)
{
    BaseType::attributes()[attributes::metric.get_name()] = metric_attr_;
}

template<typename BaseType>
template<typename ArgsT>
metric_feature<BaseType>::metric_feature(
    ArgsT const& args)
  : BaseType(args), metric_attr_(args[keywords::metric || metric_type()])
{
    BaseType::add_attribute_unlocked(attributes::metric.get_name(), metric_attr_);
}

template<typename BaseType>
typename metric_feature<BaseType>::metric_type
    metric_feature<BaseType>::metric() const
{
    BOOST_LOG_EXPR_IF_MT(boost::log::aux::shared_lock_guard<const threading_model> lock(this->get_threading_model());)
    return metric_attr_.get();
}

template<typename BaseType>
void metric_feature<BaseType>::metric(metric_type const& ch)
{
    BOOST_LOG_EXPR_IF_MT(boost::log::aux::exclusive_lock_guard<threading_model> lock(this->get_threading_model());)
    metric_attr_.set(ch);
}

template<typename BaseType>
typename metric_feature<BaseType>::metric_attribute const&
    metric_feature<BaseType>::get_metric_attribute() const
{
    return metric_attr_;
}

template<typename BaseType>
template<typename ArgsT>
boost::log::record
    metric_feature<BaseType>::open_record_unlocked(
        ArgsT const& args)
{
    return open_record_with_metric_unlocked(args, args[keywords::metric | boost::parameter::void_()]);
}

template<typename BaseType>
void metric_feature<BaseType>::swap_unlocked(
    metric_feature& that)
{
    BaseType::swap_unlocked(static_cast<BaseType&>(that));
    metric_attr_.swap(that.metric_attr_);
}

template<typename BaseType>
template<typename ArgsT, typename T>
boost::log::record
    metric_feature<BaseType>::open_record_with_metric_unlocked(
        ArgsT const& args, T const& ch)
{
    metric_attr_.set(ch);
    return BaseType::open_record_unlocked(args);
}

template<typename BaseType>
template<typename ArgsT>
boost::log::record
    metric_feature<BaseType>::open_record_with_metric_unlocked(
        ArgsT const& args, boost::parameter::void_)
{
    return BaseType::open_record_unlocked(args);
}

} // namespace features
} // namespace log
} // namespace libbitcoin

#endif
