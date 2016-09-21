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
#include <bitcoin/bitcoin/chain/output.hpp>

#include <cstdint>
#include <sstream>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace chain {

const uint64_t output::not_found = max_uint64;

output output::factory_from_data(const data_chunk& data)
{
    output instance;
    instance.from_data(data);
    return instance;
}

output output::factory_from_data(std::istream& stream)
{
    output instance;
    instance.from_data(stream);
    return instance;
}

output output::factory_from_data(reader& source)
{
    output instance;
    instance.from_data(source);
    return instance;
}

bool output::is_valid() const
{
    // BUGBUG: an empty script is valid but currently reports otherwise.
    ////BITCOIN_ASSERT_MSG(false, "not implemented");

    return (value != not_found) && script.is_valid();
}

void output::reset()
{
    value = not_found;
    script.reset();
}

bool output::from_data(const data_chunk& data)
{
    data_source istream(data);
    return from_data(istream);
}

bool output::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool output::from_data(reader& source)
{
    reset();

    value = source.read_8_bytes_little_endian();
    auto result = static_cast<bool>(source);

    if (result)
    {
        // Always parse non-coinbase input/output scripts as fallback.
        static const auto mode = script::parse_mode::raw_data_fallback;
        result = script.from_data(source, true, mode);
    }

    if (!result)
        reset();

    return result;
}

data_chunk output::to_data() const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size());
    return data;
}

void output::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void output::to_data(writer& sink) const
{
    sink.write_8_bytes_little_endian(value);
    script.to_data(sink, true);
}

uint64_t output::serialized_size() const
{
    return 8 + script.serialized_size(true);
}

size_t output::signature_operations() const
{
    return script.sigops(false);
}

std::string output::to_string(uint32_t flags) const
{
    std::ostringstream ss;

    ss << "\tvalue = " << value << "\n"
        << "\t" << script.to_string(flags) << "\n";

    return ss.str();
}

} // namespace chain
} // namespace libbitcoin
