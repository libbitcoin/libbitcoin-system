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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_STACK_VARIANT_IPP
#define LIBBITCOIN_SYSTEM_MACHINE_STACK_VARIANT_IPP

#include <tuple>
#include <utility>
#include <variant>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

// private
// Generalized integer peek for varying bit widths up to 8 bytes.
// Chunk to integer conversions are constrained by caller (4 or 5 bytes).
// False result implies value overflow, from integer or chunk conversion.
TEMPLATE
template<size_t Bytes, typename Integer,
    if_not_lesser<sizeof(Integer), Bytes>,
    if_signed_integral_integer<Integer>>
inline bool CLASS::
peek_signed(Integer& value) const NOEXCEPT
{
    using namespace number;
    auto result{ true };

    std::visit(overload
    {
        [&](bool vary) NOEXCEPT
        {
            // This is never executed in standard scripts.
            value = to_int(vary);
        },
        [&](int64_t vary) NOEXCEPT
        {
            // This is the canonical use case.
            result = integer<Bytes>::from_integer(value, vary);
        },
        [&](const chunk_xptr& vary) NOEXCEPT
        {
            // This is never executed in standard scripts.
            result = integer<Bytes>::from_chunk(value, *vary);
        }
    }, top());

    return result;
}

/// Variant data conversions.
/// ---------------------------------------------------------------------------
/// P2SH and P2WSH redeem scripts and P2TR tapscripts are not subjectable to
/// standardness and are therefore not considered "stadnard" in comments below.

TEMPLATE
inline bool CLASS::
peek_bool() const NOEXCEPT
{
    using namespace number;
    bool value{};

    std::visit(overload
    {
        [&](bool vary) NOEXCEPT
        {
            // This is the canonical use case.
            value = vary;
        },
        [&](int64_t vary) NOEXCEPT
        {
            // This is never executed in standard scripts.
            value = boolean::from_integer(vary);
        },
        [&](const chunk_xptr& vary) NOEXCEPT
        {
            // This is never executed in standard scripts.
            value = boolean::from_chunk(*vary);
        }
    }, top());

    return value;
}

// Differs from peek_bool in that a chunk false must be empty [].
TEMPLATE
inline bool CLASS::
peek_strict_bool() const NOEXCEPT
{
    using namespace number;
    bool value{};

    std::visit(overload
    {
        [&](bool vary) NOEXCEPT
        {
            // This is the canonical use case (with bip147).
            value = vary;
        },
        [&](int64_t vary) NOEXCEPT
        {
            // This may be executed in standard scripts (without bip147).
            value = boolean::from_integer(vary);
        },
        [&](const chunk_xptr& vary) NOEXCEPT
        {
            // This may be executed in standard scripts (without bip147).
            value = boolean::from_chunk_strict(*vary);
        }
    }, top());

    return value;
}

// This is the only convertor with both a return value and a result.
// A chunk false must be [] and chunk true must be [0x01], otherwise fails.
TEMPLATE
inline bool CLASS::
peek_minimal_bool(bool& value) const NOEXCEPT
{
    using namespace number;
    auto result{ true };

    std::visit(overload
    {
        [&](bool vary) NOEXCEPT
        {
            // This is the canonical use case (tapscript).
            value = vary;
        },
        [&](int64_t vary) NOEXCEPT
        {
            // This may be executed in tapscripts.
            result = boolean::from_integer(value, vary);
        },
        [&](const chunk_xptr& vary) NOEXCEPT
        {
            // This may be executed in tapscripts.
            result = boolean::from_chunk(value, *vary);
        }
    }, top());

    return result;
}

// Could use peek_chunk but this overload skips allocation and tethering.
TEMPLATE
inline size_t CLASS::
peek_size() const NOEXCEPT
{
    using namespace number;
    size_t value{};

    std::visit(overload
    {
        [&](bool vary) NOEXCEPT
        {
            // This is never executed in standard scripts.
            value = chunk::from_bool(vary).size();
        },
        [&](int64_t vary) NOEXCEPT
        {
            // This is never executed in standard scripts.
            value = chunk::from_integer(vary).size();
        },
        [&](const chunk_xptr& vary) NOEXCEPT
        {
            // This is never executed in standard scripts.
            value = vary->size();
        }
    }, top());

    return value;
}

// This is the only source of peek/pop (read) tethering (make_external).
TEMPLATE
inline chunk_xptr CLASS::
peek_chunk() const NOEXCEPT
{
    using namespace number;
    chunk_xptr value{};

    std::visit(overload
    {
        [&](bool vary) NOEXCEPT
        {
            // This is never executed in standard scripts.
            value = make_external(chunk::from_bool(vary), tether_);
        },
        [&](int64_t vary) NOEXCEPT
        {
            // This is never executed in standard scripts.
            value = make_external(chunk::from_integer(vary), tether_);
        },
        [&](const chunk_xptr& vary) NOEXCEPT
        {
            // This is the canonical use case.
            value = vary;
        }
    }, top());

    return value;
}

// Static variant compare with conversion.
// Methods bound at compile time (free).
// One runtime switch on variant index (cheap).
// bool/int conversions are compile-time (free).
// chunk conversions reduce to conventional bitcoin design.
// Integers are unconstrained as these are stack chunk equality comparisons.
TEMPLATE
inline bool CLASS::
equal_chunks(const stack_variant& left, const stack_variant& right) NOEXCEPT
{
    using namespace number;
    auto same{ true };

    std::visit(overload
    {
        // This is never executed in standard scripts.
        [&](bool vary) NOEXCEPT
        {
            switch (right.index())
            {
                case stack_type::bool_:
                    same = std::get<bool>(right) == vary;
                    break;
                case stack_type::int64_:
                    same = std::get<int64_t>(right) == to_int(vary);
                    break;
                case stack_type::pchunk_:
                    same = *std::get<chunk_xptr>(right) == chunk::from_bool(vary);
            }
        },

        // This is never executed in standard scripts.
        [&](int64_t vary) NOEXCEPT
        {
            switch (right.index())
            {
                case stack_type::bool_:
                    same = to_int(std::get<bool>(right)) == vary;
                    break;
                case stack_type::int64_:
                    same = std::get<int64_t>(right) == vary;
                    break;
                case stack_type::pchunk_:
                    same = *std::get<chunk_xptr>(right) == chunk::from_integer(vary);
            }
        },

        // This is the canonical use case.
        [&](chunk_xptr vary) NOEXCEPT
        {
            switch (right.index())
            {
                case stack_type::bool_:
                    // This is never executed in standard scripts.
                    same = chunk::from_bool(std::get<bool>(right)) == *vary;
                    break;
                case stack_type::int64_:
                    // This is never executed in standard scripts.
                    same = chunk::from_integer(std::get<int64_t>(right)) == *vary;
                    break;
                case stack_type::pchunk_:
                    // This is the canonical use case.
                    same = *std::get<chunk_xptr>(right) == *vary;
            }
        }
    }, left);

    return same;
}

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif

