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

#ifndef LIBBITCOIN_SYSTEM_UTILITY_NEUTRINO_FILTER_HPP
#define LIBBITCOIN_SYSTEM_UTILITY_NEUTRINO_FILTER_HPP

#include <istream>
#include <memory>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/chain/block.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/message/compact_filter.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/reader.hpp>
#include <bitcoin/system/utility/writer.hpp>
#include <bitcoin/system/wallet/payment_address.hpp>

namespace libbitcoin {
namespace system {
namespace neutrino {

data_chunk compute_filter(const chain::block& validated_block);

hash_digest compute_filter_header(const hash_digest& previous_block_hash,
    const data_chunk& filter);

bool match_filter(const message::compact_filter& filter,
    const chain::script& script);

bool match_filter(const message::compact_filter& filter,
    const chain::script::list& scripts);

bool match_filter(const message::compact_filter& filter,
    const wallet::payment_address& address);

bool match_filter(const message::compact_filter& filter,
    const wallet::payment_address::list& addresses);

} // namespace neutrino
} // namespace system
} // namespace libbitcoin

#endif
