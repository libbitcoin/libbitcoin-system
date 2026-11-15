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

BC_AUTO_TEST_SUITE(inflate_tests)

// Raw deflate vectors produced by zlib.
const auto fox = to_chunk(std::string{ "The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. " });
const auto fox_fixed = base16_chunk("0bc94855282ccd4cce56482aca2fcf5348cbaf50c82acd2d2856c82f4b2d5228014ae72456552aa4e4a7eb29840c0ec500");
const auto counted = to_chunk(std::string{ "0=,1=a,2=aa,3=aaa,4=aaaa,5=aaaaa,6=aaaaab,7=aaaaabb,8=aaaaabbb,9=aaaaabbbc,10=,11=a,12=aa,13=aaa,14=aaaa,15=aaaaa,16=aaaaab,17=aaaaabb,18=aaaaabbb,19=aaaaabbbc,20=,21=a,22=aa,23=aaa,24=aaaa,25=aaaaa,26=aaaaab,27=aaaaabb,28=aaaaabbb,29=aaaaabbbc,30=,31=a,32=aa,33=aaa,34=aaaa,35=aaaaa,36=aaaaab,37=aaaaabb,38=aaaaabbb,39=aaaaabbbc,40=,41=a,42=aa,43=aaa,44=aaaa,45=aaaaa,46=aaaaab,47=aaaaabb,48=aaaaabbb,49=aaaaabbbc,50=,51=a,52=aa,53=aaa,54=aaaa,55=aaaaa,56=aaaaab,57=aaaaabb,58=aaaaabbb,59=aaaaabbbc," });

// The output of a decoder fed one byte at a time, false if invalid or
// incomplete.
template <typename Decoder>
static bool fed(data_chunk& out, const data_chunk& data)
{
    Decoder decoder{};
    for (const auto byte: data)
    {
        size_t consumed{};
        if (!decoder.write(out, consumed, data_chunk{ byte }))
            return false;
    }

    return decoder.done();
}

// The output of a decoder fed parts of step bytes, each part followed by any
// it did not consume, false if invalid or incomplete.
static bool parted(data_chunk& out, const data_chunk& data, size_t step)
{
    size_t offset{};
    inflater decoder{};
    while (offset < data.size() && !decoder.done())
    {
        size_t consumed{};
        const auto size = std::min(step, data.size() - offset);
        const auto begin = std::next(data.begin(), offset);
        const auto part = data_slice(begin, std::next(begin, size));
        if (!decoder.write(out, consumed, part) || is_zero(consumed))
            return false;

        offset += consumed;
    }

    return decoder.done();
}

// The output of a decoder of limited growth per write, false if invalid or
// incomplete.
static bool paused(data_chunk& out, const data_chunk& data, size_t limit)
{
    size_t offset{};
    inflater decoder{};
    while (!decoder.done())
    {
        size_t consumed{};
        const auto before = out.size();
        const auto begin = std::next(data.begin(), offset);
        if (!decoder.write(out, consumed, data_slice(begin, data.end()), limit))
            return false;

        offset += consumed;
        if (is_zero(consumed) && out.size() == before)
            return false;
    }

    return true;
}

const auto counted_dynamic = base16_chunk("4dd1b71543310805d085283ec9a1d030c8fbef604904d1c0abee213c037018d030035ec5407635d0d30c5ede27bc234cf8649af0adf8037c96b5313c1a3a87e1618258225e129b891da585d299f0a0e428054a8952a174516a28759417ca1b65dfdb510e9413e542f9a2dc50eea82c54362a0795b866a092a8142a1795864a4775a1ba513da83aaaf9a344b550bda836543bfa07");

BC_AUTO_TEST_CASE(inflate__empty_fixed_block__empty)
{
    data_chunk out{ 0x42 };
    BC_REQUIRE(inflate(out, base16_chunk("0300")));
    BC_REQUIRE(out.empty());
}

