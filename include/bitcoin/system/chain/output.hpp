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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_OUTPUT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_OUTPUT_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <string>
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
    typedef std::shared_ptr<output> ptr;

    /// This is a consensus value required by script::generate_signature_hash.
    static const uint64_t not_found;

    // Constructors.
    // ------------------------------------------------------------------------

    /// Default output is an invalid object (input.prevout relies on this).
    output();

    output(output&& other);
    output(const output& other);

    output(uint64_t value, chain::script&& script);
    output(uint64_t value, const chain::script& script);

    output(const data_slice& data);
    output(std::istream& stream);
    output(reader& source);

    // Operators.
    // ------------------------------------------------------------------------

    output& operator=(output&& other);
    output& operator=(const output& other);

    bool operator==(const output& other) const;
    bool operator!=(const output& other) const;

    // Deserialization.
    // ------------------------------------------------------------------------

    bool from_data(const data_slice& data);
    bool from_data(std::istream& stream);
    bool from_data(reader& source);

    /// Deserialization result.
    bool is_valid() const;

    // Serialization.
    // ------------------------------------------------------------------------

    data_chunk to_data() const;
    void to_data(std::ostream& stream) const;
    void to_data(writer& sink) const;

    // Properties.
    // ------------------------------------------------------------------------

    /// Native properties.
    uint64_t value() const;
    const chain::script& script() const;

    /// Computed properties.
    size_t serialized_size() const;

    // Methods.
    // ------------------------------------------------------------------------

    bool committed_hash(hash_digest& out) const;
    size_t signature_operations(bool bip141) const;
    bool is_dust(uint64_t minimum_output_value) const;

protected:
    output(uint64_t value, chain::script&& script, bool valid);
    output(uint64_t value, const chain::script& script, bool valid);

    void reset();

private:
    uint64_t value_;
    chain::script script_;
    bool valid_;
};

typedef std::vector<output> outputs;

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
