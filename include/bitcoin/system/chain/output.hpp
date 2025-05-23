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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_OUTPUT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_OUTPUT_HPP

#include <memory>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API output
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(output);

    typedef std::shared_ptr<const output> cptr;

    /// This is a consensus value required by script::generate_signature_hash.
    static const uint64_t not_found;

    /// Constructors.
    /// -----------------------------------------------------------------------

    /// Default output is an invalid object.
    output() NOEXCEPT;

    output(uint64_t value, chain::script&& script) NOEXCEPT;
    output(uint64_t value, const chain::script& script) NOEXCEPT;
    output(uint64_t value, const chain::script::cptr& script) NOEXCEPT;

    output(stream::in::fast&& stream) NOEXCEPT;
    output(stream::in::fast& stream) NOEXCEPT;
    output(std::istream&& stream) NOEXCEPT;
    output(std::istream& stream) NOEXCEPT;
    output(reader&& source) NOEXCEPT;
    output(reader& source) NOEXCEPT;

    /// Operators.
    /// -----------------------------------------------------------------------

    bool operator==(const output& other) const NOEXCEPT;
    bool operator!=(const output& other) const NOEXCEPT;

    /// Serialization.
    /// -----------------------------------------------------------------------

    data_chunk to_data() const NOEXCEPT;
    void to_data(std::ostream& stream) const NOEXCEPT;
    void to_data(writer& sink) const NOEXCEPT;

    /// Properties.
    /// -----------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const NOEXCEPT;
    uint64_t value() const NOEXCEPT;
    const chain::script& script() const NOEXCEPT;
    const chain::script::cptr& script_ptr() const NOEXCEPT;

    /// Computed properties.
    size_t serialized_size() const NOEXCEPT;

    /// Methods.
    /// -----------------------------------------------------------------------

    hash_digest hash() const NOEXCEPT;
    bool committed_hash(hash_cref& out) const NOEXCEPT;
    size_t signature_operations(bool bip141) const NOEXCEPT;
    bool is_dust(uint64_t minimum_output_value) const NOEXCEPT;

    /// Cache setters/getters, not thread safe.
    /// -----------------------------------------------------------------------

    const hash_digest& get_hash() const NOEXCEPT;

protected:
    output(uint64_t value, const chain::script::cptr& script,
        bool valid) NOEXCEPT;

private:
    void assign_data(reader& source) NOEXCEPT;
    static size_t serialized_size(const chain::script& script,
        uint64_t value) NOEXCEPT;

    // Output should be stored as shared (adds 16 bytes).
    // copy: 3 * 64 + 1 = 25 bytes (vs. 16 when shared).
    uint64_t value_;
    chain::script::cptr script_;

    // Cache.
    bool valid_;
    size_t size_;

    // Signature hash caching (tapscript hash_single).
    mutable std::shared_ptr<const hash_digest> cache_{};
};

typedef std_vector<output> outputs;
typedef std_vector<output::cptr> output_cptrs;
typedef std::shared_ptr<output_cptrs> outputs_ptr;
typedef std::shared_ptr<const output_cptrs> outputs_cptr;

/// Constant reference optimizers.
using output_cptr_cref = std::reference_wrapper<const output::cptr>;

DECLARE_JSON_VALUE_CONVERTORS(output);
DECLARE_JSON_VALUE_CONVERTORS(output::cptr);

} // namespace chain
} // namespace system
} // namespace libbitcoin

namespace std
{
template<>
struct hash<bc::system::chain::output>
{
    size_t operator()(const bc::system::chain::output& value) const NOEXCEPT
    {
        return std::hash<bc::system::data_chunk>{}(value.to_data());
    }
};
} // namespace std

#endif
