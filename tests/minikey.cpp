#include <bitcoin/bitcoin.hpp>
using namespace libbitcoin;

void test_wallet_import_format()
{
    secret_parameter secret{
        0x0C, 0x28, 0xFC, 0xA3, 0x86, 0xC7, 0xA2, 0x27,
        0x60, 0x0B, 0x2F, 0xE5, 0x0B, 0x7C, 0xAE, 0x11,
        0xEC, 0x86, 0xD3, 0xBF, 0x1F, 0xBE, 0x47, 0x1B,
        0xE8, 0x98, 0x27, 0xE1, 0x9D, 0x72, 0xAA, 0x1D};
    std::string wif = "5HueCGU8rMjxEXxiPuD5BDku4MkFqeZyd4dZ1jvhTVqvbTLvyTJ";
    BITCOIN_ASSERT(secret_to_wif(secret) == wif);
    BITCOIN_ASSERT(wif_to_secret(wif) == secret);
}

void test_minikey()
{
    secret_parameter secret =
        minikey_to_secret("S6c56bnXQiBjk9mqSYE7ykVQ7NzrRy");
    BITCOIN_ASSERT(secret_to_wif(secret) ==
        "5JPy8Zg7z4P7RSLsiqcqyeAF1935zjNUdMxcDeVrtU1oarrgnB7");
}

void test_minikey2()
{
    secret_parameter secret =
        minikey_to_secret("S6c56bnXQiBjk9mqSYE7ykVQ7NzrRy");
    elliptic_curve_key ec;
    ec.set_secret(secret);
    payment_address addr;
    set_public_key(addr, ec.public_key());
    BITCOIN_ASSERT(addr.encoded() == "1CciesT23BNionJeXrbxmjc7ywfiyM4oLW");
    BITCOIN_ASSERT(secret_to_wif(ec.secret()) == 
        "5JPy8Zg7z4P7RSLsiqcqyeAF1935zjNUdMxcDeVrtU1oarrgnB7");
}

int main()
{
    test_wallet_import_format();
    test_minikey();
    test_minikey2();
    return 0;
}

