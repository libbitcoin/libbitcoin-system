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

// Sponsored in part by Digital Contract Design, LLC

#ifndef LIBBITCOIN_SYSTEM_CHAIN_NEUTRINO_FILTER_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_NEUTRINO_FILTER_HPP

#include <istream>
#include <memory>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/chain/block.hpp>
#include <bitcoin/system/chain/compact_filter.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/reader.hpp>
#include <bitcoin/system/utility/writer.hpp>
#include <bitcoin/system/wallet/payment_address.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

data_chunk compute_neutrino_filter(const block& validated_block);

bool match_neutrino_filter(const compact_filter& filter,
    const script& script);

bool match_neutrino_filter(const compact_filter& filter,
    const script::list& scripts);

bool match_neutrino_filter(const compact_filter& filter,
    const wallet::payment_address& address);

bool match_neutrino_filter(const compact_filter& filter,
    const wallet::payment_address::list& addresses);

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
