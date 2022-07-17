// Based on:
// sha256-arm.c - ARMv8 SHA extensions using C intrinsics
// Written and placed in public domain by Jeffrey Walton
// Based on code from ARM, and by Johannes Schneiders, Skip
// Hovsmith and Barry O'Rourke for the mbedTLS project.

#include <bitcoin/system/crypto/sha256.hpp>

#include <stdint.h>
#include <stdlib.h>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace sha256 {

#if !defined(HAVE_ARM)

void hash_neon(state&, const block1&) NOEXCEPT
{
    BC_ASSERT_MSG(false, "hash_neon undefined");
}

void merkle_neon(digest1&, const block1&) NOEXCEPT
{
    BC_ASSERT_MSG(false, "merkle_neon undefined");
}

#else

void hash_neon(state& state, const block1& blocks) NOEXCEPT
{
    BC_PUSH_WARNING(NO_ARRAY_INDEXING)

    constexpr uint32_t k[]
    {
        0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5,
        0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
        0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3,
        0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
        0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC,
        0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
        0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7,
        0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
        0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13,
        0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
        0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3,
        0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
        0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5,
        0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
        0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208,
        0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2,
    };

    uint32x4_t temp0, temp1, temp2;
    uint32x4_t state0, state1, abef_save, cdgh_save;
    uint32x4_t message0, message1, message2, message3;

    // Load state.
    state0 = vld1q_u32(&state[0]);
    state1 = vld1q_u32(&state[4]);

    // TODO: array_cast blocks into array of uint32x4_t.
    // TODO: this is currently precluded by an integral integer constraint.
    // TODO: this will eliminate pointer casts for vld1q_u32(block[n]).

    // One block in four lanes.
    for (auto& block: blocks)
    {
        // Save state.
        abef_save = state0;
        cdgh_save = state1;

        // Load message.
        message0 = vld1q_u32(pointer_cast<const uint32_t>(&block[0]));
        message1 = vld1q_u32(pointer_cast<const uint32_t>(&block[16]));
        message2 = vld1q_u32(pointer_cast<const uint32_t>(&block[32]));
        message3 = vld1q_u32(pointer_cast<const uint32_t>(&block[48]));

        // Reverse for little endian.
        message0 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(message0)));
        message1 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(message1)));
        message2 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(message2)));
        message3 = vreinterpretq_u32_u8(vrev32q_u8(vreinterpretq_u8_u32(message3)));

        temp0 = vaddq_u32(message0, vld1q_u32(&k[0x00]));

        // Rounds 0-3.
        message0 = vsha256su0q_u32(message0, message1);
        temp2 = state0;
        temp1 = vaddq_u32(message1, vld1q_u32(&k[0x04]));
        state0 = vsha256hq_u32(state0, state1, temp0);
        state1 = vsha256h2q_u32(state1, temp2, temp0);
        message0 = vsha256su1q_u32(message0, message2, message3);

        // Rounds 4-7.
        message1 = vsha256su0q_u32(message1, message2);
        temp2 = state0;
        temp0 = vaddq_u32(message2, vld1q_u32(&k[0x08]));
        state0 = vsha256hq_u32(state0, state1, temp1);
        state1 = vsha256h2q_u32(state1, temp2, temp1);
        message1 = vsha256su1q_u32(message1, message3, message0);

        // Rounds 8-11.
        message2 = vsha256su0q_u32(message2, message3);
        temp2 = state0;
        temp1 = vaddq_u32(message3, vld1q_u32(&k[0x0c]));
        state0 = vsha256hq_u32(state0, state1, temp0);
        state1 = vsha256h2q_u32(state1, temp2, temp0);
        message2 = vsha256su1q_u32(message2, message0, message1);

        // Rounds 12-15.
        message3 = vsha256su0q_u32(message3, message0);
        temp2 = state0;
        temp0 = vaddq_u32(message0, vld1q_u32(&k[0x10]));
        state0 = vsha256hq_u32(state0, state1, temp1);
        state1 = vsha256h2q_u32(state1, temp2, temp1);
        message3 = vsha256su1q_u32(message3, message1, message2);

        // Rounds 16-19.
        message0 = vsha256su0q_u32(message0, message1);
        temp2 = state0;
        temp1 = vaddq_u32(message1, vld1q_u32(&k[0x14]));
        state0 = vsha256hq_u32(state0, state1, temp0);
        state1 = vsha256h2q_u32(state1, temp2, temp0);
        message0 = vsha256su1q_u32(message0, message2, message3);

        // Rounds 20-23.
        message1 = vsha256su0q_u32(message1, message2);
        temp2 = state0;
        temp0 = vaddq_u32(message2, vld1q_u32(&k[0x18]));
        state0 = vsha256hq_u32(state0, state1, temp1);
        state1 = vsha256h2q_u32(state1, temp2, temp1);
        message1 = vsha256su1q_u32(message1, message3, message0);

        // Rounds 24-27.
        message2 = vsha256su0q_u32(message2, message3);
        temp2 = state0;
        temp1 = vaddq_u32(message3, vld1q_u32(&k[0x1c]));
        state0 = vsha256hq_u32(state0, state1, temp0);
        state1 = vsha256h2q_u32(state1, temp2, temp0);
        message2 = vsha256su1q_u32(message2, message0, message1);

        // Rounds 28-31.
        message3 = vsha256su0q_u32(message3, message0);
        temp2 = state0;
        temp0 = vaddq_u32(message0, vld1q_u32(&k[0x20]));
        state0 = vsha256hq_u32(state0, state1, temp1);
        state1 = vsha256h2q_u32(state1, temp2, temp1);
        message3 = vsha256su1q_u32(message3, message1, message2);

        // Rounds 32-35.
        message0 = vsha256su0q_u32(message0, message1);
        temp2 = state0;
        temp1 = vaddq_u32(message1, vld1q_u32(&k[0x24]));
        state0 = vsha256hq_u32(state0, state1, temp0);
        state1 = vsha256h2q_u32(state1, temp2, temp0);
        message0 = vsha256su1q_u32(message0, message2, message3);

        // Rounds 36-39.
        message1 = vsha256su0q_u32(message1, message2);
        temp2 = state0;
        temp0 = vaddq_u32(message2, vld1q_u32(&k[0x28]));
        state0 = vsha256hq_u32(state0, state1, temp1);
        state1 = vsha256h2q_u32(state1, temp2, temp1);
        message1 = vsha256su1q_u32(message1, message3, message0);

        // Rounds 40-43.
        message2 = vsha256su0q_u32(message2, message3);
        temp2 = state0;
        temp1 = vaddq_u32(message3, vld1q_u32(&k[0x2c]));
        state0 = vsha256hq_u32(state0, state1, temp0);
        state1 = vsha256h2q_u32(state1, temp2, temp0);
        message2 = vsha256su1q_u32(message2, message0, message1);

        // Rounds 44-47.
        message3 = vsha256su0q_u32(message3, message0);
        temp2 = state0;
        temp0 = vaddq_u32(message0, vld1q_u32(&k[0x30]));
        state0 = vsha256hq_u32(state0, state1, temp1);
        state1 = vsha256h2q_u32(state1, temp2, temp1);
        message3 = vsha256su1q_u32(message3, message1, message2);

        // Rounds 48-51.
        temp2 = state0;
        temp1 = vaddq_u32(message1, vld1q_u32(&k[0x34]));
        state0 = vsha256hq_u32(state0, state1, temp0);
        state1 = vsha256h2q_u32(state1, temp2, temp0);

        // Rounds 52-55.
        temp2 = state0;
        temp0 = vaddq_u32(message2, vld1q_u32(&k[0x38]));
        state0 = vsha256hq_u32(state0, state1, temp1);
        state1 = vsha256h2q_u32(state1, temp2, temp1);

        // Rounds 56-59.
        temp2 = state0;
        temp1 = vaddq_u32(message3, vld1q_u32(&k[0x3c]));
        state0 = vsha256hq_u32(state0, state1, temp0);
        state1 = vsha256h2q_u32(state1, temp2, temp0);

        // Rounds 60-63.
        temp2 = state0;
        state0 = vsha256hq_u32(state0, state1, temp1);
        state1 = vsha256h2q_u32(state1, temp2, temp1);

        // Combine state.
        state0 = vaddq_u32(state0, abef_save);
        state1 = vaddq_u32(state1, cdgh_save);
    }

    // Save state.
    vst1q_u32(&state[0], state0);
    vst1q_u32(&state[4], state1);

    BC_POP_WARNING()
}

void merkle_neon(digest1& out, const block1& blocks) NOEXCEPT
{
    auto state = sha256::initial;
    hash_neon(state, blocks);
    hash_neon(state, array_cast(sha256::pad_64));
    auto buffer = sha256::pad_32;
    to_big_endians(narrowing_array_cast<uint32_t, state_size>(buffer), state);
    state = sha256::initial;
    hash_neon(state, array_cast(buffer));
    to_big_endians(array_cast<uint32_t>(out.front()), state);
}

#endif // HAVE_ARM

} // namespace sha256
} // namespace system
} // namespace libbitcoin
