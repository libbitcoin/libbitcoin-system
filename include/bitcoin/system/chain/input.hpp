/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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

#include <istream>
#include <memory>
#include <vector>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/chain/prevout.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/witness.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API input
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(input);

    typedef std::shared_ptr<const input> cptr;

    // Constructors.
    // ------------------------------------------------------------------------

    /// Default input is invalid null point, nullptr prevout, invalid metadata.
    input() NOEXCEPT;

    input(chain::point&& point, chain::script&& script,
        uint32_t sequence) NOEXCEPT;
    input(const chain::point& point, const chain::script& script,
        uint32_t sequence) NOEXCEPT;
    input(const chain::point::cptr& point, const chain::script::cptr& script,
        uint32_t sequence) NOEXCEPT;

    input(chain::point&& point, chain::script&& script,
        chain::witness&& witness, uint32_t sequence) NOEXCEPT;
    input(const chain::point& point, const chain::script& script,
        const chain::witness& witness, uint32_t sequence) NOEXCEPT;
    input(const chain::point::cptr& point, const chain::script::cptr& script,
        const chain::witness::cptr& witness, uint32_t sequence) NOEXCEPT;

    input(const data_slice& data) NOEXCEPT;
    input(std::istream&& stream) NOEXCEPT;
    input(std::istream& stream) NOEXCEPT;
    input(reader&& source) NOEXCEPT;
    input(reader& source) NOEXCEPT;

    // Operators.
    // ------------------------------------------------------------------------

    bool operator==(const input& other) const NOEXCEPT;
    bool operator!=(const input& other) const NOEXCEPT;

    // Serialization.
    // ------------------------------------------------------------------------

    data_chunk to_data() const NOEXCEPT;
    void to_data(std::ostream& stream) const NOEXCEPT;
    void to_data(writer& sink) const NOEXCEPT;

    // Properties.
    // ------------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const NOEXCEPT;
    const chain::point& point() const NOEXCEPT;
    const chain::script& script() const NOEXCEPT;
    const chain::witness& witness() const NOEXCEPT;
    const chain::point::cptr& point_ptr() const NOEXCEPT;
    const chain::script::cptr& script_ptr() const NOEXCEPT;
    const chain::witness::cptr& witness_ptr() const NOEXCEPT;
    uint32_t sequence() const NOEXCEPT;

    /// Computed properties.
    /// Witness accounts for witness bytes, but are serialized independently.
    size_t serialized_size(bool witness) const NOEXCEPT;

    // Methods.
    // ------------------------------------------------------------------------

    bool is_final() const NOEXCEPT;
    bool is_locked(size_t height, uint32_t median_time_past) const NOEXCEPT;
    bool reserved_hash(hash_digest& out) const NOEXCEPT;
    size_t signature_operations(bool bip16, bool bip141) const NOEXCEPT;

protected:
    // So that witness may be set late in deserialization.
    friend class transaction;

    input(const chain::point::cptr& point, const chain::script::cptr& script,
        const chain::witness::cptr& witness, uint32_t sequence,
        bool valid) NOEXCEPT;

private:
    static input from_data(reader& source) NOEXCEPT;
    bool embedded_script(chain::script& out) const NOEXCEPT;

    // Input should be stored as shared (adds 16 bytes).
    // copy: 8 * 64 + 32 + 1 = 69 bytes (vs. 16 when shared).
    // mutable chain::prevout::cptr prevout; (public)
    chain::point::cptr point_;
    chain::script::cptr script_;
    chain::witness::cptr witness_;
    uint32_t sequence_;
    bool valid_;

public:
    /// Public mutable metadata access, copied but not compared for equality.
    mutable chain::output::cptr prevout{};
    mutable chain::prevout metadata{ zero, max_uint32, false, false };
};

typedef std::vector<input> inputs;
typedef std::vector<input::cptr> input_cptrs;
typedef std::shared_ptr<input_cptrs> inputs_ptr;
typedef std::shared_ptr<const input_cptrs> inputs_cptr;

DECLARE_JSON_VALUE_CONVERTORS(input);
DECLARE_JSON_VALUE_CONVERTORS(input::cptr);

} // namespace chain
} // namespace system
} // namespace libbitcoin

namespace std
{
template<>
struct hash<bc::system::chain::input>
{
    size_t operator()(const bc::system::chain::input& value) const NOEXCEPT
    {
        return std::hash<bc::system::data_chunk>{}(value.to_data());
    }
};
} // namespace std

#endif
