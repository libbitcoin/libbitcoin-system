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

// encode

static bool encode_base2048(base2048_chunk& out, const string_list& in,
    wallet::language language)
{
    // Empty if words not contained in dictionary.
    const auto indexes = base2048.index(in, language);
    if (indexes.empty() && !in.empty())
        return false;

    if (std::any_of(indexes.begin(), indexes.end(),
        [](const int32_t& index) { return index < 0; }))
        return false;

    out.resize(indexes.size());
    std::transform(indexes.begin(), indexes.end(), out.begin(),
        [](const int32_t& index) { return static_cast<uint11_t>(index); });

    return true;
}

bool encode_base2048_list(data_chunk& out, const string_list& in,
    wallet::language langauge)
{
    base2048_chunk packed;
    if (!encode_base2048(packed, in, langauge))
        return false;

    out = base2048_unpack(packed);
    return true;
}

bool encode_base2048(data_chunk& out, const std::string& in,
    wallet::language language)
{
    out.clear();

    // An empty string will split into one empty element, so normalize return.
    return in.empty() || encode_base2048_list(out, split(in), language);
}

// decode

static string_list decode_base2048(const base2048_chunk& data,
    wallet::language language)
{
    mnemonic::dictionaries::search indexes(data.size());
    std::transform(data.begin(), data.end(), indexes.begin(),
        [](const uint11_t& index) { return index.convert_to<size_t>(); });

    // Empty if dictionary not found.
    return base2048.at(indexes, language);
}

string_list decode_base2048_list(const data_chunk& data,
    wallet::language language)
{
    return decode_base2048(base2048_pack(data), language);
}

std::string decode_base2048(const data_chunk& data,
    wallet::language language)
{
    // Empty chunk returns empty string, consistent with encoding empty string.
    return join(decode_base2048_list(data, language));
}

// pack/unpack

base2048_chunk base2048_pack(const data_chunk& unpacked)
{
    base2048_chunk packed;
    data_source source(unpacked);
    istream_reader reader(source);
    istream_bit_reader bit_reader(reader);

    while (!bit_reader.is_exhausted())
        packed.push_back(bit_reader.read_bits(11));

    // Remove an element that is only padding, assumes base2048_unpack encoding.
    // The bit writer writes zeros past end as padding.
    // This is a ((n * 8) / 11) operation, (11 - ((n * 8) % 11)) bits are pad.
    // This padding is in addition to that added by unpacking. When unpacked
    // and then packed this will always result in either no pad bits or a full
    // element of zeros that is padding. This should be apparent from the fact
    // that the number of used bits is unchanged. Remainder indicates padding.
    if ((unpacked.size() * 8) % 11 != 0)
    {
        // If pad element is non-zero the unpacking was not base2048_unpack.
        // So we return a failure where the condition is detected.
        packed.resize(packed.back() == 0x00 ? packed.size() - 1u : 0);
    }

    return packed;
}

data_chunk base2048_unpack(const base2048_chunk& packed)
{
    data_chunk unpacked;
    data_sink sink(unpacked);
    ostream_writer writer(sink);
    ostream_bit_writer bit_writer(writer);

    for (const auto& value: packed)
        bit_writer.write_bits(value.convert_to<uint64_t>(), 11);

    bit_writer.flush();
    sink.flush();

    // The bit reader reads zeros past end as padding.
    // This is a ((n * 11) / 8) operation, so (8 - ((n * 11) % 8)) are pad.
    ////const auto padded = (packed.size() * 11) % 8 != 0;
    return unpacked;
}

} // namespace system
} // namespace libbitcoin
