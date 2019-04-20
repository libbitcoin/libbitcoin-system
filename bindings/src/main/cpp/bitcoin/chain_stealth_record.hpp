/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN__CHAIN_STEALTH_RECORD_HPP
#define LIBBITCOIN__CHAIN_STEALTH_RECORD_HPP

//#include <cstddef>
//#include <cstdint>
//#include <istream>
//#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/stealth_record.hpp>
//#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/math_ec_compressed.hpp>
#include <bitcoin/math_hash_digest.hpp>
#include <bitcoin/math_short_hash.hpp>
//#include <bitcoin/bitcoin/utility/binary.hpp>
#include <bitcoin/utility_data_chunk.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
//namespace chain {
namespace api {
    
/// This class models blockchain storage of a stealth record.
/// The store excludes assumed sign byte (0x02) of the ephemeral public key.
class BC_API chain_stealth_record
{
public:
//    typedef std::vector<stealth_record> list;
//    typedef hash_digest unsigned_ephemeral_key;

    // Constructors.
    //-------------------------------------------------------------------------

    chain_stealth_record();

//    stealth_record(chain::stealth_record&& other);
    chain_stealth_record(const chain::stealth_record& other);

//    stealth_record(size_t height, uint32_t prefix,
//        hash_digest&& ephemeral_public_key, short_hash&& public_key_hash,
//        hash_digest&& transaction_hash);
    chain_stealth_record(size_t height, uint32_t prefix,
        const math_hash_digest& ephemeral_public_key,
        const math_short_hash& public_key_hash, const math_hash_digest& transaction_hash);

//    stealth_record(size_t height, uint32_t prefix,
//        const math_ec_compressed& ephemeral_public_key,
//        short_hash&& public_key_hash, math_hash_digest&& transaction_hash);
    chain_stealth_record(size_t height, uint32_t prefix,
        const math_ec_compressed& ephemeral_public_key,
        const math_short_hash& public_key_hash, const math_hash_digest& transaction_hash);

    // Operators.
    //-------------------------------------------------------------------------

//    stealth_record& operator=(stealth_record&& other);
    chain_stealth_record& assign(const chain_stealth_record& other);
//    stealth_record& operator=(const stealth_record& other);

    bool eq(const chain_stealth_record& other) const;
//    bool operator==(const stealth_record& other) const;
//    bool operator!=(const stealth_record& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    static chain_stealth_record factory(const utility_data_chunk& data, bool wire=true);
//    static stealth_record factory(std::istream& stream, bool wire=true);
//    static stealth_record factory(reader& source, bool wire=true);

    bool from_data(const utility_data_chunk& data, bool wire=true);
//    bool from_data(std::istream& stream, bool wire=true);
//    bool from_data(reader& source, bool wire=true);

    /// Skip read of records below this height or not matching (non-wire only).
//    bool from_data(reader& source, size_t start_height, const binary& filter);

    bool is_valid() const;

    // Serialization.
    //-------------------------------------------------------------------------

    utility_data_chunk to_data(bool wire=true) const;
//    void to_data(std::ostream& stream, bool wire=true) const;
//    void to_data(writer& sink, bool wire=true) const;

    // Properties (size, accessors).
    //-------------------------------------------------------------------------

    static size_t satoshi_fixed_size(bool wire=true);
    size_t serialized_size(bool wire=true) const;

    size_t height() const;
    uint32_t prefix() const;
    math_ec_compressed ephemeral_public_key() const;
    const math_hash_digest& unsigned_ephemeral_public_key() const;
    const math_short_hash& public_key_hash() const;
    const math_hash_digest& transaction_hash() const;

    chain::stealth_record* getValue() {
        return value_;
    }

    void setValue(chain::stealth_record* value) {
        value_ = value;
    }
//protected:
//    void reset();

private:
    chain::stealth_record* value_;
//    uint32_t height_;
//    uint32_t prefix_;
//    hash_digest unsigned_ephemeral_;
//    short_hash public_key_hash_;
//    hash_digest transaction_hash_;
};

} // namespace chain
} // namespace libbitcoin

#endif
