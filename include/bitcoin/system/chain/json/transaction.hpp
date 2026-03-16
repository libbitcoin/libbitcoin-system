/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_JSON_TRANSACTION_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_JSON_TRANSACTION_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/chain/json/macros.hpp>
#include <bitcoin/system/chain/transaction.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

DECLARE_JSON_TAG_INVOKE(transaction);
DECLARE_JSON_TAG_INVOKE(transaction::cptr);

DECLARE_JSON_TAGGED_INVOKE(bitcoind_tag, transaction);
DECLARE_JSON_TAGGED_INVOKE(bitcoind_hashed_tag, transaction);
DECLARE_JSON_TAGGED_INVOKE(bitcoind_verbose_tag, transaction);
DECLARE_JSON_TAGGED_INVOKE(bitcoind_embedded_tag, transaction);

// The tag wrapper masks the vector, so these must be explicit.
DECLARE_JSON_TAGGED_INVOKE(bitcoind_tag, transaction_cptrs);
DECLARE_JSON_TAGGED_INVOKE(bitcoind_hashed_tag, transaction_cptrs);
DECLARE_JSON_TAGGED_INVOKE(bitcoind_verbose_tag, transaction_cptrs);
DECLARE_JSON_TAGGED_INVOKE(bitcoind_embedded_tag, transaction_cptrs);

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
