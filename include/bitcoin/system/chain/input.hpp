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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_INPUT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_INPUT_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <vector>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/chain/prevout.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/witness.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API input
{
public:
    typedef std::vector<input> list;
    typedef std::shared_ptr<input> ptr;

    // Constructors.
    // ------------------------------------------------------------------------

    input();

    input(input&& other);
    input(const input& other);

    input(chain::point&& point, chain::script&& script, uint32_t sequence);
    input(const point& point, const chain::script& script, uint32_t sequence);

    input(chain::point&& point, chain::script&& script,
        chain::witness&& witness, uint32_t sequence);
    input(const chain::point& point, const chain::script& script,
        const chain::witness& witness, uint32_t sequence);

    input(const data_chunk& data);
    input(std::istream& stream);
    input(reader& source);

    // Operators.
    // ------------------------------------------------------------------------

    input& operator=(input&& other);
    input& operator=(const input& other);

    bool operator==(const input& other) const;
    bool operator!=(const input& other) const;

    // Deserialization.
    // ------------------------------------------------------------------------

    bool from_data(const data_chunk& data);
    bool from_data(std::istream& stream);
    bool from_data(reader& source);

    // Deserialization result.
    bool is_valid() const;

    // Serialization.
    // ------------------------------------------------------------------------

    data_chunk to_data() const;
    void to_data(std::ostream& stream) const;
    void to_data(writer& sink) const;

    // Witness accounts for witness bytes, but are serialized independently.
    size_t serialized_size(bool witness) const;

    // Properties.
    // ------------------------------------------------------------------------

    /// Native properties.
    const chain::point& point() const;
    const chain::script& script() const;
    const chain::witness& witness() const;
    uint32_t sequence() const;

    // Methods.
    // ------------------------------------------------------------------------

    bool is_final() const;
    bool is_locked(size_t height, uint32_t median_time_past) const;
    bool reserved_hash(hash_digest& out) const;
    size_t signature_operations(bool bip16, bool bip141) const;

    /// Public mutable metadata access, not copied or compared.
    chain::prevout prevout;

protected:
    // So that witness may be set late in deserialization.
    friend class transaction;

    input(chain::point&& point, chain::script&& script,
        chain::witness&& witness, uint32_t sequence, bool valid);
    input(const chain::point& point, const chain::script& script,
        const chain::witness& witness, uint32_t sequence, bool valid);

    void reset();

private:
    bool embedded_script(chain::script& out) const;

    chain::point point_;
    chain::script script_;
    chain::witness witness_;
    uint32_t sequence_;
    bool valid_;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
