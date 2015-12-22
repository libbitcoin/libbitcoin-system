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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_CHAIN_SCRIPT_HPP
#define LIBBITCOIN_CHAIN_SCRIPT_HPP

//#include <string>
#include <istream>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/operation.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace chain {

class BC_API transaction;

/// Signature hash types.
/// Comments from: bitcoin.org/en/developer-guide#standard-transactions
enum signature_hash_algorithm : uint32_t
{
    /// The default, signs all the inputs and outputs, protecting everything
    /// except the signature scripts against modification.
    all = 1,

    /// Signs all of the inputs but none of the outputs, allowing anyone to
    /// change where the satoshis are going unless other signatures using 
    /// other signature hash flags protect the outputs.
    none = 2,

    /// The only output signed is the one corresponding to this input (the
    /// output with the same output index number as this input), ensuring
    /// nobody can change your part of the transaction but allowing other 
    /// signers to change their part of the transaction. The corresponding 
    /// output must exist or the value '1' will be signed, breaking the
    /// security scheme. This input, as well as other inputs, are included
    /// in the signature. The sequence numbers of other inputs are not
    /// included in the signature, and can be updated.
    single = 3,

    /// The above types can be modified with this flag, creating three new
    /// combined types.
    anyone_can_pay = 0x80,

    /// Signs all of the outputs but only this one input, and it also allows
    /// anyone to add or remove other inputs, so anyone can contribute
    /// additional satoshis but they cannot change how many satoshis are
    /// sent nor where they go.
    all_anyone_can_pay = all | anyone_can_pay,

    /// Signs only this one input and allows anyone to add or remove other
    /// inputs or outputs, so anyone who gets a copy of this input can spend
    /// it however they'd like.
    none_anyone_can_pay = none | anyone_can_pay,

    /// Signs this one input and its corresponding output. Allows anyone to
    /// add or remove other inputs.
    single_anyone_can_pay = single | anyone_can_pay
};

// All prefix = true
// All parse_mode = script::parse_mode::strict

class BC_API script
{
public:
    typedef std::vector<script> list;

    enum class parse_mode
    {
        strict,
        raw_data,
        raw_data_fallback
    };

    static script factory_from_data(const data_chunk& data, bool prefix,
        parse_mode mode);
    static script factory_from_data(std::istream& stream, bool prefix,
        parse_mode mode);
    static script factory_from_data(reader& source, bool prefix,
        parse_mode mode);

    static bool verify(const script& input_script,
        const script& output_script, const transaction& parent_tx,
        uint32_t input_index, bool bip16_enabled=true);

    static hash_digest generate_signature_hash(transaction parent_tx,
        uint32_t input_index, const script& script_code, uint32_t hash_type);

    static bool check_endorsement(data_slice endorsement,
        const data_chunk& point, const script& script_code,
        const transaction& parent_tx, uint32_t input_index);
    static bool create_endorsement(endorsement& endorsement,
        const ec_secret& secret, const script& prevout_script,
        const transaction& tx, uint32_t input_index, uint32_t hash_type);

    script_pattern pattern() const;
    bool is_raw_data() const;
    bool from_data(const data_chunk& data, bool prefix, parse_mode mode);
    bool from_data(std::istream& stream, bool prefix, parse_mode mode);
    bool from_data(reader& source, bool prefix, parse_mode mode);
    data_chunk to_data(bool prefix) const;
    void to_data(std::ostream& stream, bool prefix) const;
    void to_data(writer& sink, bool prefix) const;

    bool from_string(const std::string& human_readable);
    std::string to_string() const;
    bool is_valid() const;
    void reset();
    uint64_t satoshi_content_size() const;
    uint64_t serialized_size(bool prefix) const;

    operation::stack operations;

private:
    bool deserialize(const data_chunk& raw_script, parse_mode mode);
    bool parse(const data_chunk& raw_script);
};

} // namspace chain
} // namspace libbitcoin

#endif
