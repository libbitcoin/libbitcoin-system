/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_CHAIN_SCRIPT_HPP
#define LIBBITCOIN_CHAIN_SCRIPT_HPP

#include <cstdint>
#include <istream>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/chain/script/operation.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace chain {

class BC_API transaction;

class BC_API script
{
public:
    enum class parse_mode
    {
        strict,
        raw_data,
        raw_data_fallback
    };

    // Constructors.
    //-------------------------------------------------------------------------

    script();

    script(script&& other);
    script(const script& other);

    script(operation::stack&& operations);
    script(const operation::stack& operations);

    // Operators.
    //-------------------------------------------------------------------------

    script& operator=(script&& other);
    script& operator=(const script& other);

    bool operator==(const script& other) const;
    bool operator!=(const script& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    static script factory_from_data(const data_chunk& data, bool prefix,
        parse_mode mode);
    static script factory_from_data(std::istream& stream, bool prefix,
        parse_mode mode);
    static script factory_from_data(reader& source, bool prefix, parse_mode mode);

    bool from_data(const data_chunk& data, bool prefix, parse_mode mode);
    bool from_data(std::istream& stream, bool prefix, parse_mode mode);
    bool from_data(reader& source, bool prefix, parse_mode mode);

    bool from_string(const std::string& mnemonic);

    bool is_valid() const;

    // Serialization.
    //-------------------------------------------------------------------------

    data_chunk to_data(bool prefix) const;
    void to_data(std::ostream& stream, bool prefix) const;
    void to_data(writer& sink, bool prefix) const;

    std::string to_string(uint32_t flags) const;

    // Properties (size, accessors, cache).
    //-------------------------------------------------------------------------

    uint64_t satoshi_content_size() const;
    uint64_t serialized_size(bool prefix) const;

    // deprecated (unsafe)
    operation::stack& operations();

    const operation::stack& operations() const;

    void set_operations(operation::stack&& value);
    void set_operations(const operation::stack& value);

    // Signing.
    //-------------------------------------------------------------------------

    static hash_digest generate_signature_hash(const transaction& tx,
        uint32_t input_index, const script& script_code, uint8_t sighash_type);

    static bool check_signature(const ec_signature& signature,
        uint8_t sighash_type, const data_chunk& public_key,
        const script& script_code, const transaction& tx,
        uint32_t input_index);

    static bool create_endorsement(endorsement& out, const ec_secret& secret,
        const script& prevout_script, const transaction& tx,
        uint32_t input_index, uint8_t sighash_type);

    // Utilities.
    //-------------------------------------------------------------------------

    static bool is_enabled(uint32_t active_forks, rule_fork flag);

    script_pattern pattern() const;
    size_t sigops(bool serialized_script) const;
    size_t pay_script_hash_sigops(const script& prevout) const;

    // TODO: hide this as protected after changing tests.
    bool is_raw_data() const;

    // Validation.
    //-------------------------------------------------------------------------

    static code verify(const transaction& tx, uint32_t input_index,
        uint32_t flags);

    static code verify(const transaction& tx, uint32_t input_index,
        const script& prevout_script, uint32_t flags);

protected:
    void reset();

private:
    bool emplace(data_chunk&& raw_script);
    bool parse(const data_chunk& raw_script);

    operation::stack operations_;
    bool is_raw_;
    bool valid_;
};

} // namespace chain
} // namespace libbitcoin

#endif
