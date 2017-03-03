/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_LOG_STATSD_SOURCE_HPP
#define LIBBITCOIN_LOG_STATSD_SOURCE_HPP

#include <chrono>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/features.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/threading_models.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/log/features/counter.hpp>
#include <bitcoin/bitcoin/log/features/gauge.hpp>
#include <bitcoin/bitcoin/log/features/metric.hpp>
#include <bitcoin/bitcoin/log/features/rate.hpp>
#include <bitcoin/bitcoin/log/features/timer.hpp>

namespace libbitcoin {
namespace log {

class BC_API statsd_source
  : public boost::log::sources::basic_composite_logger<char, statsd_source,
        boost::log::sources::multi_thread_model<boost::log::aux::light_rw_mutex>,
        boost::log::sources::features<features::metric, features::counter,
            features::gauge, features::timer, features::rate>>
{
    BOOST_LOG_FORWARD_LOGGER_MEMBERS(statsd_source)
};

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(stats, statsd_source);

#define BC_STATS_SIMPLE(name, sequence) \
    BOOST_LOG_WITH_PARAMS(bc::log::stats::get(), \
        (bc::log::keywords::metric = (name))sequence)

#define BC_STATS_WITH_RATE(name, rate, sequence) \
    BOOST_LOG_WITH_PARAMS(bc::log::stats::get(), \
        (bc::log::keywords::metric = (name)) \
        (bc::log::keywords::rate = (rate))sequence)

#define BC_STATS_COUNTER(name, value) \
    BC_STATS_SIMPLE(name, (bc::log::keywords::counter = (value)))

#define BC_STATS_COUNTER_RATE(name, value, rate) \
    BC_STATS_WITH_RATE(name, rate, (bc::log::keywords::counter = (value)))

#define BC_STATS_GAUGE(name, value) \
    BC_STATS_SIMPLE(name, (bc::log::keywords::gauge = (value)))

#define BC_STATS_GAUGE_RATE(name, value, rate) \
    BC_STATS_WITH_RATE(name, rate, (bc::log::keywords::gauge = (value)))

#define BC_STATS_TIMER(name, value) \
    BC_STATS_SIMPLE(name, (bc::log::keywords::timer = (value)))

#define BC_STATS_TIMER_RATE(name, value, rate) \
    BC_STATS_WITH_RATE(name, rate, (bc::log::keywords::timer = (value)))

} // namespace log
} // namespace libbitcoin

#endif
