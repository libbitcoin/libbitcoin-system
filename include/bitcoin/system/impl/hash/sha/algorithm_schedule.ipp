/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_SCHEDULE_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_SCHEDULE_IPP


// Message scheduling.
// ============================================================================

namespace libbitcoin {
namespace system {
namespace sha {

// protected
// ----------------------------------------------------------------------------

TEMPLATE
template<size_t Round>
INLINE constexpr void CLASS::
prepare(auto& buffer) NOEXCEPT
{
    constexpr auto s = SHA::word_bits;

    if constexpr (SHA::strength == 160)
    {
        buffer[Round] = f::rol<1, s>(f::xor_(
            f::xor_(buffer[Round - 16], buffer[Round - 14]),
            f::xor_(buffer[Round -  8], buffer[Round -  3])));
    }
    else
    {
        buffer[Round] = f::add<s>(
            f::add<s>(buffer[Round - 16], sigma0(buffer[Round - 15])),
            f::add<s>(buffer[Round -  7], sigma1(buffer[Round -  2])));
    }
}

TEMPLATE
constexpr void CLASS::
schedule_(auto& buffer) NOEXCEPT
{
    prepare<16>(buffer);
    prepare<17>(buffer);
    prepare<18>(buffer);
    prepare<19>(buffer);
    prepare<20>(buffer);
    prepare<21>(buffer);
    prepare<22>(buffer);
    prepare<23>(buffer);
    prepare<24>(buffer);
    prepare<25>(buffer);
    prepare<26>(buffer);
    prepare<27>(buffer);
    prepare<28>(buffer);
    prepare<29>(buffer);
    prepare<30>(buffer);
    prepare<31>(buffer);

    prepare<32>(buffer);
    prepare<33>(buffer);
    prepare<34>(buffer);
    prepare<35>(buffer);
    prepare<36>(buffer);
    prepare<37>(buffer);
    prepare<38>(buffer);
    prepare<39>(buffer);
    prepare<40>(buffer);
    prepare<41>(buffer);
    prepare<42>(buffer);
    prepare<43>(buffer);
    prepare<44>(buffer);
    prepare<45>(buffer);
    prepare<46>(buffer);
    prepare<47>(buffer);

    prepare<48>(buffer);
    prepare<49>(buffer);
    prepare<50>(buffer);
    prepare<51>(buffer);
    prepare<52>(buffer);
    prepare<53>(buffer);
    prepare<54>(buffer);
    prepare<55>(buffer);
    prepare<56>(buffer);
    prepare<57>(buffer);
    prepare<58>(buffer);
    prepare<59>(buffer);
    prepare<60>(buffer);
    prepare<61>(buffer);
    prepare<62>(buffer);
    prepare<63>(buffer);

    if constexpr (SHA::rounds == 80)
    {
        prepare<64>(buffer);
        prepare<65>(buffer);
        prepare<66>(buffer);
        prepare<67>(buffer);
        prepare<68>(buffer);
        prepare<69>(buffer);
        prepare<70>(buffer);
        prepare<71>(buffer);
        prepare<72>(buffer);
        prepare<73>(buffer);
        prepare<74>(buffer);
        prepare<75>(buffer);
        prepare<76>(buffer);
        prepare<77>(buffer);
        prepare<78>(buffer);
        prepare<79>(buffer);
    }

    konstant(buffer);
}

TEMPLATE
constexpr void CLASS::
schedule(buffer_t& buffer) NOEXCEPT
{
    if (std::is_constant_evaluated())
    {
        schedule_(buffer);
    }
    else if constexpr (vector)
    {
        // Single block (without shani) message scheduling optimization.
        schedule_sigma(buffer);
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
