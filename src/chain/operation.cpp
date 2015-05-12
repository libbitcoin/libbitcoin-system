/*
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
#include <bitcoin/bitcoin/chain/operation.hpp>
#include <sstream>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/utility/data_stream_host.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>

namespace libbitcoin {
namespace chain {

operation::operation()
{
}

operation::operation(const opcode code, const data_chunk& data)
    : code_(code), data_(data)
{
}

opcode operation::code() const
{
    return code_;
}

void operation::code(const opcode code)
{
    code_ = code;
}

const data_chunk& operation::data() const
{
    return data_;
}

void operation::data(const data_chunk& data)
{
    data_ = data;
}

void operation::reset()
{
    code_ = opcode::zero;
    data_.clear();
}

bool operation::from_data(const data_chunk& data)
{
    data_chunk_stream_host host(data);
    return from_data(host.stream);
}

bool operation::from_data(std::istream& stream)
{
    bool result = true;

    reset();

    uint8_t raw_byte = read_byte(stream);
    result = !stream.fail();

    code_ = static_cast<opcode>(raw_byte);
    result &= (raw_byte != 0 || code_ == opcode::zero);

    if (0 < raw_byte && raw_byte <= 75)
        code_ = opcode::special;

    if (result && operation::must_read_data(code_))
    {
        size_t read_n_bytes =
            read_opcode_data_byte_count(code_, raw_byte, stream);

        data_ = read_data(stream, read_n_bytes);

        result = !stream.fail() && (read_n_bytes || data_.empty());
    }

    if (!result)
        reset();

    return result;
}

data_chunk operation::to_data() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    if (code_ != opcode::raw_data)
    {
        uint8_t raw_byte = static_cast<uint8_t>(code_);

        if (code_ == opcode::special)
            raw_byte = static_cast<uint8_t>(data_.size());

        serial.write_byte(raw_byte);

        switch (code_)
        {
            case opcode::pushdata1:
                serial.write_byte(static_cast<uint8_t>(data_.size()));
                break;

            case opcode::pushdata2:
                serial.write_little_endian(static_cast<uint16_t>(
                    data_.size()));
                break;

            case opcode::pushdata4:
                serial.write_little_endian(static_cast<uint32_t>(
                    data_.size()));
                break;

            default:
                break;
        }
    }

    serial.write_data(data_);


    return result;
}

size_t operation::satoshi_size() const
{
    size_t size = 1 + data_.size();

    switch (code_)
    {
        case opcode::pushdata1:
            size += 1;
            break;

        case opcode::pushdata2:
            size += 2;
            break;

        case opcode::pushdata4:
            size += 4;
            break;

        case opcode::raw_data:
            // remove padding for raw_data
            size -= 1;
            break;

        default:
            break;
    }

    return size;
}

std::string operation::to_string() const
{
    std::ostringstream ss;

    if (data_.empty())
        ss << opcode_to_string(code_);
    else
        ss << "[ " << encode_base16(data_) << " ]";

    return ss.str();
}

size_t operation::read_opcode_data_byte_count(opcode code, uint8_t raw_byte,
    std::istream& stream)
{
    switch (code)
    {
        case opcode::special:
            return raw_byte;
        case opcode::pushdata1:
            return read_byte(stream);
        case opcode::pushdata2:
            return read_2_bytes(stream);
        case opcode::pushdata4:
            return read_4_bytes(stream);
        default:
            break;
    }

    throw std::ios_base::failure("read_opcode_data_byte_count invalid opcode");

    return 0;
}

bool operation::must_read_data(opcode code)
{
    return code == opcode::special
        || code == opcode::pushdata1
        || code == opcode::pushdata2
        || code == opcode::pushdata4;
}

bool is_push(const opcode code)
{
    return code == opcode::zero
        || code == opcode::special
        || code == opcode::pushdata1
        || code == opcode::pushdata2
        || code == opcode::pushdata4
        || code == opcode::negative_1
        || code == opcode::op_1
        || code == opcode::op_2
        || code == opcode::op_3
        || code == opcode::op_4
        || code == opcode::op_5
        || code == opcode::op_6
        || code == opcode::op_7
        || code == opcode::op_8
        || code == opcode::op_9
        || code == opcode::op_10
        || code == opcode::op_11
        || code == opcode::op_12
        || code == opcode::op_13
        || code == opcode::op_14
        || code == opcode::op_15
        || code == opcode::op_16;
}

size_t count_non_push(const operation_stack& operations)
{
    return std::count_if(operations.begin(), operations.end(),
        [](const operation& op) { return !is_push(op.code()); });
}

bool is_push_only(const operation_stack& operations)
{
    return count_non_push(operations) == 0;
}

bool is_pubkey_type(const operation_stack& ops)
{
    return ops.size() == 2 &&
        ops[0].code() == opcode::special &&
        ops[1].code() == opcode::checksig;
}

bool is_pubkey_hash_type(const operation_stack& ops)
{
    return ops.size() == 5 &&
        ops[0].code() == opcode::dup &&
        ops[1].code() == opcode::hash160 &&
        ops[2].code() == opcode::special &&
        ops[2].data().size() == 20 &&
        ops[3].code() == opcode::equalverify &&
        ops[4].code() == opcode::checksig;
}

bool is_script_hash_type(const operation_stack& ops)
{
    return ops.size() == 3 &&
        ops[0].code() == opcode::hash160 &&
        ops[1].code() == opcode::special &&
        ops[1].data().size() == 20 &&
        ops[2].code() == opcode::equal;
}

bool is_stealth_info_type(const operation_stack& ops)
{
    return ops.size() == 2 &&
        ops[0].code() == opcode::return_ &&
        ops[1].code() == opcode::special &&
        ops[1].data().size() >= hash_size;
}

bool is_multisig_type(const operation_stack&)
{
    return false;
}

bool is_pubkey_hash_sig_type(const operation_stack& ops)
{
    if (ops.size() != 2 || !is_push_only(ops))
        return false;
    const ec_point& last_data = ops.back().data();
    return verify_public_key_fast(last_data);
}

bool is_script_code_sig_type(const operation_stack& ops)
{
    if (ops.size() < 2 || !is_push_only(ops))
        return false;

    const data_chunk& last_data = ops.back().data();

    if (last_data.empty())
        return false;

    // note: ignores parse failure, though by allowing fallback, failure
    // cannot meaningfully occur.
    script script_code;
    script_code.from_data(last_data, true);

    if (script_code.is_raw_data())
        return false;

    // Minimum size is 4
    // M [SIG]... N checkmultisig
    return script_code.operations().size() >= 4 &&
        count_non_push(script_code.operations()) == 1 &&
        script_code.operations().back().code() == opcode::checkmultisig;
}

} // end chain
} // end libbitcoin
