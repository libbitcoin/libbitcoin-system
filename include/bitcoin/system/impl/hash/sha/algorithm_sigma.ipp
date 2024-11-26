/**
 * Copyright (c) 2011-2024 libbitcoin developers (see AUTHORS)
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
template <typename xWord, if_extended<xWord>>
INLINE auto CLASS::
sigma0_8(auto x1, auto x2, auto x3, auto x4, auto x5, auto x6, auto x7,
    auto x8) NOEXCEPT
{
    return sigma0(set<xWord>(x1, x2, x3, x4, x5, x6, x7, x8));
}

TEMPLATE
template<size_t Round, size_t Offset>
INLINE void CLASS::
prepare1(buffer_t& buffer, const auto& xsigma0) NOEXCEPT
{
    static_assert(Round >= 16);
    constexpr auto r02 = Round - 2;
    constexpr auto r07 = Round - 7;
    constexpr auto r16 = Round - 16;
    constexpr auto s = SHA::word_bits;

    // buffer[r07 + 7] is buffer[Round + 0], so sigma0 is limited to 8 lanes.
    buffer[Round + Offset] = f::add<s>(
        f::add<s>(buffer[r16 + Offset], get<word_t, Offset>(xsigma0)),
        f::add<s>(buffer[r07 + Offset], sigma1(buffer[r02 + Offset])));
}

TEMPLATE
template<size_t Round>
INLINE void CLASS::
prepare8(buffer_t& buffer) NOEXCEPT
{
    // Requires avx512 for sha512 and avx2 for sha256.
    // The simplicity of sha160 message prepare precludes this optimization.
    static_assert(SHA::strength != 160);

    // sigma0x8 message scheduling for single block iteration.
    // Does not alter buffer structure, fully private to this method.
    // Tests with sigma1x2 half lanes vectorization show loss of ~10%.
    // Tests with sigma0x8 full lanes vectorization show gain of ~5%.
    constexpr auto r15 = Round - 15;
    const auto xsigma0 = sigma0_8<to_extended<word_t, 8>>(
        buffer[r15 + 0], buffer[r15 + 1], buffer[r15 + 2], buffer[r15 + 3],
        buffer[r15 + 4], buffer[r15 + 5], buffer[r15 + 6], buffer[r15 + 7]);

    prepare1<Round, 0>(buffer, xsigma0);
    prepare1<Round, 1>(buffer, xsigma0);
    prepare1<Round, 2>(buffer, xsigma0);
    prepare1<Round, 3>(buffer, xsigma0);
    prepare1<Round, 4>(buffer, xsigma0);
    prepare1<Round, 5>(buffer, xsigma0);
    prepare1<Round, 6>(buffer, xsigma0);
    prepare1<Round, 7>(buffer, xsigma0);
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
    if constexpr (SHA::strength != 160 && have_lanes<word_t, 8>())
    {
        prepare8<16>(buffer);
        prepare8<24>(buffer);
        prepare8<32>(buffer);
        prepare8<40>(buffer);
        prepare8<48>(buffer);
        prepare8<56>(buffer);

        if constexpr (SHA::rounds == 80)
        {
            prepare8<64>(buffer);
            prepare8<72>(buffer);
        }

        konstant(buffer);
    }
    else
    {
        schedule_(buffer);
    }
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif
