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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_SIGMA_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_SIGMA_IPP

// sigma0 vectorization.
// ============================================================================

namespace libbitcoin {
namespace system {
namespace sha {

// protected
// ----------------------------------------------------------------------------

TEMPLATE
template<size_t Round, size_t Offset>
INLINE void CLASS::
prepare_1(buffer_t& buffer, const auto& sigmas) NOEXCEPT
{
    static_assert(Round >= 16);
    constexpr auto r02 = Round - 2;
    constexpr auto r07 = Round - 7;
    constexpr auto r16 = Round - 16;

    // buffer[r07 + 7] is buffer[Round + 0], so sigma0 is limited to 8 lanes.
    buffer[Round + Offset] = add(
        add(buffer[r16 + Offset], sigmas[Offset]),
        add(buffer[r07 + Offset], sigma1(buffer[r02 + Offset])));
}

TEMPLATE
template<size_t Round>
INLINE void CLASS::
prepare_8(buffer_t& buffer) NOEXCEPT
{
    // Requires avx512 for sha512 and avx2 for sha256.
    // The simplicity of sha160 message prepare precludes this optimization.
    static_assert(SHA::strength != 160);

    // sigma0x8 message scheduling for single block iteration.
    // Does not alter buffer structure, fully private to this method.
    // Tests with sigma1x2 half lanes vectorization show loss of ~10%.
    // Tests with sigma0x8 full lanes vectorization show gain of ~5%.
    using xword = to_extended<word_t, 8>;
    constexpr auto r15 = Round - 15;
    std_array<word_t, 8> sigmas{};
    const auto& xwords = array_cast<xword, one, r15>(buffer);
    auto& xsigmas = array_cast<xword>(sigmas).front();
    f::store<word_t>(xsigmas, sigma0(f::load<word_t>(xwords.front())));

    prepare_1<Round, 0>(buffer, sigmas);
    prepare_1<Round, 1>(buffer, sigmas);
    prepare_1<Round, 2>(buffer, sigmas);
    prepare_1<Round, 3>(buffer, sigmas);
    prepare_1<Round, 4>(buffer, sigmas);
    prepare_1<Round, 5>(buffer, sigmas);
    prepare_1<Round, 6>(buffer, sigmas);
    prepare_1<Round, 7>(buffer, sigmas);
}

TEMPLATE
template <typename xWord>
INLINE void CLASS::
schedule_sigma(xbuffer_t<xWord>& xbuffer) NOEXCEPT
{
    // Merkle extended buffer is not vector dispatched.
    schedule_(xbuffer);
}

TEMPLATE
void CLASS::
schedule_sigma(buffer_t& buffer) NOEXCEPT
{
    if constexpr (SHA::strength != 160 && have_lanes<word_t, 8>)
    {
        prepare_8<16>(buffer);
        prepare_8<24>(buffer);
        prepare_8<32>(buffer);
        prepare_8<40>(buffer);
        prepare_8<48>(buffer);
        prepare_8<56>(buffer);

        if constexpr (SHA::rounds == 80)
        {
            prepare_8<64>(buffer);
            prepare_8<72>(buffer);
        }

        konstant(buffer);
    }
    else
    {
        schedule_(buffer);
    }
}

// Scheduling interleaved with compression (single blocks).
// ----------------------------------------------------------------------------
// Each group of eight words is prepared one group ahead of its rounds, so that
// vector scheduling executes alongside scalar compression.

TEMPLATE
template<size_t Round>
INLINE void CLASS::
konstant_8(buffer_t& wk, const buffer_t& buffer) NOEXCEPT
{
    // Raw words remain in buffer for scheduling, so K is added to a copy.
    using xword = to_extended<word_t, 8>;
    const auto& xwords = array_cast<xword, one, Round>(buffer);
    auto& xwk = array_cast<xword, one, Round>(wk);
    const auto words = f::load<word_t>(xwords.front());
    const auto constants = f::set<xword>(
        K::get[Round + 0], K::get[Round + 1], K::get[Round + 2],
        K::get[Round + 3], K::get[Round + 4], K::get[Round + 5],
        K::get[Round + 6], K::get[Round + 7]);
    f::store<word_t>(xwk.front(), f::add<word_t>(words, constants));
}

TEMPLATE
template<size_t Round>
INLINE void CLASS::
compress_8(state_t& state, const buffer_t& wk) NOEXCEPT
{
    round<Round + 0, zero>(state, wk);
    round<Round + 1, zero>(state, wk);
    round<Round + 2, zero>(state, wk);
    round<Round + 3, zero>(state, wk);
    round<Round + 4, zero>(state, wk);
    round<Round + 5, zero>(state, wk);
    round<Round + 6, zero>(state, wk);
    round<Round + 7, zero>(state, wk);
}

TEMPLATE
void CLASS::
schedule_compress_sigma(state_t& state, buffer_t& buffer) NOEXCEPT
{
    static_assert(SHA::strength != 160);
    const auto start = state;
    buffer_t wk{};

    konstant_8<0>(wk, buffer);
    compress_8<0>(state, wk);
    prepare_8<16>(buffer);
    konstant_8<8>(wk, buffer);
    compress_8<8>(state, wk);
    prepare_8<24>(buffer);
    konstant_8<16>(wk, buffer);
    compress_8<16>(state, wk);
    prepare_8<32>(buffer);
    konstant_8<24>(wk, buffer);
    compress_8<24>(state, wk);
    prepare_8<40>(buffer);
    konstant_8<32>(wk, buffer);
    compress_8<32>(state, wk);
    prepare_8<48>(buffer);
    konstant_8<40>(wk, buffer);
    compress_8<40>(state, wk);
    prepare_8<56>(buffer);
    konstant_8<48>(wk, buffer);
    compress_8<48>(state, wk);

    if constexpr (SHA::rounds == 80)
    {
        prepare_8<64>(buffer);
        konstant_8<56>(wk, buffer);
        compress_8<56>(state, wk);
        prepare_8<72>(buffer);
        konstant_8<64>(wk, buffer);
        compress_8<64>(state, wk);
        konstant_8<72>(wk, buffer);
        compress_8<72>(state, wk);
    }
    else
    {
        konstant_8<56>(wk, buffer);
        compress_8<56>(state, wk);
    }

    summarize(state, start);
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif
