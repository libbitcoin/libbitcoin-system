/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(sha_tests)

template <size_t Strength, size_t Rounds,
    bool_if<Strength == 160 || Strength == 256 || Strength == 512> = true,
    bool_if<Rounds == 64 || Rounds == 80> = true>
struct k
{
    static constexpr size_t strength = Strength;
    static constexpr size_t rounds = Rounds;
    static constexpr size_t size = (Strength == 160 ? 256 : Strength);
    using constants_t = std_array<iif<Strength == 512, uint64_t, uint32_t>,
        Rounds>;
};

struct k160
  : public k<160, 80>
{
    using base = k<160, 80>;

    /// K values (constants).
    static constexpr base::constants_t get
    {
        // rounds 0..19
        0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,
        0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,
        0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,
        0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,
        0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,

        // rounds 20..39
        0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,
        0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,
        0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,
        0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,
        0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,

        // rounds 40..59
        0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,
        0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,
        0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,
        0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,
        0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,

        // rounds 60..79
        0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6,
        0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6,
        0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6,
        0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6,
        0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6
    };
};

struct k256
  : public k<256, 64>
{
    using base = k<256, 64>;

    /// K values (constants).
    static constexpr constants_t get
    {
        // rounds 0..15
        0x428a2f98,	0x71374491,	0xb5c0fbcf,	0xe9b5dba5,
        0x3956c25b,	0x59f111f1,	0x923f82a4,	0xab1c5ed5,
        0xd807aa98,	0x12835b01,	0x243185be,	0x550c7dc3,
        0x72be5d74,	0x80deb1fe,	0x9bdc06a7,	0xc19bf174,

        // rounds 15..31
        0xe49b69c1,	0xefbe4786,	0x0fc19dc6,	0x240ca1cc,
        0x2de92c6f,	0x4a7484aa,	0x5cb0a9dc,	0x76f988da,
        0x983e5152,	0xa831c66d,	0xb00327c8,	0xbf597fc7,
        0xc6e00bf3,	0xd5a79147,	0x06ca6351,	0x14292967,

        // rounds 32..47
        0x27b70a85,	0x2e1b2138,	0x4d2c6dfc,	0x53380d13,
        0x650a7354,	0x766a0abb,	0x81c2c92e,	0x92722c85,
        0xa2bfe8a1,	0xa81a664b,	0xc24b8b70,	0xc76c51a3,
        0xd192e819,	0xd6990624,	0xf40e3585,	0x106aa070,

        // rounds 48..63
        0x19a4c116,	0x1e376c08,	0x2748774c,	0x34b0bcb5,
        0x391c0cb3,	0x4ed8aa4a,	0x5b9cca4f,	0x682e6ff3,
        0x748f82ee,	0x78a5636f,	0x84c87814,	0x8cc70208,
        0x90befffa,	0xa4506ceb,	0xbef9a3f7,	0xc67178f2
    };
};

struct k512
  : public k<512, 80>
{
    using base = k<512, 80>;

    /// K values (constants).
    static constexpr constants_t get
    {
        // rounds 0..15
        0x428a2f98d728ae22,	0x7137449123ef65cd,
        0xb5c0fbcfec4d3b2f,	0xe9b5dba58189dbbc,
        0x3956c25bf348b538,	0x59f111f1b605d019,
        0x923f82a4af194f9b,	0xab1c5ed5da6d8118,
        0xd807aa98a3030242,	0x12835b0145706fbe,
        0x243185be4ee4b28c,	0x550c7dc3d5ffb4e2,
        0x72be5d74f27b896f,	0x80deb1fe3b1696b1,
        0x9bdc06a725c71235,	0xc19bf174cf692694,

        // rounds 15..31
        0xe49b69c19ef14ad2,	0xefbe4786384f25e3,
        0x0fc19dc68b8cd5b5,	0x240ca1cc77ac9c65,
        0x2de92c6f592b0275,	0x4a7484aa6ea6e483,
        0x5cb0a9dcbd41fbd4,	0x76f988da831153b5,
        0x983e5152ee66dfab,	0xa831c66d2db43210,
        0xb00327c898fb213f,	0xbf597fc7beef0ee4,
        0xc6e00bf33da88fc2,	0xd5a79147930aa725,
        0x06ca6351e003826f,	0x142929670a0e6e70,

        // rounds 32..47
        0x27b70a8546d22ffc,	0x2e1b21385c26c926,
        0x4d2c6dfc5ac42aed,	0x53380d139d95b3df,
        0x650a73548baf63de,	0x766a0abb3c77b2a8,
        0x81c2c92e47edaee6,	0x92722c851482353b,
        0xa2bfe8a14cf10364,	0xa81a664bbc423001,
        0xc24b8b70d0f89791,	0xc76c51a30654be30,
        0xd192e819d6ef5218,	0xd69906245565a910,
        0xf40e35855771202a,	0x106aa07032bbd1b8,

        // rounds 48..63
        0x19a4c116b8d2d0c8,	0x1e376c085141ab53,
        0x2748774cdf8eeb99,	0x34b0bcb5e19b48a8,
        0x391c0cb3c5c95a63,	0x4ed8aa4ae3418acb,
        0x5b9cca4f7763e373,	0x682e6ff3d6b2b8a3,
        0x748f82ee5defb2fc,	0x78a5636f43172f60,
        0x84c87814a1f0ab72,	0x8cc702081a6439ec,
        0x90befffa23631e28,	0xa4506cebde82bde9,
        0xbef9a3f7b2c67915,	0xc67178f2e372532b,

        // rounds 64..79
        0xca273eceea26619c,	0xd186b8c721c0c207,
        0xeada7dd6cde0eb1e,	0xf57d4f7fee6ed178,
        0x06f067aa72176fba,	0x0a637dc5a2c898a6,
        0x113f9804bef90dae,	0x1b710b35131c471b,
        0x28db77f523047d84,	0x32caab7b40c72493,
        0x3c9ebe0a15c9bebc,	0x431d67c49c100d4c,
        0x4cc5d4becb3e42b6,	0x597f299cfc657e2a,
        0x5fcb6fab3ad6faec,	0x6c44198c4a475817
    };
};

