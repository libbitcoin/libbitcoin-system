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
#include <cstdint>
#include <bitcoin/bitcoin/settings.hpp>

namespace libbitcoin {

// Common default values (no settings context).
settings::settings()
  : retargeting_factor(4),
    target_spacing_seconds(10 * 60),
    easy_spacing_seconds(20 * 60),
    timestamp_future_seconds(2 * 60 * 60),
    target_timespan_seconds(2 * 7 * 24 * 60 * 60),
    retarget_proof_of_work_limit(0x1d00ffff),
    no_retarget_proof_of_work_limit(0x207fffff),
    min_timespan(target_timespan_seconds / retargeting_factor),
    max_timespan(target_timespan_seconds * retargeting_factor),
    retargeting_interval(target_timespan_seconds / target_spacing_seconds)
{
}

uint32_t settings::work_limit(bool retarget) const
{
    return retarget ? retarget_proof_of_work_limit :
        no_retarget_proof_of_work_limit;
}

} // namespace libbitcoin