BC_AUTO_TEST_CASE(inflate__fixed_block__expected)
{
    data_chunk out{};
    BC_REQUIRE(inflate(out, base16_chunk("4b0400")));
    BC_REQUIRE_EQUAL(out, to_chunk(std::string{ "a" }));
}

BC_AUTO_TEST_CASE(inflate__fixed_block_matches__expected)
{
    data_chunk out{};
    BC_REQUIRE(inflate(out, fox_fixed));
    BC_REQUIRE_EQUAL(out, fox);
}

BC_AUTO_TEST_CASE(inflate__stored_block__expected)
{
    data_chunk out{};
    BC_REQUIRE(inflate(out, base16_chunk("010500faff68656c6c6f")));
    BC_REQUIRE_EQUAL(out, to_chunk(std::string{ "hello" }));
}

BC_AUTO_TEST_CASE(inflate__dynamic_block__expected)
{
    data_chunk out{};
    BC_REQUIRE(inflate(out, counted_dynamic));
    BC_REQUIRE_EQUAL(out, counted);
}

BC_AUTO_TEST_CASE(inflater__bytewise_dynamic__expected)
{
    data_chunk out{};
    BC_REQUIRE(fed<inflater>(out, counted_dynamic));
    BC_REQUIRE_EQUAL(out, counted);
}

BC_AUTO_TEST_CASE(inflater__bytewise_fixed__expected)
{
    data_chunk out{};
    BC_REQUIRE(fed<inflater>(out, fox_fixed));
    BC_REQUIRE_EQUAL(out, fox);
}

BC_AUTO_TEST_CASE(inflater__bytewise_stored__expected)
{
    data_chunk out{};
    BC_REQUIRE(fed<inflater>(out, base16_chunk("010500faff68656c6c6f")));
    BC_REQUIRE_EQUAL(out, to_chunk(std::string{ "hello" }));
}

BC_AUTO_TEST_CASE(inflater__bytewise_truncated__incomplete)
{
    data_chunk out{};
    const data_chunk truncated(fox_fixed.begin(), std::prev(fox_fixed.end()));
    BC_REQUIRE(!fed<inflater>(out, truncated));
}

BC_AUTO_TEST_CASE(inflater__parted_dynamic__expected)
{
    data_chunk out{};
    BC_REQUIRE(parted(out, counted_dynamic, 9));
    BC_REQUIRE_EQUAL(out, counted);
}

BC_AUTO_TEST_CASE(inflater__parted_fixed__expected)
{
    data_chunk out{};
    BC_REQUIRE(parted(out, fox_fixed, 3));
    BC_REQUIRE_EQUAL(out, fox);
}

BC_AUTO_TEST_CASE(inflater__paused__expected)
{
    data_chunk out{};
    BC_REQUIRE(paused(out, fox_fixed, 10));
    BC_REQUIRE_EQUAL(out, fox);
}

BC_AUTO_TEST_CASE(inflater__paused_dynamic__expected)
{
    data_chunk out{};
    BC_REQUIRE(paused(out, counted_dynamic, 1));
    BC_REQUIRE_EQUAL(out, counted);
}

BC_AUTO_TEST_CASE(inflater__bytewise_trailing__stream_size)
{
    data_chunk out{};
    size_t consumed{};
    inflater decoder{};
    BC_REQUIRE(decoder.write(out, consumed, base16_chunk("4b0400ffff")));
    BC_REQUIRE(decoder.done());
    BC_REQUIRE_EQUAL(consumed, 3u);
}

BC_AUTO_TEST_CASE(inflate__empty_input__false)
{
    data_chunk out{};
    BC_REQUIRE(!inflate(out, data_chunk{}));
}

BC_AUTO_TEST_CASE(inflate__truncated__false)
{
    data_chunk out{};
    const data_chunk truncated(fox_fixed.begin(), std::prev(fox_fixed.end()));
    BC_REQUIRE(!inflate(out, truncated));
}