template <typename Constants, size_t Digest = Constants::strength,
    if_not_greater<Digest, Constants::size> = true>
struct h
{
    using K = Constants;

    /// These constants are exposed though H on the algorithm.
    static constexpr auto digest       = Digest;
    static constexpr auto size         = K::size;
    static constexpr auto rounds       = K::rounds;
    static constexpr auto word_bits    = bytes<size>;
    static constexpr auto word_bytes   = bytes<word_bits>;
    static constexpr auto block_words  = bytes<size> / to_half(word_bytes);
    static constexpr auto chunk_words  = to_half(block_words);
    static constexpr auto buffer_words = rounds;
    static constexpr auto state_words  = Digest == 160 ? 5 : 8;

    /// These types are exposed though H on the algorithm.
    using byte_t   = uint8_t;
    using word_t   = unsigned_type<word_bytes>;
    using state_t  = std_array<word_t, state_words>;  // IV/state
    using words_t  = std_array<word_t, block_words>;  // one block pad
    using chunk_t  = std_array<word_t, chunk_words>;  // half block pad
    using buffer_t = std_array<word_t, buffer_words>; // expanded one block pad
    using statep_t = std_array<word_t, block_words - state_words>; // state pad
    using stream_t = std_array<byte_t, block_words * word_bytes>; // stream pad
};

