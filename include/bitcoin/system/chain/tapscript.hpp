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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_TAPSCRIPT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_TAPSCRIPT_HPP

#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API tapscript
{
public:
    using keys_t = std::array<ec_xonly, taproot_max_keys>;

    DEFAULT_COPY_MOVE_DESTRUCT(tapscript);
    
    inline static bool is_control(const data_chunk& control) NOEXCEPT;

    inline tapscript(chunk_cptr&& control) NOEXCEPT;
    inline tapscript(const chunk_cptr& control) NOEXCEPT;

    inline bool is_valid() const NOEXCEPT;
    inline bool is_tapscript() const NOEXCEPT;
    inline bool parity() const NOEXCEPT;
    inline size_t count() const NOEXCEPT;
    inline uint8_t version() const NOEXCEPT;
    inline const ec_xonly& key() const NOEXCEPT;
    inline const keys_t& keys() const NOEXCEPT;

private:
    chunk_cptr control_;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/chain/tapscript.ipp>

#endif
