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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_HEADER_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_HEADER_HPP

#include <memory>
#include <optional>
#include <bitcoin/system/chain/chain_state.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API header
{
public:
    /// Cache is copied/moved on copy/assign.
    DEFAULT_COPY_MOVE_DESTRUCT(header);

    typedef std::shared_ptr<const header> cptr;

    static uint256_t proof(uint32_t bits) NOEXCEPT;
    static constexpr size_t serialized_size() NOEXCEPT
    {
        return sizeof(version_)
            + hash_size
            + hash_size
            + sizeof(timestamp_)
            + sizeof(bits_)
            + sizeof(nonce_);
    }

    /// Constructors.
    /// -----------------------------------------------------------------------

    /// Default header is an invalid object.
    header() NOEXCEPT;

    header(uint32_t version, hash_digest&& previous_block_hash,
        hash_digest&& merkle_root, uint32_t timestamp, uint32_t bits,
        uint32_t nonce) NOEXCEPT;
    header(uint32_t version, const hash_digest& previous_block_hash,
        const hash_digest& merkle_root, uint32_t timestamp, uint32_t bits,
        uint32_t nonce) NOEXCEPT;

    header(stream::in::fast&& stream) NOEXCEPT;
    header(stream::in::fast& stream) NOEXCEPT;
    header(std::istream&& stream) NOEXCEPT;
    header(std::istream& stream) NOEXCEPT;
    header(reader&& source) NOEXCEPT;
    header(reader& source) NOEXCEPT;

    /// Operators.
    /// -----------------------------------------------------------------------

    bool operator==(const header& other) const NOEXCEPT;
    bool operator!=(const header& other) const NOEXCEPT;

    /// Serialization.
    /// -----------------------------------------------------------------------

    data_chunk to_data() const NOEXCEPT;
    void to_data(std::ostream& stream) const NOEXCEPT;
    void to_data(writer& sink) const NOEXCEPT;


    /// Properties.
    /// -----------------------------------------------------------------------
    /// Native properties.
    bool is_valid() const NOEXCEPT;
    uint32_t version() const NOEXCEPT;
    const hash_digest& previous_block_hash() const NOEXCEPT;
    const hash_digest& merkle_root() const NOEXCEPT;
    uint32_t timestamp() const NOEXCEPT;
    uint32_t bits() const NOEXCEPT;
    uint32_t nonce() const NOEXCEPT;

    /// Computed properties.
    uint256_t proof() const NOEXCEPT;
    hash_digest hash() const NOEXCEPT;

    /// Cache and metadata.
    /// -----------------------------------------------------------------------
    /// getters/setters, const/mutable, not thread safe.

    /// Set to avoid hash() re-computation.
    void set_hash(const hash_digest& hash) const NOEXCEPT;

    /// Reference used to avoid copy, sets cache if not set.
    const hash_digest& get_hash() const NOEXCEPT;

    /// Set/get chain_state associated with the header, may be nullptr.
    void set_state(const chain_state::cptr& state) const NOEXCEPT;
    const chain_state::cptr& get_state() const NOEXCEPT;

    /// Validation.
    /// -----------------------------------------------------------------------
    /// Checkpoints and previous_block_hash are chain validation (not here).

    code check(uint32_t timestamp_limit_seconds, uint32_t proof_of_work_limit,
        bool scrypt=false) const NOEXCEPT;
    code accept(const context& ctx) const NOEXCEPT;

protected:
    header(uint32_t version, hash_digest&& previous_block_hash,
        hash_digest&& merkle_root, uint32_t timestamp, uint32_t bits,
        uint32_t nonce, bool valid) NOEXCEPT;
    header(uint32_t version, const hash_digest& previous_block_hash,
        const hash_digest& merkle_root, uint32_t timestamp, uint32_t bits,
        uint32_t nonce, bool valid) NOEXCEPT;

    /// Check (context free).
    /// -----------------------------------------------------------------------

    bool is_invalid_proof_of_work(uint32_t proof_of_work_limit,
        bool scrypt=false) const NOEXCEPT;
    bool is_futuristic_timestamp(uint32_t timestamp_limit_seconds) const NOEXCEPT;

    /// Accept (relative to chain_state).
    /// -----------------------------------------------------------------------

    // error::insufficient_block_version
    // error::anachronistic_timestamp
    // error::incorrect_proof_of_work

private:
    void assign_data(reader& source) NOEXCEPT;

    // Header should be stored as shared (adds 16 bytes).
    // copy: 4 * 32 + 2 * 256 + 1 = 81 bytes (vs. 16 when shared).
    uint32_t version_;
    hash_digest previous_block_hash_;
    hash_digest merkle_root_;
    uint32_t timestamp_;
    uint32_t bits_;
    uint32_t nonce_;
    bool valid_;

    // Identity hash caching.
    mutable std::optional<hash_digest> hash_{};

    // Chain state caching.
    mutable chain_state::cptr state_{};
};

typedef std_vector<header> headers;
typedef std_vector<header::cptr> header_cptrs;
typedef std::shared_ptr<header_cptrs> headers_ptr;
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
    size_t operator()(const bc::system::chain::header& value) const NOEXCEPT
    {
        return bc::system::unique_hash(value.hash());
    }
};
} // namespace std

#endif
