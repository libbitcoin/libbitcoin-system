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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_HEADER_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_HEADER_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <vector>
#include <boost/json.hpp>
#include <bitcoin/system/chain/chain_state.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {

class settings;

namespace chain {

class BC_API header
{
public:
    typedef std::shared_ptr<const header> cptr;

    static constexpr size_t serialized_size() noexcept
    {
        return sizeof(version_)
            + hash_size
            + hash_size
            + sizeof(timestamp_)
            + sizeof(bits_)
            + sizeof(nonce_);
    }

    // Constructors.
    // ------------------------------------------------------------------------

    /// Default header is an invalid object.
    header() noexcept;

    /// Defaults.
    header(header&&) = default;
    header(const header&) = default;
    header& operator=(header&&) = default;
    header& operator=(const header&) = default;
    ~header() = default;

    header(uint32_t version, hash_digest&& previous_block_hash,
        hash_digest&& merkle_root, uint32_t timestamp, uint32_t bits,
        uint32_t nonce) noexcept;
    header(uint32_t version, const hash_digest& previous_block_hash,
        const hash_digest& merkle_root, uint32_t timestamp, uint32_t bits,
        uint32_t nonce) noexcept;

    header(const data_slice& data) noexcept;
    header(std::istream&& stream) noexcept;
    header(std::istream& stream) noexcept;
    header(reader&& source) noexcept;
    header(reader& source) noexcept;

    // Operators.
    // ------------------------------------------------------------------------

    bool operator==(const header& other) const noexcept;
    bool operator!=(const header& other) const noexcept;

    // Serialization.
    // ------------------------------------------------------------------------

    data_chunk to_data() const noexcept;
    void to_data(std::ostream& stream) const noexcept;
    void to_data(writer& sink) const noexcept;


    // Properties.
    // ------------------------------------------------------------------------
    /// Native properties.
    bool is_valid() const noexcept;
    uint32_t version() const noexcept;
    const hash_digest& previous_block_hash() const noexcept;
    const hash_digest& merkle_root() const noexcept;
    uint32_t timestamp() const noexcept;
    uint32_t bits() const noexcept;
    uint32_t nonce() const noexcept;

    /// Computed properties.
    hash_digest hash() const noexcept;
    uint256_t difficulty() const noexcept;

    // Validation.
    // ------------------------------------------------------------------------

    code check(uint32_t timestamp_limit_seconds, uint32_t proof_of_work_limit,
        bool scrypt=false) const noexcept;

    code accept(const chain_state& state) const noexcept;

protected:
    header(uint32_t version, hash_digest&& previous_block_hash,
        hash_digest&& merkle_root, uint32_t timestamp, uint32_t bits,
        uint32_t nonce, bool valid) noexcept;
    header(uint32_t version, const hash_digest& previous_block_hash,
        const hash_digest& merkle_root, uint32_t timestamp, uint32_t bits,
        uint32_t nonce, bool valid) noexcept;

    // Check (context free).
    // ------------------------------------------------------------------------

    bool is_invalid_proof_of_work(uint32_t proof_of_work_limit,
        bool scrypt=false) const noexcept;
    bool is_invalid_timestamp(uint32_t timestamp_limit_seconds) const noexcept;

    // Accept (relative to chain_state).
    // ------------------------------------------------------------------------

    // error::checkpoints_failed
    // error::invalid_block_version
    // error::timestamp_too_early
    // error::incorrect_proof_of_work

private:
    static header from_data(reader& source) noexcept;
    static uint256_t difficulty(uint32_t bits) noexcept;

    // Header should be stored as shared (adds 16 bytes).
    // copy: 4 * 32 + 2 * 256 + 1 = 81 bytes (vs. 16 when shared).
    uint32_t version_;
    hash_digest previous_block_hash_;
    hash_digest merkle_root_;
    uint32_t timestamp_;
    uint32_t bits_;
    uint32_t nonce_;
    bool valid_;
};

typedef std::vector<header> headers;
typedef std::vector<header::cptr> header_cptrs;
typedef std::shared_ptr<const header_cptrs> headers_cptr;

DECLARE_JSON_VALUE_CONVERTORS(header);
DECLARE_JSON_VALUE_CONVERTORS(header::cptr);

} // namespace chain
} // namespace system
} // namespace libbitcoin

namespace std
{
template<>
struct hash<bc::system::chain::header>
{
    size_t operator()(const bc::system::chain::header& value) const noexcept
    {
        return std::hash<bc::system::hash_digest>{}(value.hash());
    }
};
} // namespace std

#endif
