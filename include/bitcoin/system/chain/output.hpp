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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_OUTPUT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_OUTPUT_HPP

#include <istream>
#include <memory>
#include <vector>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API output
{
public:
    typedef std::shared_ptr<const output> cptr;

    /// This is a consensus value required by script::generate_signature_hash.
    static const uint64_t not_found;

    // Constructors.
    // ------------------------------------------------------------------------

    /// Default output is an invalid object.
    output() NOEXCEPT;

    /// Defaults.
    output(output&&) = default;
    output(const output&) = default;
    output& operator=(output&&) = default;
    output& operator=(const output&) = default;
    ~output() = default;

    output(uint64_t value, chain::script&& script) NOEXCEPT;
    output(uint64_t value, const chain::script& script) NOEXCEPT;
    output(uint64_t value, const chain::script::cptr& script) NOEXCEPT;

    output(const data_slice& data) NOEXCEPT;
    output(std::istream&& stream) NOEXCEPT;
    output(std::istream& stream) NOEXCEPT;
    output(reader&& source) NOEXCEPT;
    output(reader& source) NOEXCEPT;

    // Operators.
    // ------------------------------------------------------------------------

    bool operator==(const output& other) const NOEXCEPT;
    bool operator!=(const output& other) const NOEXCEPT;

    // Serialization.
    // ------------------------------------------------------------------------

    data_chunk to_data() const NOEXCEPT;
    void to_data(std::ostream& stream) const NOEXCEPT;
    void to_data(writer& sink) const NOEXCEPT;

    // Properties.
    // ------------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const NOEXCEPT;
    uint64_t value() const NOEXCEPT;
    const chain::script& script() const NOEXCEPT;
    const chain::script::cptr& script_ptr() const NOEXCEPT;

    /// Computed properties.
    size_t serialized_size() const NOEXCEPT;

    // Methods.
    // ------------------------------------------------------------------------

    bool committed_hash(hash_digest& out) const NOEXCEPT;
    size_t signature_operations(bool bip141) const NOEXCEPT;
    bool is_dust(uint64_t minimum_output_value) const NOEXCEPT;

protected:
    output(uint64_t value, const chain::script::cptr& script,
        bool valid) NOEXCEPT;

private:
    static output from_data(reader& source) NOEXCEPT;

    // Output should be stored as shared (adds 16 bytes).
    // copy: 3 * 64 + 1 = 25 bytes (vs. 16 when shared).
    uint64_t value_;
    chain::script::cptr script_;
    bool valid_;
};

typedef std::vector<output> outputs;
typedef std::vector<output::cptr> output_cptrs;
typedef std::shared_ptr<output_cptrs> outputs_ptr;
typedef std::shared_ptr<const output_cptrs> outputs_cptr;

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
