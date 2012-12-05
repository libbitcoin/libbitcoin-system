#include <bitcoin/bitcoin.hpp>
using namespace bc;

constexpr size_t sha512_length = SHA512_DIGEST_LENGTH;
constexpr size_t sha512_block_size = 128;

typedef std::array<uint8_t, sha512_length> long_hash;

template <typename HashType, size_t block_size>
HashType generate_hmac(const data_chunk& key, const data_chunk& data,
    std::function<HashType (const data_chunk&)> hash_func)
{
    typedef typename HashType::value_type hash_char;
    std::array<hash_char, block_size> fixed_key;
    // Zero out
    std::fill(fixed_key.begin(), fixed_key.end(), 0);
    // Now copy key or hash into zeroed out buffer
    if (key.size() <= block_size)
    {
        std::copy(key.begin(), key.end(), fixed_key.begin());
    }
    else
    {
        HashType fixed_key_digest = hash_func(key);
        BITCOIN_ASSERT(fixed_key.size() >= fixed_key_digest.size());
        std::copy(fixed_key_digest.begin(), fixed_key_digest.end(),
            fixed_key.begin());
    }
    // hash(o_key_pad + hash(i_key_pad + data))
    // Work on inner section first
    data_chunk inner_data(fixed_key.size() + data.size());
    // xor each digit of key...
    std::transform(fixed_key.begin(), fixed_key.end(), inner_data.begin(),
        [](hash_char digit) { return digit ^ 0x36; });
    // ... and append the data.
    std::copy(data.begin(), data.end(),
        inner_data.begin() + fixed_key.size());
    HashType inner_hash = hash_func(inner_data);
    // Work on outer section
    data_chunk outer_data(fixed_key.size() + inner_hash.size());
    // xor each digit of key...
    std::transform(fixed_key.begin(), fixed_key.end(), outer_data.begin(),
        [](hash_char digit) { return digit ^ 0x5c; });
    // ... and combine with inner_hash to get outer_data.
    std::copy(inner_hash.begin(), inner_hash.end(),
        outer_data.begin() + fixed_key.size());
    return hash_func(outer_data);
}

long_hash single_sha512_hash(const data_chunk& chunk)
{
    long_hash digest;
    SHA512_CTX ctx;
    SHA512_Init(&ctx);
    SHA512_Update(&ctx, chunk.data(), chunk.size());
    SHA512_Final(digest.data(), &ctx);
    return digest;
}

long_hash generate_hmac_sha512(const data_chunk& key, const data_chunk& data)
{
    return generate_hmac<long_hash, sha512_block_size>(
        key, data, single_sha512_hash);
}

typedef struct {
    const char *pszKey;
    const char *pszData;
    const char *pszMAC;
} testvec_t;

// test cases 1, 2, 3, 4, 6 and 7 of RFC 4231
static const testvec_t vtest[] = {
    {
        "0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b"
        "0b0b0b0b",
        "4869205468657265",
        "87aa7cdea5ef619d4ff0b4241a1d6cb0"
        "2379f4e2ce4ec2787ad0b30545e17cde"
        "daa833b7d6b8a702038b274eaea3f4e4"
        "be9d914eeb61f1702e696c203a126854"
    },
    {
        "4a656665",
        "7768617420646f2079612077616e7420"
        "666f72206e6f7468696e673f",
        "164b7a7bfcf819e2e395fbe73b56e0a3"
        "87bd64222e831fd610270cd7ea250554"
        "9758bf75c05a994a6d034f65f8f0e6fd"
        "caeab1a34d4a6b4b636e070a38bce737"
    },
    {
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaa",
        "dddddddddddddddddddddddddddddddd"
        "dddddddddddddddddddddddddddddddd"
        "dddddddddddddddddddddddddddddddd"
        "dddd",
        "fa73b0089d56a284efb0f0756c890be9"
        "b1b5dbdd8ee81a3655f83e33b2279d39"
        "bf3e848279a722c806b485a47e67c807"
        "b946a337bee8942674278859e13292fb"
    },
    {
        "0102030405060708090a0b0c0d0e0f10"
        "111213141516171819",
        "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
        "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
        "cdcdcdcdcdcdcdcdcdcdcdcdcdcdcdcd"
        "cdcd",
        "b0ba465637458c6990e5a8c5f61d4af7"
        "e576d97ff94b872de76f8050361ee3db"
        "a91ca5c11aa25eb4d679275cc5788063"
        "a5f19741120c4f2de2adebeb10a298dd"
    },
    {
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaa",
        "54657374205573696e67204c61726765"
        "72205468616e20426c6f636b2d53697a"
        "65204b6579202d2048617368204b6579"
        "204669727374",
        "80b24263c7c1a3ebb71493c1dd7be8b4"
        "9b46d1f41b4aeec1121b013783f8f352"
        "6b56d037e05f2598bd0fd2215d6a1e52"
        "95e64f73f63f0aec8b915a985d786598"
    },
    {
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaa",
        "54686973206973206120746573742075"
        "73696e672061206c6172676572207468"
        "616e20626c6f636b2d73697a65206b65"
        "7920616e642061206c61726765722074"
        "68616e20626c6f636b2d73697a652064"
        "6174612e20546865206b6579206e6565"
        "647320746f2062652068617368656420"
        "6265666f7265206265696e6720757365"
        "642062792074686520484d414320616c"
        "676f726974686d2e",
        "e37b6a775dc87dbaa4dfa9f96e5e3ffd"
        "debd71f8867289865df5a32d20cdc944"
        "b6022cac3c4982b10d5eeb55c3e4de15"
        "134676fb6de0446065c97440fa8c6a58"
    }
};

data_chunk string_data(const std::string& str)
{
    data_chunk result(str.size());
    std::copy(str.begin(), str.end(), result.begin());
    return result;
}

int main()
{
    BITCOIN_ASSERT(single_sha512_hash({0x12, 0x13}) ==
        hash_from_pretty<long_hash>("1b09a411dc31ee2284e642a05fd2657ff4a117916e8c16e8fd4b5af5778f91eb4283b01667e388c5aa757172496d41cc209faf6697bc0b371e414175f205dfec"));
    data_chunk key, data;
    long_hash brown_fox = generate_hmac_sha512(string_data("key"),
        string_data("The quick brown fox jumps over the lazy dog"));
    BITCOIN_ASSERT(brown_fox == hash_from_pretty<long_hash>("b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a"));

    for (int n=0; n<sizeof(vtest)/sizeof(vtest[0]); n++)
    {
        data_chunk key = bytes_from_pretty(vtest[n].pszKey);
        data_chunk data = bytes_from_pretty(vtest[n].pszData);
        long_hash hmac = hash_from_pretty<long_hash>(vtest[n].pszMAC);

        long_hash temp = generate_hmac_sha512(key, data);
        BITCOIN_ASSERT(temp == hmac);
    }
    return 0;
}

