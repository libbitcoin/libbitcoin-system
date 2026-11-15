/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#include "../test.hpp"

BC_AUTO_TEST_SUITE(deflate_tests)

// Deterministic bytes of uniform distribution (incompressible).
static data_chunk noise(size_t size, uint32_t seed)
{
    data_chunk out(size);
    for (auto& byte: out)
    {
        seed = seed * 1103515245_u32 + 12345_u32;
        byte = narrow_cast<uint8_t>(shift_right(seed, 16));
    }

    return out;
}

// Bytes of a few frequent symbols (compressible by codes, not matches).
static data_chunk skewed(size_t size)
{
    const std::string text{ "aaaaaaaaaaaaaaaabbbbbbbbcccdde" };
    const auto alphabet = to_chunk(text);
    auto out = noise(size, 7);
    for (auto& byte: out)
        byte = alphabet.at(byte % alphabet.size());

    return out;
}

static data_chunk joined(const data_chunk& left, const data_chunk& right)
{
    auto out = left;
    out.insert(out.end(), right.begin(), right.end());
    return out;
}

// The stream of data written to a deflater in parts of step bytes.
static data_chunk written(const data_chunk& data, size_t step)
{
    data_chunk out{};
    deflater encoder{};
    for (size_t offset{}; offset < data.size(); offset += step)
    {
        const auto size = std::min(step, data.size() - offset);
        const auto begin = std::next(data.begin(), offset);
        encoder.write(out, data_slice(begin, std::next(begin, size)));
    }

    encoder.flush(out);
    return out;
}

const auto counted = to_chunk(std::string{ "0=,1=a,2=aa,3=aaa,4=aaaa,5=aaaaa,6=aaaaab,7=aaaaabb,8=aaaaabbb,9=aaaaabbbc,10=,11=a,12=aa,13=aaa,14=aaaa,15=aaaaa,16=aaaaab,17=aaaaabb,18=aaaaabbb,19=aaaaabbbc,20=,21=a,22=aa,23=aaa,24=aaaa,25=aaaaa,26=aaaaab,27=aaaaabb,28=aaaaabbb,29=aaaaabbbc,30=,31=a,32=aa,33=aaa,34=aaaa,35=aaaaa,36=aaaaab,37=aaaaabb,38=aaaaabbb,39=aaaaabbbc,40=,41=a,42=aa,43=aaa,44=aaaa,45=aaaaa,46=aaaaab,47=aaaaabb,48=aaaaabbb,49=aaaaabbbc,50=,51=a,52=aa,53=aaa,54=aaaa,55=aaaaa,56=aaaaab,57=aaaaabb,58=aaaaabbb,59=aaaaabbbc," });

const std_vector<data_chunk> round_trips
{
    {},
    { 'a' },
    { 'a', 'b' },
    { 'a', 'b', 'c' },
    to_chunk(std::string{ "abababababababababab" }),
    counted,
    data_chunk(100000, 'a'),
    noise(70000, 42),
    noise(200000, 43),
    skewed(300000),
    joined(joined(noise(40000, 44), data_chunk(40000, 'x')), noise(40000, 45))
};

BC_AUTO_TEST_CASE(deflate__empty__fixed_end_of_block)
{
    BC_REQUIRE_EQUAL(deflate(data_chunk{}), base16_chunk("0300"));
}

BC_AUTO_TEST_CASE(deflate__one_byte__fixed_block)
{
    BC_REQUIRE_EQUAL(deflate(to_chunk(std::string{ "a" })), base16_chunk("4b0400"));
}

BC_AUTO_TEST_CASE(deflate__counted__dynamic_block)
{
    BC_REQUIRE_EQUAL(deflate(counted), base16_chunk("4dd1b71543310805d085284c7228348ce4fd77f8124934f0aa7b08af013826d0981378970972ea04b536e1ed7dc127c2826fa605bf8a7fc0631d0c4d43e7303c4c104bc44b6233b1a3b451b2090d254729504a940aa58b5243a9a3bc513e28fbde8e72a09c2817ca17e586724765a372503154e29a814aa252a85c541a2a1dd58dea41d5507554f347896aa17a516da876f401"));
}

BC_AUTO_TEST_CASE(deflate__round_trips__inflated)
{
    for (const auto& data: round_trips)
    {
        data_chunk out{};
        BC_REQUIRE(inflate(out, deflate(data)));
        BC_REQUIRE_EQUAL(out, data);
    }
}

BC_AUTO_TEST_CASE(deflater__write_parts__deflate)
{
    for (const auto& data: round_trips)
    {
        BC_REQUIRE_EQUAL(written(data, 7777), deflate(data));
    }
}

BC_AUTO_TEST_CASE(deflater__write_bytes__deflate)
{
    BC_REQUIRE_EQUAL(written(counted, 1), deflate(counted));
}

BC_AUTO_TEST_CASE(deflater__flush__reusable)
{
    data_chunk first{};
    data_chunk second{};
    deflater encoder{};
    encoder.write(first, counted);
    encoder.flush(first);
    encoder.write(second, counted);
    encoder.flush(second);
    BC_REQUIRE_EQUAL(first, deflate(counted));
    BC_REQUIRE_EQUAL(second, deflate(counted));
}

BC_AUTO_TEST_CASE(deflate__repeated_byte__compressed)
{
    BC_REQUIRE_LT(deflate(data_chunk(100000, 'a')).size(), 200u);
}

BC_AUTO_TEST_CASE(deflate__incompressible__stored_overhead)
{
    const auto data = noise(200000, 46);
    BC_REQUIRE_LE(deflate(data).size(), data.size() + data.size() / 1000u);
}

BC_AUTO_TEST_SUITE_END()
