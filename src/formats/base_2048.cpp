/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/formats/base_2048.hpp>

#include <algorithm>
#include <cstdint>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/iostream.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/wallet/mnemonics/language.hpp>
#include <bitcoin/system/wallet/mnemonics/mnemonic.hpp>

namespace libbitcoin {
namespace system {

using namespace wallet;

// Use the BIP39 dictionary set.
// This allows the encoding to map to BIP39 in any language.
static const mnemonic::dictionaries base2048
{
    {
        mnemonic::dictionary{ language::en, mnemonic::en },
        mnemonic::dictionary{ language::es, mnemonic::es },
        mnemonic::dictionary{ language::it, mnemonic::it },
        mnemonic::dictionary{ language::fr, mnemonic::fr },
        mnemonic::dictionary{ language::cs, mnemonic::cs },
        mnemonic::dictionary{ language::pt, mnemonic::pt },
        mnemonic::dictionary{ language::ja, mnemonic::ja },
        mnemonic::dictionary{ language::ko, mnemonic::ko },
        mnemonic::dictionary{ language::zh_Hans, mnemonic::zh_Hans },
        mnemonic::dictionary{ language::zh_Hant, mnemonic::zh_Hant }
    }
};

static base2048_chunk base2048_expand(const data_chunk& data);
static data_chunk base2048_compact(const base2048_chunk& expanded);

// encode

static string_list encode_base2048(const base2048_chunk& data,
    wallet::language language)
{
    mnemonic::dictionaries::search indexes(data.size());
    std::transform(data.begin(), data.end(), indexes.begin(), 
    [](const uint11_t& index)
    {
        return index.convert_to<size_t>();
    });

    // Empty if dictionary not found.
    return base2048.at(indexes, language);
}

string_list encode_base2048_list(const data_chunk& data,
    wallet::language language)
{
    return encode_base2048(base2048_expand(data), language);
}

std::string encode_base2048(const data_chunk& data,
    wallet::language language)
{
    return join(encode_base2048_list(data, language));
}

// decode

static bool decode_base2048(base2048_chunk& out, const string_list& in,
    wallet::language language)
{
    out.clear();

    // Empty if words not contained in dictionary.
    const auto indexes = base2048.index(in, language);
    if (indexes.empty())
        return false;

    if (std::any_of(indexes.begin(), indexes.end(),
        [](const int32_t& index)
        {
            return index < 0;
        }))
        return false;

    out.resize(indexes.size());
    std::transform(indexes.begin(), indexes.end(), out.begin(),
        [](const int32_t& index)
        {
            return static_cast<uint11_t>(index);
        });

    return true;
}

bool decode_base2048_list(data_chunk& out, const string_list& in,
    wallet::language langauge)
{
    base2048_chunk expanded;
    if (!decode_base2048(expanded, in, langauge))
        return false;

    out = base2048_compact(expanded);
    return true;
}

bool decode_base2048(data_chunk& out, const std::string& in,
    wallet::language language)
{
    return decode_base2048_list(out, split(in), language);
}

// compact/expand

static base2048_chunk base2048_expand(const data_chunk& data)
{
    base2048_chunk out;
    data_source source(data);
    istream_reader reader(source);
    istream_bit_reader bit_reader(reader);

    while (!bit_reader.is_exhausted())
        out.push_back(bit_reader.read_bits(11));

    // TODO: look at pad calc.

    // The bit reader reads zeros past end as padding.
    // This is a ((n * 8) / 11) operation, (11 - ((n * 8) % 11)) bits are pad.
    ////const auto padded = (data.size() * 8) % 11 != 0;

    return out;
}

static data_chunk base2048_compact(const base2048_chunk& data)
{
    data_chunk out;
    data_sink sink(out);
    ostream_writer writer(sink);
    ostream_bit_writer bit_writer(writer);

    for (const auto& value: data)
        bit_writer.write_bits(value.convert_to<uint32_t>(), 11);

    bit_writer.flush();
    sink.flush();

    // TODO: look at pad calc.
    // TODO: remove two bytes if padding > 8 bits.

    // The bit writer writes zeros past end as padding.
    // This is a ((n * 11) / 8) operation, so (8 - ((n * 11) % 8)) are pad.
    // Remove a byte that is only padding, assumes base2048_expand encoding.
    if ((data.size() * 11) % 8 != 0)
    {
        // If pad byte is non-zero the expansion was not base2048_expand, but
        // it remains possible that zero but non-pad data may be passed.
        // So we return a failure where the condition is detected.
        out.resize(out.back() == 0x00 ? out.size() - 1u : 0);
    }

    return out;
}

} // namespace system
} // namespace libbitcoin
