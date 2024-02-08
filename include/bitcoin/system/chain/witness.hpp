/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

#include <istream>
#include <memory>
#include <string>
#include <vector>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
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
    DEFAULT_COPY_MOVE_DESTRUCT(witness);

    typedef std::shared_ptr<const witness> cptr;

    /// Constructors.
    /// -----------------------------------------------------------------------

    /// Default witness is an invalid empty stack object.
    witness() NOEXCEPT;

    witness(data_stack&& stack) NOEXCEPT;
    witness(const data_stack& stack) NOEXCEPT;

    witness(chunk_cptrs&& stack) NOEXCEPT;
    witness(const chunk_cptrs& stack) NOEXCEPT;

    witness(const data_slice& data, bool prefix) NOEXCEPT;
    ////witness(stream::in::fast&& stream, bool prefix) NOEXCEPT;
    witness(stream::in::fast& stream, bool prefix) NOEXCEPT;
    witness(std::istream&& stream, bool prefix) NOEXCEPT;
    witness(std::istream& stream, bool prefix) NOEXCEPT;
    witness(reader&& source, bool prefix) NOEXCEPT;
    witness(reader& source, bool prefix) NOEXCEPT;

    // TODO: move to config serialization wrapper.
    witness(const std::string& mnemonic) NOEXCEPT;

    /// Operators.
    /// -----------------------------------------------------------------------

    bool operator==(const witness& other) const NOEXCEPT;
    bool operator!=(const witness& other) const NOEXCEPT;

    /// Serialization.
    /// -----------------------------------------------------------------------

    data_chunk to_data(bool prefix) const NOEXCEPT;
    void to_data(std::ostream& stream, bool prefix) const NOEXCEPT;
    void to_data(writer& sink, bool prefix) const NOEXCEPT;

    // TODO: move to config serialization wrapper.
    std::string to_string() const NOEXCEPT;

    /// Properties.
    /// -----------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const NOEXCEPT;
    const chunk_cptrs& stack() const NOEXCEPT;

    /// Computed properties.
    size_t serialized_size(bool prefix) const NOEXCEPT;

    /// Utilities.
    /// -----------------------------------------------------------------------

    /// Skip a witness (as if deserialized).
    static void skip(reader& source, bool prefix) NOEXCEPT;

    static VCONSTEXPR bool is_push_size(const chunk_cptrs& stack) NOEXCEPT
    {
        return std::all_of(stack.begin(), stack.end(),
            [](const auto& element) NOEXCEPT
            {
                return element->size() <= max_push_data_size;
            });
    }

    /// The (only) coinbase witness must be (arbitrary) 32-byte value (bip141).
    static VCONSTEXPR bool is_reserved_pattern(const chunk_cptrs& stack) NOEXCEPT
    {
        return stack.size() == one && stack.front()->size() == hash_size;
    }

    bool extract_sigop_script(script& out_script,
        const script& program_script) const NOEXCEPT;
    bool extract_script(script::cptr& out_script, chunk_cptrs_ptr& out_stack,
        const script& program_script) const NOEXCEPT;

private:
    // TODO: move to config serialization wrapper.
    static witness from_string(const std::string& mnemonic) NOEXCEPT;
    static witness from_data(reader& source, bool prefix) NOEXCEPT;
    size_t serialized_size() const NOEXCEPT;

    witness(chunk_cptrs&& stack, bool valid) NOEXCEPT;
    witness(const chunk_cptrs& stack, bool valid) NOEXCEPT;

    // Witness should be stored as shared.
    chunk_cptrs stack_;
    bool valid_;
};

typedef std::vector<witness> witnesses;

DECLARE_JSON_VALUE_CONVERTORS(witness);
DECLARE_JSON_VALUE_CONVERTORS(witness::cptr);

} // namespace chain
} // namespace system
} // namespace libbitcoin

namespace std
{
template<>
struct hash<bc::system::chain::witness>
{
    size_t operator()(const bc::system::chain::witness& value) const NOEXCEPT
    {
        // Witness coinbases will collide (null_hash).
        return std::hash<bc::system::data_chunk>{}(value.to_data(true));
    }
};
} // namespace std

#endif
