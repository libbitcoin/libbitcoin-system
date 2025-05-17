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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_WITNESS_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_WITNESS_HPP

#include <istream>
#include <memory>
#include <string>
#include <vector>
#include <bitcoin/system/chain/annex.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API witness
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(witness);

    typedef std::shared_ptr<const witness> cptr;

    /// serialized_size(..., true) returns one for an empty witness stack.
    static size_t serialized_size(const chunk_cptrs& stack,
        bool prefix) NOEXCEPT;

    /// Constructors.
    /// -----------------------------------------------------------------------

    /// Default witness is an invalid empty stack object.
    witness() NOEXCEPT;

    witness(data_stack&& stack) NOEXCEPT;
    witness(const data_stack& stack) NOEXCEPT;

    witness(chunk_cptrs&& stack) NOEXCEPT;
    witness(const chunk_cptrs& stack) NOEXCEPT;

    witness(stream::in::fast&& stream, bool prefix) NOEXCEPT;
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

    /// Skip a witness (as if deserialized).
    static void skip(reader& source, bool prefix) NOEXCEPT;

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
    const chain::annex& annex() const NOEXCEPT;

    /// Computed properties.
    /// serialized_size(true) returns one for an empty witness stack.
    size_t serialized_size(bool prefix) const NOEXCEPT;

    /// Patterns.
    /// -----------------------------------------------------------------------

    /// The stack adheres to the annex pattern [bip341].
    static constexpr bool is_annex_pattern(const chunk_cptrs& stack) NOEXCEPT;

    /// Verify the push size of each stack element [bip141].
    static constexpr bool is_push_size(const chunk_cptrs& stack) NOEXCEPT;

    /// The (only) coinbase witness must be (arbitrary) 32-byte value [bip141].
    static constexpr bool is_reserved_pattern(
        const chunk_cptrs& stack) NOEXCEPT;

    /// The stack adheres to the annex pattern [bip341].
    inline bool is_annex_pattern() const NOEXCEPT;

    /// Script extractors.
    /// -----------------------------------------------------------------------

    /// Script for witness version 0 signature operation counting.
    bool extract_sigop_script(script& out_script,
        const script& program_script) const NOEXCEPT;

    /// Script for witness validation.
    code extract_script(script::cptr& out_script, chunk_cptrs_ptr& out_stack,
        const script& program_script) const NOEXCEPT;

protected:
    witness(chunk_cptrs&& stack, bool valid) NOEXCEPT;
    witness(const chunk_cptrs& stack, bool valid) NOEXCEPT;
    witness(const chunk_cptrs& stack, bool valid, size_t size) NOEXCEPT;

private:
    // TODO: move to config serialization wrapper.
    static witness from_string(const std::string& mnemonic) NOEXCEPT;
    static inline bool drop_annex(chunk_cptrs& stack) NOEXCEPT;

    void assign_data(reader& source, bool prefix) NOEXCEPT;

    // Witness should be stored as shared.
    chunk_cptrs stack_;

    // Cache/alias.
    bool valid_;
    size_t size_;
    chain::annex annex_;
};

typedef std_vector<witness> witnesses;

DECLARE_JSON_VALUE_CONVERTORS(witness);
DECLARE_JSON_VALUE_CONVERTORS(witness::cptr);

} // namespace chain
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/chain/annex.ipp>
#include <bitcoin/system/impl/chain/witness_patterns.ipp>

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
