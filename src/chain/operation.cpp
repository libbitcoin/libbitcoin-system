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
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace chain {

operation operation::factory_from_data(const data_chunk& data)
{
    operation instance;
    instance.from_data(data);
    return instance;
}

operation operation::factory_from_data(std::istream& stream)
{
    operation instance;
    instance.from_data(stream);
    return instance;
}

operation operation::factory_from_data(reader& source)
{
    operation instance;
    instance.from_data(source);
    return instance;
}

bool operation::is_valid() const
{
    return (code == opcode::zero) && data.empty();
}

void operation::reset()
{
    code = opcode::zero;
    data.clear();
}

bool operation::from_data(const data_chunk& data)
{
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    return from_data(istream);
}

bool operation::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool operation::from_data(reader& source)
{
    bool result = true;

    reset();

    uint8_t raw_byte = source.read_byte();
    result = source;

    code = static_cast<opcode>(raw_byte);
    result &= (raw_byte != 0 || code == opcode::zero);

    if (0 < raw_byte && raw_byte <= 75)
        code = opcode::special;

    if (result && operation::must_read_data(code))
    {
        auto read_n_bytes =
            read_opcode_data_byte_count(code, raw_byte, source);

        data = source.read_data(read_n_bytes);

        result = source && (read_n_bytes || data.empty());
    }

    if (!result)
        reset();

    return result;
}

data_chunk operation::to_data() const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == satoshi_size());
    return data;
}

void operation::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void operation::to_data(writer& sink) const
{
    if (code != opcode::raw_data)
    {
        uint8_t raw_byte = static_cast<uint8_t>(code);

        if (code == opcode::special)
            raw_byte = static_cast<uint8_t>(data.size());

        sink.write_byte(raw_byte);

        switch (code)
        {
            case opcode::pushdata1:
                sink.write_byte(static_cast<uint8_t>(data.size()));
                break;

            case opcode::pushdata2:
                sink.write_2_bytes_little_endian(
                    static_cast<uint16_t>(data.size()));
                break;

            case opcode::pushdata4:
                sink.write_4_bytes_little_endian(
                    static_cast<uint32_t>(data.size()));
                break;

            default:
                break;
        }
    }

    sink.write_data(data);
}

uint64_t operation::satoshi_size() const
{
    uint64_t size = 1 + data.size();

    switch (code)
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

    if (data.empty())
        ss << opcode_to_string(code);
    else
        ss << "[ " << encode_base16(data) << " ]";

    return ss.str();
}

uint32_t operation::read_opcode_data_byte_count(opcode code, uint8_t raw_byte,
    reader& source)
{
    switch (code)
    {
        case opcode::special:
            return raw_byte;
        case opcode::pushdata1:
            return source.read_byte();
        case opcode::pushdata2:
            return source.read_2_bytes_little_endian();
        case opcode::pushdata4:
            return source.read_4_bytes_little_endian();
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

uint64_t count_non_push(const operation_stack& operations)
{
    return std::count_if(operations.begin(), operations.end(),
        [](const operation& op) { return !is_push(op.code); });
}

bool is_push_only(const operation_stack& operations)
{
    return count_non_push(operations) == 0;
}

bool is_pubkey_type(const operation_stack& ops)
{
    return ops.size() == 2 &&
        ops[0].code == opcode::special &&
        ops[1].code == opcode::checksig;
}

bool is_pubkey_hash_type(const operation_stack& ops)
{
    return ops.size() == 5 &&
        ops[0].code == opcode::dup &&
        ops[1].code == opcode::hash160 &&
        ops[2].code == opcode::special &&
        ops[2].data.size() == 20 &&
        ops[3].code == opcode::equalverify &&
        ops[4].code == opcode::checksig;
}

bool is_script_hash_type(const operation_stack& ops)
{
    return ops.size() == 3 &&
        ops[0].code == opcode::hash160 &&
        ops[1].code == opcode::special &&
        ops[1].data.size() == 20 &&
        ops[2].code == opcode::equal;
}

bool is_stealth_info_type(const operation_stack& ops)
{
    return ops.size() == 2 &&
        ops[0].code == opcode::return_ &&
        ops[1].code == opcode::special &&
        ops[1].data.size() >= hash_size;
}

bool is_multisig_type(const operation_stack&)
{
    return false;
}

bool is_pubkey_hash_sig_type(const operation_stack& ops)
{
    if (ops.size() != 2 || !is_push_only(ops))
        return false;
    const ec_point& last_data = ops.back().data;
    return verify_public_key_fast(last_data);
}

bool is_script_code_sig_type(const operation_stack& ops)
{
    if (ops.size() < 2 || !is_push_only(ops))
        return false;

    const data_chunk& last_data = ops.back().data;

    if (last_data.empty())
        return false;

    script script_code;

    if (!script_code.from_data(last_data, false, script::parse_mode::strict))
        return false;

    // Minimum size is 4
    // M [SIG]... N checkmultisig
    return script_code.operations.size() >= 4 &&
        count_non_push(script_code.operations) == 1 &&
        script_code.operations.back().code == opcode::checkmultisig;
}

} // end chain
} // end libbitcoin
