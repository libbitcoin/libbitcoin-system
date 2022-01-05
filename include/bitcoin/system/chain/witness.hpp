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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_WITNESS_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_WITNESS_HPP

#include <cstddef>
#include <istream>
#include <memory>
#include <string>
#include <vector>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {
    
class transaction;

class BC_API witness
{
public:
    typedef std::shared_ptr<const witness> ptr;

    // Constructors.
    // ------------------------------------------------------------------------

    /// Default witness is an invalid empty stack object.
    witness();

    witness(witness&& other);
    witness(const witness& other);

    witness(chunk_ptrs&& stack);
    witness(const chunk_ptrs& stack);

    witness(const data_slice& data, bool prefix);
    witness(std::istream&& stream, bool prefix);
    witness(std::istream& stream, bool prefix);
    witness(reader&& source, bool prefix);
    witness(reader& source, bool prefix);

    witness(const std::string& mnemonic);

    // Operators.
    // ------------------------------------------------------------------------

    witness& operator=(witness&& other);
    witness& operator=(const witness& other);

    bool operator==(const witness& other) const;
    bool operator!=(const witness& other) const;

    // Serialization.
    // ------------------------------------------------------------------------

    data_chunk to_data(bool prefix) const;
    void to_data(std::ostream& stream, bool prefix) const;
    void to_data(writer& sink, bool prefix) const;

    std::string to_string() const;

    // Properties.
    // ------------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const;
    const chunk_ptrs& stack() const;

    /// Computed properties.
    size_t serialized_size(bool prefix) const;

    // Utilities.
    // ------------------------------------------------------------------------

    static bool is_push_size(const chunk_ptrs& stack);
    static bool is_reserved_pattern(const chunk_ptrs& stack);

    bool extract_sigop_script(script& out_script,
        const script& program_script) const;
    bool extract_script(script& out_script, chunk_ptrs& out_stack,
        const script& program_script) const;

private:
    static witness from_data(reader& source, bool prefix);
    static witness from_string(const std::string& mnemonic);
    size_t serialized_size() const;

    witness(chunk_ptrs&& stack, bool valid);
    witness(const chunk_ptrs& stack, bool valid);

    // Witness should be stored as shared.
    chunk_ptrs stack_;
    bool valid_;
};

typedef std::vector<witness> witnesses;

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
