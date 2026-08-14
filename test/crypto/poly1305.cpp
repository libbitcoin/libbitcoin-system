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

BOOST_AUTO_TEST_SUITE(poly1305_tests)

// rfc8439 section 2.5.2.
BOOST_AUTO_TEST_CASE(poly1305__flush__rfc8439_example__expected)
{
    const auto key = base16_array(
        "85d6be7857556d337f4452fe42d506a80103808afb0db2fd4abff6af4149f51b");
    const auto message = to_chunk("Cryptographic Forum Research Group");
    const auto expected = base16_array("a8061dc1305136c6c22b8baf0c0127a9");

    poly1305 mac{ key };
    mac.write(message);

    poly1305::tag tag{};
    mac.flush(tag);
    BOOST_REQUIRE_EQUAL(tag, expected);
}

// rfc8439 appendix A.3 (test vector 2, all zero key yields zero tag).
BOOST_AUTO_TEST_CASE(poly1305__flush__zero_key__zero_tag)
{
    const poly1305::secret key{};
    const data_chunk message(64, 0xff_u8);
    const poly1305::tag expected{};

    poly1305 mac{ key };
    mac.write(message);

    poly1305::tag tag{};
    mac.flush(tag);
    BOOST_REQUIRE_EQUAL(tag, expected);
}

// rfc8439 appendix A.3 test vector 2 (r zero, tag is s).
BOOST_AUTO_TEST_CASE(poly1305__flush__vector_2__expected)
{
    const auto key = base16_array(
        "0000000000000000000000000000000036e5f6b5c5e06070f0efca96227a863e");
    const auto message = to_chunk(
        "Any submission to the IETF intended by the Contributor for publi"
        "cation as all or part of an IETF Internet-Draft or RFC and any s"
        "tatement made within the context of an IETF activity is consider"
        "ed an \"IETF Contribution\". Such statements include oral statem"
        "ents in IETF sessions, as well as written and electronic communi"
        "cations made at any time or place, which are addressed to");
    const auto expected = base16_array("36e5f6b5c5e06070f0efca96227a863e");

    poly1305 mac{ key };
    mac.write(message);

    poly1305::tag tag{};
    mac.flush(tag);
    BOOST_REQUIRE_EQUAL(tag, expected);
}

// Split writes accumulate identically to a single write.
BOOST_AUTO_TEST_CASE(poly1305__write__split__same_tag)
{
    const auto key = base16_array(
        "85d6be7857556d337f4452fe42d506a80103808afb0db2fd4abff6af4149f51b");
    const auto message = to_chunk("Cryptographic Forum Research Group");

    poly1305 whole{ key };
    whole.write(message);
    poly1305::tag expected{};
    whole.flush(expected);

    poly1305 split{ key };
    split.write({ message.data(), 5 });
    split.write({ std::next(message.data(), 5), 20 });
    split.write({ std::next(message.data(), 25), message.size() - 25 });
    poly1305::tag tag{};
    split.flush(tag);
    BOOST_REQUIRE_EQUAL(tag, expected);
}

BOOST_AUTO_TEST_SUITE_END()
