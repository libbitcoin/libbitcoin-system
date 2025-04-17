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

    static bool is_relative_locktime_applied(uint32_t sequence) NOEXCEPT;
    static bool is_relative_locked(uint32_t sequence, size_t height,
        uint32_t median_time_past, size_t prevout_height,
        uint32_t prevout_median_time_past) NOEXCEPT;

    /// Constructors.
    /// -----------------------------------------------------------------------

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

    input(stream::in::fast&& stream) NOEXCEPT;
    input(stream::in::fast& stream) NOEXCEPT;
    input(std::istream&& stream) NOEXCEPT;
    input(std::istream& stream) NOEXCEPT;
    input(reader&& source) NOEXCEPT;
    input(reader& source) NOEXCEPT;

    /// Operators.
    /// -----------------------------------------------------------------------

    bool operator==(const input& other) const NOEXCEPT;
    bool operator!=(const input& other) const NOEXCEPT;

    /// Serialization.
    /// -----------------------------------------------------------------------

    data_chunk to_data() const NOEXCEPT;
    void to_data(std::ostream& stream) const NOEXCEPT;
    void to_data(writer& sink) const NOEXCEPT;

    /// Properties.
    /// -----------------------------------------------------------------------

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

    /// Methods.
    /// -----------------------------------------------------------------------

    bool is_final() const NOEXCEPT;
    bool is_roller() const NOEXCEPT;
    bool reserved_hash(hash_cref& out) const NOEXCEPT;

    /// Assumes coinbase if prevout not populated (returns only legacy sigops).
    size_t signature_operations(bool bip16, bool bip141) const NOEXCEPT;

    /// Requires metadata.height and median_time_past (otherwise returns true).
    bool is_relative_locked(size_t height,
        uint32_t median_time_past) const NOEXCEPT;

protected:
    input(const chain::point::cptr& point, const chain::script::cptr& script,
        const chain::witness::cptr& witness, uint32_t sequence,
        bool valid) NOEXCEPT;

private:
    typedef struct { size_t nominal; size_t witnessed; } sizes;

    static sizes serialized_size(const chain::script& script) NOEXCEPT;
    static sizes serialized_size(const chain::script& script,
        const chain::witness& witness) NOEXCEPT;
    static const chain::witness& no_witness() NOEXCEPT;
    static const chain::witness::cptr& no_witness_cptr() NOEXCEPT;

    // So that witness may be set late in deserialization.
    friend class transaction;
    size_t nominal_size() const NOEXCEPT;
    size_t witnessed_size() const NOEXCEPT;
    void set_witness(reader& source) NOEXCEPT;

    const chain::witness& get_witness() const NOEXCEPT;
    const chain::witness::cptr& get_witness_cptr() const NOEXCEPT;
    bool extract_sigop_script(chain::script& out,
        const chain::script& prevout_script) const NOEXCEPT;

    // Input should be stored as shared (adds 16 bytes).
    // copy: 8 * 64 + 32 + 1 = 69 bytes (vs. 16 when shared).
    // mutable chain::prevout::cptr prevout; (public)
    chain::point::cptr point_;
    chain::script::cptr script_;
    chain::witness::cptr witness_;
    uint32_t sequence_;

    // Cache.
    bool valid_;
    sizes size_;

public:
    /// Public mutable metadata access, copied but not compared for equality.
    /// Defaults are set so non-population issues usually imply invalidity.
    mutable chain::output::cptr prevout{};
    mutable chain::prevout metadata{ zero, max_uint32, true, true, true };
};

typedef std_vector<input> inputs;
typedef std_vector<input::cptr> input_cptrs;
typedef std::shared_ptr<input_cptrs> inputs_ptr;
typedef std::shared_ptr<const input_cptrs> inputs_cptr;

/// Constant reference optimizers.
struct cref_point { hash_cref hash; uint32_t index; };
using unordered_map_of_cref_point_to_output_cptr_cref =
    std::unordered_map<cref_point, output_cptr_cref>;
BC_API bool operator<(const cref_point& left, const cref_point& right) NOEXCEPT;
BC_API bool operator==(const cref_point& left, const cref_point& right) NOEXCEPT;
BC_API bool operator!=(const cref_point& left, const cref_point& right) NOEXCEPT;

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

template<>
struct hash<bc::system::chain::cref_point>
{
    std::size_t operator()(
        const bc::system::chain::cref_point& value) const NOEXCEPT
    {
        return bc::system::hash_combine(value.index,
            bc::system::unique_hash(value.hash.get()));
    }
};
} // namespace std

#endif
