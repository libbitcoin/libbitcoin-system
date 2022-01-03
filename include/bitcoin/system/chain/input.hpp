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
    typedef std::shared_ptr<const input> ptr;

    // Constructors.
    // ------------------------------------------------------------------------

    /// Default input is an invalid null point object with an invalid prevout.
    input();

    input(input&& other);
    input(const input& other);

    input(chain::point&& point, chain::script&& script, uint32_t sequence);
    input(const chain::point& point, const chain::script& script,
        uint32_t sequence);
    input(const chain::point::ptr& point, const chain::script::ptr& script,
        uint32_t sequence);

    input(chain::point&& point, chain::script&& script,
        chain::witness&& witness, uint32_t sequence);
    input(const chain::point& point, const chain::script& script,
        const chain::witness& witness, uint32_t sequence);
    input(const chain::point::ptr& point, const chain::script::ptr& script,
        const chain::witness::ptr& witness, uint32_t sequence);

    input(const data_slice& data);
    input(std::istream&& stream);
    input(std::istream& stream);
    input(reader&& source);
    input(reader& source);

    // Operators.
    // ------------------------------------------------------------------------

    input& operator=(input&& other);
    input& operator=(const input& other);

    bool operator==(const input& other) const;
    bool operator!=(const input& other) const;

    // Serialization.
    // ------------------------------------------------------------------------

    data_chunk to_data() const;
    void to_data(std::ostream& stream) const;
    void to_data(writer& sink) const;

    // Properties.
    // ------------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const;
    const chain::point& point() const;
    const chain::script& script() const;
    const chain::witness& witness() const;
    const chain::point::ptr point_ptr() const;
    const chain::script::ptr script_ptr() const;
    const chain::witness::ptr witness_ptr() const;
    uint32_t sequence() const;

    /// Computed properties.
    /// Witness accounts for witness bytes, but are serialized independently.
    size_t serialized_size(bool witness) const;

    // Methods.
    // ------------------------------------------------------------------------

    bool is_final() const;
    bool is_locked(size_t height, uint32_t median_time_past) const;
    bool reserved_hash(hash_digest& out) const;
    size_t signature_operations(bool bip16, bool bip141) const;

    /// Public mutable metadata access, copied but not compared for equality.
    mutable chain::prevout::ptr prevout;

protected:
    // So that witness may be set late in deserialization.
    friend class transaction;

    input(const chain::point::ptr& point, const chain::script::ptr& script,
        const chain::witness::ptr& witness, uint32_t sequence, bool valid,
        const chain::prevout::ptr& prevout);

private:
    static input from_data(reader& source);
    bool embedded_script(chain::script& out) const;

    // Input should be stored as shared (adds 16 bytes).
    // copy: 8 * 64 + 32 + 1 = 69 bytes (vs. 16 when shared).
    // mutable chain::prevout::ptr prevout; (public)
    chain::point::ptr point_;
    chain::script::ptr script_;
    chain::witness::ptr witness_;
    uint32_t sequence_;
    bool valid_;
};

typedef std::vector<input> inputs;
typedef std::vector<input::ptr> input_ptrs;
typedef std::shared_ptr<input_ptrs> inputs_ptr;

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
