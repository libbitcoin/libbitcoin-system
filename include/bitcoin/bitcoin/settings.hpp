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
#ifndef LIBBITCOIN_SETTINGS_HPP
#define LIBBITCOIN_SETTINGS_HPP

#include <bitcoin/bitcoin/define.hpp>

#include <cstdint>
#include <cstddef>

namespace libbitcoin {

/// Common database configuration settings, properties not thread safe.
class BC_API settings
{
public:
    settings();

    uint32_t retargeting_factor;
    uint32_t target_spacing_seconds;
    uint32_t easy_spacing_seconds;
    uint32_t timestamp_future_seconds;
    uint32_t target_timespan_seconds;
    uint32_t retarget_proof_of_work_limit;
    uint32_t no_retarget_proof_of_work_limit;

    // The upper and lower bounds for the retargeting timespan.
    uint32_t min_timespan;
    uint32_t max_timespan;

    // The target number of blocks for 2 weeks of work (2016 blocks).
    size_t retargeting_interval;
};

} // namespace libbitcoin

#endif
