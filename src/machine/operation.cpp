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
#include <bitcoin/bitcoin/machine/operation.hpp>

#include <string>
#include <bitcoin/bitcoin/formats/base_16.hpp>
#include <bitcoin/bitcoin/machine/opcode.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>

namespace libbitcoin {
namespace machine {

// Deserialization.
//-----------------------------------------------------------------------------

// static
operation operation::factory_from_data(const data_chunk& encoded)
{
    operation instance;
    instance.from_data(encoded);
    return instance;
}

// static
operation operation::factory_from_data(std::istream& stream)
{
    operation instance;
    instance.from_data(stream);
    return instance;
}

// static
operation operation::factory_from_data(reader& source)
{
    operation instance;
    instance.from_data(source);
    return instance;
}

bool operation::from_data(const data_chunk& encoded)
{
    data_source istream(encoded);
    return from_data(istream);
}

bool operation::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

// TODO: optimize for larger data by using a shared byte array.
bool operation::from_data(reader& source)
{
    reset();

    valid_ = true;
    code_ = static_cast<opcode>(source.read_byte());
    const auto size = read_data_size(code_, source);

    // Guard against potential for arbitary memory allocation.
    if (size > max_push_data_size)
        source.invalidate();
    else if (size != 0)
        data_ = source.read_bytes(size);

    if (!source)
        reset();

    return valid_;
}

inline bool is_push_token(const std::string& token)
{
    return token.size() > 1 && token.front() == '[' && token.back() == ']';
}

inline std::string trim_push(const std::string& token)
{
    BITCOIN_ASSERT(token.size() > 1);

    // The removal of spaces in data is a compatability break with our v2.
    return std::string(token.begin() + 1, token.end() - 1);
}

bool operation::from_string(const std::string& mnemonic)
{
    reset();

    if (is_push_token(mnemonic))
    {
        if (decode_base16(data_, trim_push(mnemonic)) && !is_oversized())
        {
            code_ = minimal_opcode_from_data(data_);
            valid_ = true;

            // Revert data if opcode_from_data produced a numeric encoding.
            if (is_numeric(code_))
            {
                data_.clear();
                data_.shrink_to_fit();
            }
        }
    }
    else if (opcode_from_string(code_, mnemonic))
    {
        valid_ = true;
    }

    if (!valid_)
        reset();

    return valid_;
}

bool operation::is_valid() const
{
    return valid_;
}

// protected
void operation::reset()
{
    code_ = invalid_code;
    data_.clear();
    valid_ = false;
}

// Serialization.
//-----------------------------------------------------------------------------

data_chunk operation::to_data() const
{
    data_chunk data;
    data.reserve(serialized_size());
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size());
    return data;
}

void operation::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void operation::to_data(writer& sink) const
{
    static constexpr auto op_75 = static_cast<uint8_t>(opcode::push_size_75);
    const auto size = data_.size();
    const auto code = static_cast<uint8_t>(code_);
    sink.write_byte(code);

    switch (code_)
    {
        case opcode::push_one_size:
            sink.write_byte(static_cast<uint8_t>(size));
            break;
        case opcode::push_two_size:
            sink.write_2_bytes_little_endian(static_cast<uint16_t>(size));
            break;
        case opcode::push_four_size:
            sink.write_4_bytes_little_endian(static_cast<uint32_t>(size));
            break;
        default:
            break;
    }

    sink.write_bytes(data_);
}

std::string operation::to_string(uint32_t active_forks) const
{
    // The removal of spaces in data is a compatibility break with our v2.
    if (!valid_)
        return "<invalid>";

    if (data_.empty())
        return opcode_to_string(code_, active_forks);

    return "[" + encode_base16(data_) + "]";
}

} // namespace machine
} // namespace libbitcoin
