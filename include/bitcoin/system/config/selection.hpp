/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_SELECTION_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_SELECTION_HPP

#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/config/token.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// The chain::selection words.
struct selection_words
{
    static constexpr std::array<std::string_view, 6> words
    {
        "", "mainnet", "testnet3", "testnet4", "regtest", "signet"
    };
};

/// Serialization helper for chain::selection.
using selection = token<chain::selection, selection_words>;

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
