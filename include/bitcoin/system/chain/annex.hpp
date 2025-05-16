/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_ANNEX_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_ANNEX_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class witness;

/// Annex is an abstraction over a witness stack.
class BC_API annex
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(annex);

    inline annex(const witness& owner) NOEXCEPT;

    inline size_t size() const NOEXCEPT;
    inline const data_chunk& data() const NOEXCEPT;
    inline const hash_digest hash() const NOEXCEPT;
    inline operator bool() const NOEXCEPT;

private:
    const witness& witness_;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