BC_AUTO_TEST_CASE(inflate__reserved_block_type__false)
{
    data_chunk out{};
    BC_REQUIRE(!inflate(out, base16_chunk("07")));
}

BC_AUTO_TEST_CASE(inflate__stored_bad_complement__false)
{
    data_chunk out{};
    BC_REQUIRE(!inflate(out, base16_chunk("010500faf068656c6c6f")));
}

BC_AUTO_TEST_CASE(inflate__distance_before_start__false)
{
    data_chunk out{};
    BC_REQUIRE(!inflate(out, base16_chunk("030200")));
}

BC_AUTO_TEST_CASE(inflate__invalid_length_symbol__false)
{
    data_chunk out{};
    BC_REQUIRE(!inflate(out, base16_chunk("1b03")));
}

BC_AUTO_TEST_CASE(inflate__incomplete_literal_codes__false)
{
    data_chunk out{};
    BC_REQUIRE(!inflate(out, base16_chunk("05c001040000008020000000000000000000000000030000000000000000000000000000000000000087")));
}

BC_AUTO_TEST_CASE(inflate__incomplete_length_codes__false)
{
    data_chunk out{};
    BC_REQUIRE(!inflate(out, base16_chunk("05c001040000000020000000000000000000000000010000000000000000000000000000000000000025")));
}

BC_AUTO_TEST_CASE(inflate__incomplete_distance_codes__false)
{
    data_chunk out{};
    BC_REQUIRE(!inflate(out, base16_chunk("0dc0010400000080200000000000000000000000000100000000000000000000000000000000000000bf09")));
}

BC_AUTO_TEST_CASE(inflate__single_distance_code__expected)
{
    data_chunk out{};
    BC_REQUIRE(inflate(out, base16_chunk("0dc00104000000802000000000000000000000000001000000000000000000000000000000000000009f05")));
    BC_REQUIRE_EQUAL(out, to_chunk(std::string{ "aaaa" }));
}

BC_AUTO_TEST_CASE(inflate__empty_distance_codes__expected)
{
    data_chunk out{};
    BC_REQUIRE(inflate(out, base16_chunk("05c001040000000010000000000000000000000000010000000000000000000000000000000000008004")));
    BC_REQUIRE_EQUAL(out, to_chunk(std::string{ "a" }));
}

BC_AUTO_TEST_CASE(inflate__single_literal_code__empty)
{
    data_chunk out{};
    BC_REQUIRE(inflate(out, base16_chunk("05c001040000000010000000000000000000000000000000000000000000000000000000000000008001")));
    BC_REQUIRE(out.empty());
}

BC_AUTO_TEST_CASE(inflate__trailing_data__false)
{
    data_chunk out{};
    BC_REQUIRE(!inflate(out, base16_chunk("4b040000")));
}

BC_AUTO_TEST_CASE(inflate__consumed_trailing_data__stream_size)
{
    data_chunk out{};
    size_t consumed{};
    BC_REQUIRE(inflate(out, consumed, base16_chunk("4b0400ffff")));
    BC_REQUIRE_EQUAL(consumed, 3u);
    BC_REQUIRE_EQUAL(out, to_chunk(std::string{ "a" }));
}

BC_AUTO_TEST_CASE(inflate__limit_equal__true)
{
    data_chunk out{};
    BC_REQUIRE(inflate(out, fox_fixed, fox.size()));
    BC_REQUIRE_EQUAL(out, fox);
}

BC_AUTO_TEST_CASE(inflate__limit_exceeded__false)
{
    data_chunk out{};
    BC_REQUIRE(!inflate(out, fox_fixed, sub1(fox.size())));
}

BC_AUTO_TEST_CASE(inflate__stored_limit_exceeded__false)
{
    data_chunk out{};
    BC_REQUIRE(!inflate(out, base16_chunk("010500faff68656c6c6f"), 4u));
}

BC_AUTO_TEST_SUITE_END()
