#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>
using namespace bc;

BOOST_AUTO_TEST_CASE(encode)
{
    const std::string hex_str = "10a7fd15cb45bda9e90e19a15f";
    BOOST_REQUIRE(hex_str.size() % 2 == 0);
    BOOST_REQUIRE(encode_hex(decode_hex(hex_str)) == hex_str);
    const std::string padded_hex = "  \n\t10a7fd15cb45bda9e90e19a15f\n  \t";
    BOOST_REQUIRE(encode_hex(decode_hex(padded_hex)) == hex_str);
}

