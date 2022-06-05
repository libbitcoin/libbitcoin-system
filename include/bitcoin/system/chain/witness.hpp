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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_WITNESS_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_WITNESS_HPP

#include <cstddef>
#include <istream>
#include <memory>
#include <string>
#include <vector>
#include <boost/json.hpp>
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
    typedef std::shared_ptr<const witness> cptr;

    // Constructors.
    // ------------------------------------------------------------------------

    /// Default witness is an invalid empty stack object.
    witness() noexcept;

    /// Defaults.
    witness(witness&&) = default;
    witness(const witness&) = default;
    witness& operator=(witness&&) = default;
    witness& operator=(const witness&) = default;
    ~witness() = default;

    witness(data_stack&& stack) noexcept;
    witness(const data_stack& stack) noexcept;

    witness(chunk_cptrs&& stack) noexcept;
    witness(const chunk_cptrs& stack) noexcept;

    witness(const data_slice& data, bool prefix) noexcept;
    witness(std::istream&& stream, bool prefix) noexcept;
    witness(std::istream& stream, bool prefix) noexcept;
    witness(reader&& source, bool prefix) noexcept;
    witness(reader& source, bool prefix) noexcept;

    witness(const std::string& mnemonic) noexcept;

    // Operators.
    // ------------------------------------------------------------------------

    bool operator==(const witness& other) const noexcept;
    bool operator!=(const witness& other) const noexcept;

    // Serialization.
    // ------------------------------------------------------------------------

    data_chunk to_data(bool prefix) const noexcept;
    void to_data(std::ostream& stream, bool prefix) const noexcept;
    void to_data(writer& sink, bool prefix) const noexcept;

    std::string to_string() const noexcept;

    // Properties.
    // ------------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const noexcept;
    const chunk_cptrs& stack() const noexcept;

    /// Computed properties.
    size_t serialized_size(bool prefix) const noexcept;

    // Utilities.
    // ------------------------------------------------------------------------

    static bool is_push_size(const chunk_cptrs& stack) noexcept;
    static bool is_reserved_pattern(const chunk_cptrs& stack) noexcept;

    bool extract_sigop_script(script& out_script,
        const script& program_script) const noexcept;
    bool extract_script(script& out_script, chunk_cptrs& out_stack,
        const script& program_script) const noexcept;

private:
    static witness from_data(reader& source, bool prefix) noexcept;
    static witness from_string(const std::string& mnemonic) noexcept;
    size_t serialized_size() const noexcept;

    witness(chunk_cptrs&& stack, bool valid) noexcept;
    witness(const chunk_cptrs& stack, bool valid) noexcept;

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
    size_t operator()(const bc::system::chain::witness& value) const noexcept
    {
        // Witness coinbases will collide (null_hash).
        return std::hash<bc::system::data_chunk>{}(value.to_data(true));
    }
};
} // namespace std

#endif