struct h160
  : public h<k160>
{
    using base = h<k160>;

    struct H
    {
        /// H values (initial state).
        static constexpr const base::state_t get
        {
            0x67452301,
            0xefcdab89,
            0x98badcfe,
            0x10325476,
            0xc3d2e1f0
        };
    };

    struct pad
    {
        /// chunk (8 words)
        static constexpr base::chunk_t chunk
        {
            0x80000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000100
        };

        /// state (11 [16-5] words)
        static constexpr base::statep_t state
        {
            0x80000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x000000fa
        };

        /// block (16 words)
        static constexpr base::words_t block
        {
            0x80000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000200
        };

        /// Rounds-count (80 words).
        static constexpr base::buffer_t buffer
        {
            // block (16 words)
            0x80000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000200,

            // prepared 1
            0x00000001, 0x00000000, 0x00000400, 0x00000002,
            0x00000000, 0x00000800, 0x00000004, 0x00000400,
            0x00001002, 0x00000008, 0x00000000, 0x00002000,
            0x00000010, 0x00001400, 0x0000400a, 0x00000c20,

            // prepared 2
            0x00000006, 0x00008000, 0x00001040, 0x00005008,
            0x00010028, 0x00001080, 0x00000008, 0x00021000,
            0x00000108, 0x00014000, 0x000400a0, 0x0000ca00,
            0x00000064, 0x00081000, 0x00011408, 0x00053888,

            // prepared 3
            0x0010029c, 0x00010800, 0x00005080, 0x00211028,
            0x00001088, 0x00148000, 0x00400a40, 0x000cf000,
            0x00010668, 0x00811080, 0x00114088, 0x00519880,
            0x010028c8, 0x0011d000, 0x000108a8, 0x0211d080,

            // prepared 4
            0x000108e8, 0x01400000, 0x0401a000, 0x00ca0080,
            0x00006400, 0x08100000, 0x01140800, 0x0538a800,
            0x10029c10, 0x01090000, 0x0050c080, 0x2111a820,
            0x001088c0, 0x14818000, 0x400b40c0, 0x0cf3e080
        };

        /// block (64 bytes)
        static constexpr base::stream_t stream
        {
            0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
    };
};

// Digest 224 changes IV and therefore buffer.
template <size_t Digest = 256,
    bool_if<Digest == 224 || Digest == 256> = true>
struct h256
  : public h<k256, Digest>
{
    using base = h<k256, Digest>;

    struct H
    {
        /// H values (initial state).
        static constexpr base::state_t get
        {
            0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
            0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
        };
    };

    struct pad
    {
        /// chunk (8 words)
        static constexpr base::chunk_t chunk = h160::pad::chunk;

        /// state (8 words)
        static constexpr base::statep_t state = chunk;

        /// block (16 words)
        static constexpr base::words_t block = h160::pad::block;

        /// Rounds-count (64 words).
        static constexpr base::buffer_t buffer
        {
            // block (16 words)
            0x80000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000200,

            // prepared 1
            0x80000000, 0x01400000, 0x00205000, 0x00005088,
            0x22000800, 0x22550014, 0x05089742, 0xa0000020,
            0x5a880000, 0x005c9400, 0x0016d49d, 0xfa801f00,
            0xd33225d0, 0x11675959, 0xf6e6bfda, 0xb30c1549,

            // prepared 2
            0x08b2b050, 0x9d7c4c27, 0x0ce2a393, 0x88e6e1ea,
            0xa52b4335, 0x67a16f49, 0xd732016f, 0x4eeb2e91,
            0x5dbf55e5, 0x8eee2335, 0xe2bc5ec2, 0xa83f4394,
            0x45ad78f7, 0x36f3d0cd, 0xd99c05e8, 0xb0511dc7,

            // prepared 3
            0x69bc7ac4, 0xbd11375b, 0xe3ba71e5, 0x3b209ff2,
            0x18feee17, 0xe25ad9e7, 0x13375046, 0x0515089d,
            0x4f0d0f04, 0x2627484e, 0x310128d2, 0xc668b434,
            0x420841cc, 0x62d311b8, 0xe59ba771, 0x85a7a484
        };

        // block (64 bytes)
        static constexpr base::stream_t stream = h160::pad::stream;
    };
};

// Digest 224/256/384 change IV and therefore buffer.
template <size_t Digest = 512,
    bool_if<Digest == 224 || Digest == 256 ||
            Digest == 384 || Digest == 512> = true>
struct h512
  : public h<k512, Digest>
{
    using base = h<k512, Digest>;

    struct H
    {
        /// H values (initial state).
        static constexpr base::state_t get
        {
            // initialization vector
            0x6a09e667f3bcc908,
            0xbb67ae8584caa73b,
            0x3c6ef372fe94f82b,
            0xa54ff53a5f1d36f1,
            0x510e527fade682d1,
            0x9b05688c2b3e6c1f,
            0x1f83d9abfb41bd6b,
            0x5be0cd19137e2179
        };
    };

    struct pad
    {
        /// chunk (8 words)
        static constexpr base::chunk_t chunk
        {
            0x8000000000000000, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000200
        };

        /// state (8 words)
        static constexpr base::statep_t state = chunk;

        /// block (16 words)
        static constexpr base::words_t block
        {
            0x8000000000000000, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000400
        };

        /// Rounds-count (80 words).
        static constexpr base::buffer_t buffer
        {
            // block (16 words)
            0x8000000000000000, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000000,
            0x0000000000000000, 0x0000000000000400,

            // prepared 1
            0x8000000000000000, 0x0080000000002010,
            0x0200100000000004, 0x0000001000010000,
            0x1008000002000020, 0x20000080400a0400,
            0x8004220110080540, 0xc000000001400010,
            0x0309108000404820, 0x050228000a0500b2,
            0x1140a03320144028, 0x981b0800170010f2,
            0x224501b2182e3842, 0x40aaa124e9648a77,
            0x95a95ddeaa673c11, 0xd9a25c368ce5a8df,

            // prepared 2
            0xdde221370c30ba4a, 0x92363c5416ac61fb,
            0x954af2894094852f, 0x6a3f61d93be874e0,
            0x99b9ee900c20fa1d, 0x15fb79cecd3c7c6d,
            0x9bd09e44ba76c0a7, 0xdf6535d6cb806a60,
            0x4acca272eea082e3, 0xaff8ce10b19b2e94,
            0x8237f36b92a68615, 0x9dc44caed53d87a4,
            0xd34b1fc9c4087634, 0x13877c6f75d59407,
            0x3f9da50cf87a4a06, 0x78fb529d5788517f,

            // prepared 3
            0x76dbccb6f9cbd353, 0xc49f6cd2882de777,
            0xd62094e578b97aaf, 0xa908a2d39ccae43a,
            0x320efc28fa75a859, 0xd743be93132b3b61,
            0x4954238de05d52da, 0x7b26c7f84876f42f,
            0xb357836b91080425, 0x621650501aa2c568,
            0xaf7af3fd9fbc62c5, 0x763b731fe771f67f,
            0x2eb8d7dc388abaa5, 0x0536b05ae9da1b8a,
            0xa0e20a27fb7a6753, 0x801978c883fcf485,

            // prepared 4
            0xb66de1ee4d774abc, 0x55dfa043ceb10895,
            0x14bc7ecc899ddfc0, 0x26cbbde46844e311,
            0xe166cd171201bd2c, 0x213c9b6b18ee775b,
            0x9aa214aaa725363d, 0xf69e302a02a2b922,
            0xd4a1d4dc2e580485, 0x6a2f6d4ae8d99e94,
            0x419b4123147570f2, 0xecd33d74f247460a,
            0x7c4ba374352ec6c8, 0xb054e2db4d0dc1ce,
            0x17820ce42463abd1, 0x740727d55fc2865e
        };

        /// block (128 bytes)
        static constexpr base::stream_t stream
        {
            0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        };
    };
};

/// FIPS180 SHA1/SHA256/SHA512 variants.
using sha160     = h160;
using sha256_224 = h256<224>;
using sha256_256 = h256<>;
using sha512_256 = h512<256>;
using sha512_224 = h512<224>;
using sha512_384 = h512<384>;
using sha512_512 = h512<>;

/// SHA hashing algorithm.
template <typename SHA>
class algorithm
{
public:
    /// SHA alises.
    /// -----------------------------------------------------------------------

    using H        = typename SHA::H;
    using K        = typename SHA::K;
    using pad      = typename SHA::pad;
    using byte_t   = typename SHA::byte_t;
    using word_t   = typename SHA::word_t;
    using state_t  = typename SHA::state_t;
    using chunk_t  = typename SHA::chunk_t;
    using words_t  = typename SHA::words_t;
    using buffer_t = typename SHA::buffer_t;
    using statep_t = typename SHA::statep_t;
    using stream_t = typename SHA::stream_t;

    /// I/O (local) types.
    /// -----------------------------------------------------------------------

    /// Chunk is 1/2 block, not same as state/digest (sha160).
    static constexpr auto block_bytes   = SHA::block_words * SHA::word_bytes;
    static constexpr auto chunk_bytes   = SHA::chunk_words * SHA::word_bytes;
    static constexpr auto digest_bytes  = bytes<SHA::digest>;
    static constexpr auto count_bits    = block_bytes;
    static constexpr auto count_bytes   = bytes<count_bits>;
    static constexpr auto big_end_count = true;

    /// Blocks is a vector of cref (use emplace(block)).
    /// count_t is 64/128 bit for 64/128 byte blocks (sha512 uses uintx_t).
    using block_t   = std_array<byte_t, block_bytes>;
    using half_t    = std_array<byte_t, chunk_bytes>;
    using digest_t  = std_array<byte_t, digest_bytes>;
    using count_t   = unsigned_exact_type<count_bytes>;
    using blocks_t  = std_vector<cref<block_t>>;
    using digests_t = std_vector<digest_t>;

    /// Limits incorporate requirement to encode counter in final block.
    static constexpr auto limit_bits = maximum<count_t> - count_bits;
    static constexpr auto limit_bytes = to_floored_bytes(limit_bits);

    /// Streaming (single hash, one/multiple full blocks).
    static constexpr void accumulate(state_t& state, const block_t& block) NOEXCEPT;
    static VCONSTEXPR void accumulate(state_t& state, const blocks_t& blocks) NOEXCEPT;
    static constexpr digest_t finalize(const state_t& state) NOEXCEPT;

    /// Finalized single hash (one half block or one/multiple full blocks).
    static constexpr digest_t hash(const half_t& half) NOEXCEPT;
    static constexpr digest_t hash(const block_t& block) NOEXCEPT;
    static VCONSTEXPR digest_t hash(const blocks_t& blocks) NOEXCEPT;

    /// Finalized double hash (N independent blocks produces N/2 hashes).
    static constexpr digest_t double_hash(const block_t& block) NOEXCEPT;
    static VCONSTEXPR digests_t double_hash(const blocks_t& blocks) NOEXCEPT;

protected:
    /// Functions
    /// -----------------------------------------------------------------------

    static constexpr auto parity(auto x, auto y, auto z) NOEXCEPT;
    static constexpr auto choice(auto x, auto y, auto z) NOEXCEPT;
    static constexpr auto majority(auto x, auto y, auto z) NOEXCEPT;

    static constexpr auto SIGMA0(auto x) NOEXCEPT;
    static constexpr auto SIGMA1(auto x) NOEXCEPT;
    static constexpr auto sigma0(auto x) NOEXCEPT;
    static constexpr auto sigma1(auto x) NOEXCEPT;

    /// Rounds
    /// -----------------------------------------------------------------------

    template<size_t Round>
    static constexpr auto functor() NOEXCEPT;

    template<size_t Round>
    static constexpr void round(auto a, auto& b, auto c, auto d, auto& e,
        auto w) NOEXCEPT;

    template<size_t Round>
    static constexpr void round(auto a, auto b, auto c, auto& d, auto e,
        auto f, auto g, auto& h, auto w) NOEXCEPT;

    template<size_t Round>
    static constexpr void round(auto& out, const auto& in) NOEXCEPT;
    static constexpr void rounding(state_t& out, const buffer_t& in) NOEXCEPT;

    template<size_t Word>
    static constexpr void prepare(auto& out) NOEXCEPT;
    static constexpr void preparing(buffer_t& out) NOEXCEPT;
    static constexpr void summarize(state_t& out, const state_t& in) NOEXCEPT;
    static constexpr void push_state(buffer_t& out, const state_t& in) NOEXCEPT;

    /// Padding
    /// -----------------------------------------------------------------------
    static constexpr void pad_one(buffer_t& out) NOEXCEPT;
    static constexpr void pad_half(buffer_t& out) NOEXCEPT;
    static constexpr void pad_state(buffer_t& out) NOEXCEPT;
    static constexpr void pad_count(buffer_t& out, count_t blocks) NOEXCEPT;

    /// Parsing
    /// -----------------------------------------------------------------------
    static constexpr void big_one(buffer_t& out, const block_t& in) NOEXCEPT;
    static constexpr void big_half(buffer_t& out, const half_t& in) NOEXCEPT;
    static constexpr digest_t big_state(const state_t& in) NOEXCEPT;
};

#ifndef TESTS

// k<,>
static_assert(k<160, 80>::size == 256);
static_assert(k<256, 64>::size == 256);
static_assert(k<512, 80>::size == 512);
static_assert(k<160, 80>::rounds == 80);
static_assert(k<256, 64>::rounds == 64);
static_assert(k<512, 80>::rounds == 80);
static_assert(k<160, 80>::strength == 160);
static_assert(k<256, 64>::strength == 256);
static_assert(k<512, 80>::strength == 512);
static_assert(is_same_type<k<256, 64>::constants_t, std_array<uint32_t, 64>>);
static_assert(is_same_type<k<160, 80>::constants_t, std_array<uint32_t, 80>>);
static_assert(is_same_type<k<512, 80>::constants_t, std_array<uint64_t, 80>>);

// k160
static_assert(k160::size == 256);
static_assert(k160::rounds == 80);
static_assert(k160::strength == 160);
static_assert(k160::get[0] == 0x5a827999);
static_assert(k160::get[19] == 0x5a827999);
static_assert(k160::get[20] == 0x6ed9eba1);
static_assert(k160::get[39] == 0x6ed9eba1);
static_assert(k160::get[40] == 0x8f1bbcdc);
static_assert(k160::get[59] == 0x8f1bbcdc);
static_assert(k160::get[60] == 0xca62c1d6);
static_assert(k160::get[79] == 0xca62c1d6);
static_assert(is_same_type<k160::constants_t, std_array<uint32_t, 80>>);

// k256
static_assert(k256::size == 256);
static_assert(k256::rounds == 64);
static_assert(k256::strength == 256);
static_assert(k256::get[0] == 0x428a2f98);
static_assert(k256::get[63] == 0xc67178f2);
static_assert(is_same_type<k256::constants_t, std_array<uint32_t, 64>>);

// k512
static_assert(k512::size == 512);
static_assert(k512::rounds == 80);
static_assert(k512::strength == 512);
static_assert(k512::get[0] == 0x428a2f98d728ae22);
static_assert(k512::get[79] == 0x6c44198c4a475817);
static_assert(is_same_type<k512::constants_t, std_array<uint64_t, 80>>);

// h<k160,...>
static_assert(h<k160>::size == 256);
static_assert(h<k160>::digest == 160);
static_assert(h<k160, 160>::word_bits == 32);
static_assert(h<k160, 160>::word_bytes == 4);
static_assert(h<k160, 160>::chunk_words == 8);
static_assert(h<k160, 160>::block_words == 16);
static_assert(h<k160, 160>::buffer_words == 80);
static_assert(h<k160, 160>::state_words == 5);
static_assert(is_same_type<h<k160, 160>::K, k160>);
static_assert(is_same_type<h<k160, 160>::byte_t, uint8_t>);
static_assert(is_same_type<h<k160, 160>::word_t, uint32_t>);
static_assert(is_same_type<h<k160, 160>::state_t, std_array<uint32_t, 5>>);
static_assert(is_same_type<h<k160, 160>::chunk_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<h<k160, 160>::words_t, std_array<uint32_t, 16>>);
static_assert(is_same_type<h<k160, 160>::buffer_t, std_array<uint32_t, 80>>);
static_assert(is_same_type<h<k160, 160>::statep_t, std_array<uint32_t, 11>>);
static_assert(is_same_type<h<k160, 160>::stream_t, std_array<uint8_t, 64>>);

// h<k256,...>
static_assert(h<k256, 256>::size == 256);
static_assert(h<k256, 224>::size == 256);
static_assert(h<k256, 256>::digest == 256);
static_assert(h<k256, 224>::digest == 224);
static_assert(h<k256, 256>::word_bits == 32);
static_assert(h<k256, 224>::word_bits == 32);
static_assert(h<k256, 256>::word_bytes == 4);
static_assert(h<k256, 224>::word_bytes == 4);
static_assert(h<k256, 256>::chunk_words == 8);
static_assert(h<k256, 224>::chunk_words == 8);
static_assert(h<k256, 256>::block_words == 16);
static_assert(h<k256, 224>::block_words == 16);
static_assert(h<k256, 256>::buffer_words == 64);
static_assert(h<k256, 224>::buffer_words == 64);
static_assert(h<k256, 256>::state_words == 8);
static_assert(h<k256, 224>::state_words == 8);
static_assert(is_same_type<h<k256, 256>::K, k256>);
static_assert(is_same_type<h<k256, 224>::K, k256>);
static_assert(is_same_type<h<k256, 256>::byte_t, uint8_t>);
static_assert(is_same_type<h<k256, 224>::byte_t, uint8_t>);
static_assert(is_same_type<h<k256, 256>::word_t, uint32_t>);
static_assert(is_same_type<h<k256, 224>::word_t, uint32_t>);
static_assert(is_same_type<h<k256, 256>::state_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<h<k256, 224>::state_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<h<k256, 256>::chunk_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<h<k256, 224>::chunk_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<h<k256, 256>::words_t, std_array<uint32_t, 16>>);
static_assert(is_same_type<h<k256, 224>::words_t, std_array<uint32_t, 16>>);
static_assert(is_same_type<h<k256, 256>::buffer_t, std_array<uint32_t, 64>>);
static_assert(is_same_type<h<k256, 224>::buffer_t, std_array<uint32_t, 64>>);
static_assert(is_same_type<h<k256, 256>::statep_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<h<k256, 224>::statep_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<h<k256, 256>::stream_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<h<k256, 224>::stream_t, std_array<uint8_t, 64>>);

// h<k512,...>
static_assert(h<k512, 512>::size == 512);
static_assert(h<k512, 384>::size == 512);
static_assert(h<k512, 224>::size == 512);
static_assert(h<k512, 256>::size == 512);
static_assert(h<k512, 512>::digest == 512);
static_assert(h<k512, 384>::digest == 384);
static_assert(h<k512, 224>::digest == 224);
static_assert(h<k512, 256>::digest == 256);
static_assert(h<k512, 512>::word_bits == 64);
static_assert(h<k512, 384>::word_bits == 64);
static_assert(h<k512, 224>::word_bits == 64);
static_assert(h<k512, 256>::word_bits == 64);
static_assert(h<k512, 512>::word_bytes == 8);
static_assert(h<k512, 384>::word_bytes == 8);
static_assert(h<k512, 224>::word_bytes == 8);
static_assert(h<k512, 256>::word_bytes == 8);
static_assert(h<k512, 512>::chunk_words == 8);
static_assert(h<k512, 384>::chunk_words == 8);
static_assert(h<k512, 224>::chunk_words == 8);
static_assert(h<k512, 256>::chunk_words == 8);
static_assert(h<k512, 512>::block_words == 16);
static_assert(h<k512, 384>::block_words == 16);
static_assert(h<k512, 224>::block_words == 16);
static_assert(h<k512, 256>::block_words == 16);
static_assert(h<k512, 512>::buffer_words == 80);
static_assert(h<k512, 384>::buffer_words == 80);
static_assert(h<k512, 224>::buffer_words == 80);
static_assert(h<k512, 256>::buffer_words == 80);
static_assert(h<k512, 512>::state_words == 8);
static_assert(h<k512, 384>::state_words == 8);
static_assert(h<k512, 224>::state_words == 8);
static_assert(h<k512, 256>::state_words == 8);
static_assert(is_same_type<h<k512, 512>::K, k512>);
static_assert(is_same_type<h<k512, 384>::K, k512>);
static_assert(is_same_type<h<k512, 224>::K, k512>);
static_assert(is_same_type<h<k512, 256>::K, k512>);
static_assert(is_same_type<h<k512, 512>::byte_t, uint8_t>);
static_assert(is_same_type<h<k512, 384>::byte_t, uint8_t>);
static_assert(is_same_type<h<k512, 224>::byte_t, uint8_t>);
static_assert(is_same_type<h<k512, 256>::byte_t, uint8_t>);
static_assert(is_same_type<h<k512, 512>::word_t, uint64_t>);
static_assert(is_same_type<h<k512, 384>::word_t, uint64_t>);
static_assert(is_same_type<h<k512, 224>::word_t, uint64_t>);
static_assert(is_same_type<h<k512, 256>::word_t, uint64_t>);
static_assert(is_same_type<h<k512, 512>::state_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<h<k512, 384>::state_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<h<k512, 224>::state_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<h<k512, 256>::state_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<h<k512, 512>::chunk_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<h<k512, 384>::chunk_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<h<k512, 224>::chunk_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<h<k512, 256>::chunk_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<h<k512, 512>::words_t, std_array<uint64_t, 16>>);
static_assert(is_same_type<h<k512, 384>::words_t, std_array<uint64_t, 16>>);
static_assert(is_same_type<h<k512, 224>::words_t, std_array<uint64_t, 16>>);
static_assert(is_same_type<h<k512, 256>::words_t, std_array<uint64_t, 16>>);
static_assert(is_same_type<h<k512, 512>::buffer_t, std_array<uint64_t, 80>>);
static_assert(is_same_type<h<k512, 384>::buffer_t, std_array<uint64_t, 80>>);
static_assert(is_same_type<h<k512, 224>::buffer_t, std_array<uint64_t, 80>>);
static_assert(is_same_type<h<k512, 256>::buffer_t, std_array<uint64_t, 80>>);
static_assert(is_same_type<h<k512, 512>::statep_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<h<k512, 384>::statep_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<h<k512, 224>::statep_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<h<k512, 256>::statep_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<h<k512, 512>::stream_t, std_array<uint8_t, 128>>);
static_assert(is_same_type<h<k512, 384>::stream_t, std_array<uint8_t, 128>>);
static_assert(is_same_type<h<k512, 224>::stream_t, std_array<uint8_t, 128>>);
static_assert(is_same_type<h<k512, 256>::stream_t, std_array<uint8_t, 128>>);

// h160
static_assert(h160::size == 256);
static_assert(h160::digest == 160);
static_assert(h160::rounds == 80);
static_assert(h160::K::rounds == 80);
static_assert(h160::K::get[0] == 0x5a827999);
static_assert(h160::K::get[79] == 0xca62c1d6);
static_assert(h160::K::get.size() == 80);
static_assert(h160::H::get[0] == 0x67452301);
static_assert(h160::H::get[4] == 0xc3d2e1f0);
static_assert(h160::H::get.size() == 5);
static_assert(h160::pad::chunk.size() == 8);
static_assert(h160::pad::chunk[0] == 0x80000000);
static_assert(h160::pad::chunk[7] == 0x00000100);
static_assert(h160::pad::state.size() == 11);
static_assert(h160::pad::state[0] == 0x80000000);
static_assert(h160::pad::state[10] == 0x000000fa);
static_assert(h160::pad::block.size() == 16);
static_assert(h160::pad::block[0] == 0x80000000);
static_assert(h160::pad::block[15] == 0x00000200);
static_assert(h160::pad::buffer.size() == 80);
static_assert(h160::pad::buffer[0] == 0x80000000);
static_assert(h160::pad::buffer[79] == 0x0cf3e080);
static_assert(h160::pad::stream.size() == 64);
static_assert(h160::pad::stream[0] == 0x80);
static_assert(h160::pad::stream[63] == 0x00);

// h256<256>
static_assert(h256<256>::size == 256);
static_assert(h256<256>::digest == 256);
static_assert(h256<256>::rounds == 64);
static_assert(h256<256>::K::rounds == 64);
static_assert(h256<256>::K::get[0] == 0x428a2f98);
static_assert(h256<256>::K::get[63] == 0xc67178f2);
static_assert(h256<256>::K::get.size() == 64);
static_assert(h256<256>::H::get[0] == 0x6a09e667);
static_assert(h256<256>::H::get[7] == 0x5be0cd19);
static_assert(h256<256>::H::get.size() == 8);
static_assert(h256<256>::pad::chunk.size() == 8);
static_assert(h256<256>::pad::chunk[0] == 0x80000000);
static_assert(h256<256>::pad::chunk[7] == 0x00000100);
static_assert(h256<256>::pad::state.size() == 8);
static_assert(h256<256>::pad::state[0] == 0x80000000);
static_assert(h256<256>::pad::state[7] == 0x00000100);
static_assert(h256<256>::pad::block.size() == 16);
static_assert(h256<256>::pad::block[0] == 0x80000000);
static_assert(h256<256>::pad::block[15] == 0x00000200);
static_assert(h256<256>::pad::buffer.size() == 64);
static_assert(h256<256>::pad::buffer[0] == 0x80000000);
static_assert(h256<256>::pad::buffer[63] == 0x85a7a484);
static_assert(h256<256>::pad::stream.size() == 64);
static_assert(h256<256>::pad::stream[0] == 0x80);
static_assert(h256<256>::pad::stream[63] == 0x00);

// h512<512>
static_assert(h512<512>::size == 512);
static_assert(h512<512>::digest == 512);
static_assert(h512<512>::rounds == 80);
static_assert(h512<512>::K::rounds == 80);
static_assert(h512<512>::K::get[0] == 0x428a2f98d728ae22);
static_assert(h512<512>::K::get[79] == 0x6c44198c4a475817);
static_assert(h512<512>::K::get.size() == 80);
static_assert(h512<512>::H::get[0] == 0x6a09e667f3bcc908);
static_assert(h512<512>::H::get[7] == 0x5be0cd19137e2179);
static_assert(h512<512>::H::get.size() == 8);
static_assert(h512<512>::pad::chunk.size() == 8);
static_assert(h512<512>::pad::chunk[0] == 0x8000000000000000);
static_assert(h512<512>::pad::chunk[7] == 0x0000000000000200);
static_assert(h512<512>::pad::state.size() == 8);
static_assert(h512<512>::pad::state[0] == 0x8000000000000000);
static_assert(h512<512>::pad::state[7] == 0x0000000000000200);
static_assert(h512<512>::pad::block.size() == 16);
static_assert(h512<512>::pad::block[0] == 0x8000000000000000);
static_assert(h512<512>::pad::block[15] == 0x0000000000000400);
static_assert(h512<512>::pad::buffer.size() == 80);
static_assert(h512<512>::pad::buffer[0] == 0x8000000000000000);
static_assert(h512<512>::pad::buffer[79] == 0x740727d55fc2865e);
static_assert(h512<512>::pad::stream.size() == 128);
static_assert(h512<512>::pad::stream[0] == 0x80);
static_assert(h512<512>::pad::stream[127] == 0x00);

// sha160
static_assert(sha160::size == 256);
static_assert(sha160::digest == 160);
static_assert(sha160::K::rounds == 80);
static_assert(sha160::K::get.size() == 80);
static_assert(sha160::K::get[0] == 0x5a827999);
static_assert(sha160::K::get[79] == 0xca62c1d6);
static_assert(sha160::H::get.size() == 5);
static_assert(sha160::H::get[0] == 0x67452301);
static_assert(sha160::H::get[4] == 0xc3d2e1f0);

// sha256_256
static_assert(sha256_256::size == 256);
static_assert(sha256_256::digest == 256);
static_assert(sha256_256::K::rounds == 64);
static_assert(sha256_256::K::get.size() == 64);
static_assert(sha256_256::K::get[0] == 0x428a2f98);
static_assert(sha256_256::K::get[63] == 0xc67178f2);
static_assert(sha256_256::H::get.size() == 8);
static_assert(sha256_256::H::get[0] == 0x6a09e667);
static_assert(sha256_256::H::get[7] == 0x5be0cd19);

// sha512_512
static_assert(sha512_512::size == 512);
static_assert(sha512_512::digest == 512);
static_assert(sha512_512::K::rounds == 80);
static_assert(sha512_512::K::get.size() == 80);
static_assert(sha512_512::K::get[0] == 0x428a2f98d728ae22);
static_assert(sha512_512::K::get[79] == 0x6c44198c4a475817);
static_assert(sha512_512::H::get.size() == 8);
static_assert(sha512_512::H::get[0] == 0x6a09e667f3bcc908);
static_assert(sha512_512::H::get[7] == 0x5be0cd19137e2179);

// Truncations.
static_assert(sha256_224::digest == 224);
static_assert(sha512_256::digest == 256);
static_assert(sha512_224::digest == 224);
static_assert(sha512_384::digest == 384);

// algorithm<sha160>
static_assert(algorithm<sha160>::big_end_count);
static_assert(algorithm<sha160>::block_bytes == 64u);
static_assert(algorithm<sha160>::chunk_bytes == 32u);
static_assert(algorithm<sha160>::digest_bytes == 20u);
static_assert(algorithm<sha160>::count_bits == 64u);
static_assert(algorithm<sha160>::count_bytes == 8u);
static_assert(algorithm<sha160>::H::get.size() == 5u);
static_assert(algorithm<sha160>::K::get.size() == 80u);
static_assert(algorithm<sha160>::pad::chunk.size() == 8u);
static_assert(algorithm<sha160>::limit_bits == std::numeric_limits<uint64_t>::max() - 64u);
static_assert(algorithm<sha160>::limit_bytes == algorithm<sha160>::limit_bits / byte_bits);
static_assert(is_same_type<algorithm<sha160>::byte_t, uint8_t>);
static_assert(is_same_type<algorithm<sha160>::word_t, uint32_t>);
static_assert(is_same_type<algorithm<sha160>::state_t, std_array<uint32_t, 5>>);
static_assert(is_same_type<algorithm<sha160>::chunk_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<algorithm<sha160>::words_t, std_array<uint32_t, 16>>);
static_assert(is_same_type<algorithm<sha160>::statep_t, std_array<uint32_t, 11>>);
static_assert(is_same_type<algorithm<sha160>::buffer_t, std_array<uint32_t, 80>>);
static_assert(is_same_type<algorithm<sha160>::stream_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<sha160>::block_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<sha160>::half_t, std_array<uint8_t, 32>>);
static_assert(is_same_type<algorithm<sha160>::digest_t, std_array<uint8_t, 20>>);
static_assert(is_same_type<algorithm<sha160>::count_t, uint64_t>);
static_assert(is_same_type<algorithm<sha160>::blocks_t, std_vector<cref<std_array<uint8_t, 64>>>>);
static_assert(is_same_type<algorithm<sha160>::digests_t, std_vector<std_array<uint8_t, 20>>>);
static_assert(is_same_type<decltype(algorithm<sha160>::limit_bits), const uint64_t>);
static_assert(is_same_type<decltype(algorithm<sha160>::limit_bytes), const uint64_t>);

// algorithm<sha256_256>
static_assert(algorithm<sha256_256>::big_end_count);
static_assert(algorithm<sha256_256>::block_bytes == 64u);
static_assert(algorithm<sha256_256>::chunk_bytes == 32u);
static_assert(algorithm<sha256_256>::digest_bytes == 32u);
static_assert(algorithm<sha256_256>::count_bits == 64u);
static_assert(algorithm<sha256_256>::count_bytes == 8u);
static_assert(algorithm<sha256_256>::H::get.size() == 8u);
static_assert(algorithm<sha256_256>::K::get.size() == 64u);
static_assert(algorithm<sha256_256>::pad::chunk.size() == 8u);
static_assert(algorithm<sha256_256>::limit_bits == std::numeric_limits<uint64_t>::max() - 64u);
static_assert(algorithm<sha256_256>::limit_bytes == algorithm<sha256_256>::limit_bits / byte_bits);
static_assert(is_same_type<algorithm<sha256_256>::byte_t, uint8_t>);
static_assert(is_same_type<algorithm<sha256_256>::word_t, uint32_t>);
static_assert(is_same_type<algorithm<sha256_256>::state_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<algorithm<sha256_256>::chunk_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<algorithm<sha256_256>::words_t, std_array<uint32_t, 16>>);
static_assert(is_same_type<algorithm<sha256_256>::statep_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<algorithm<sha256_256>::buffer_t, std_array<uint32_t, 64>>);
static_assert(is_same_type<algorithm<sha256_256>::stream_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<sha256_256>::block_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<sha256_256>::half_t, std_array<uint8_t, 32>>);
static_assert(is_same_type<algorithm<sha256_256>::digest_t, std_array<uint8_t, 32>>);
static_assert(is_same_type<algorithm<sha256_256>::count_t, uint64_t>);
static_assert(is_same_type<algorithm<sha256_256>::blocks_t, std_vector<cref<std_array<uint8_t, 64>>>>);
static_assert(is_same_type<algorithm<sha256_256>::digests_t, std_vector<std_array<uint8_t, 32>>>);
static_assert(is_same_type<decltype(algorithm<sha256_256>::limit_bits), const uint64_t>);
static_assert(is_same_type<decltype(algorithm<sha256_256>::limit_bytes), const uint64_t>);

// algorithm<sha512_512>
static_assert(algorithm<sha512_512>::big_end_count);
static_assert(algorithm<sha512_512>::block_bytes == 128u);
static_assert(algorithm<sha512_512>::chunk_bytes == 64u);
static_assert(algorithm<sha512_512>::digest_bytes == 64u);
static_assert(algorithm<sha512_512>::count_bits == 128u);
static_assert(algorithm<sha512_512>::count_bytes == 16u);
static_assert(algorithm<sha512_512>::H::get.size() == 8u);
static_assert(algorithm<sha512_512>::K::get.size() == 80u);
static_assert(algorithm<sha512_512>::pad::chunk.size() == 8u);
static_assert(algorithm<sha512_512>::limit_bits == std::numeric_limits<uint128_t>::max() - 128u);
static_assert(algorithm<sha512_512>::limit_bytes == algorithm<sha512_512>::limit_bits / byte_bits);
static_assert(is_same_type<algorithm<sha512_512>::byte_t, uint8_t>);
static_assert(is_same_type<algorithm<sha512_512>::word_t, uint64_t>);
static_assert(is_same_type<algorithm<sha512_512>::state_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<algorithm<sha512_512>::chunk_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<algorithm<sha512_512>::words_t, std_array<uint64_t, 16>>);
static_assert(is_same_type<algorithm<sha512_512>::statep_t, std_array<uint64_t, 8>>);
static_assert(is_same_type<algorithm<sha512_512>::buffer_t, std_array<uint64_t, 80>>);
static_assert(is_same_type<algorithm<sha512_512>::stream_t, std_array<uint8_t, 128>>);
static_assert(is_same_type<algorithm<sha512_512>::block_t, std_array<uint8_t, 128>>);
static_assert(is_same_type<algorithm<sha512_512>::half_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<sha512_512>::digest_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<sha512_512>::count_t, uint128_t>);
static_assert(is_same_type<algorithm<sha512_512>::blocks_t, std_vector<cref<std_array<uint8_t, 128>>>>);
static_assert(is_same_type<algorithm<sha512_512>::digests_t, std_vector<std_array<uint8_t, 64>>>);
static_assert(is_same_type<decltype(algorithm<sha512_512>::limit_bits), const uint128_t>);
static_assert(is_same_type<decltype(algorithm<sha512_512>::limit_bytes), const uint128_t>);

// constexpr for all variants of sha160/256/512!

////using sha_160 = algorithm<h160>;
////static_assert(sha_160::hash(sha_160::half_t{})  == base16_array("de8a847bff8c343d69b853a215e6ee775ef2ef96"));
////static_assert(sha_160::hash(sha_160::block_t{}) == base16_array("c8d7d0ef0eedfa82d2ea1aa592845b9a6d4b02b7"));
////
////using sha_256 = algorithm<h256<>>;
////static_assert(sha_256::hash(sha_256::half_t{})  == base16_array("66687aadf862bd776c8fc18b8e9f8e20089714856ee233b3902a591d0d5f2925"));
////static_assert(sha_256::hash(sha_256::block_t{}) == base16_array("f5a5fd42d16a20302798ef6ed309979b43003d2320d9f0e8ea9831a92759fb4b"));
////
////using sha_512 = algorithm<h512<>>;
////static_assert(sha_512::hash(sha_512::half_t{})  == base16_array("7be9fda48f4179e611c698a73cff09faf72869431efee6eaad14de0cb44bbf66503f752b7a8eb17083355f3ce6eb7d2806f236b25af96a24e22b887405c20081"));
////static_assert(sha_512::hash(sha_512::block_t{}) == base16_array("ab942f526272e456ed68a979f50202905ca903a141ed98443567b11ef0bf25a552d639051a01be58558122c58e3de07d749ee59ded36acf0c55cd91924d6ba11"));
#endif

BOOST_AUTO_TEST_SUITE_END()
