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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_WITNESS_IPP
#define LIBBITCOIN_SYSTEM_CHAIN_WITNESS_IPP

#include <algorithm>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED)
BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR)

// static
constexpr bool witness::is_push_size(const chunk_cptrs& stack) NOEXCEPT
{
    return std::all_of(stack.begin(), stack.end(),
        [](const auto& element) NOEXCEPT
        {
            return element->size() <= max_push_data_size;
        });
}

// static
constexpr bool witness::is_reserved_pattern(const chunk_cptrs& stack) NOEXCEPT
{
    return is_one(stack.size()) && stack.front()->size() == hash_size;
}

// static/private
inline size_t witness::element_size(const chunk_cptr& element) NOEXCEPT
{
    // Each witness is prefixed with number of elements (bip144).
    const auto size = element->size();
    return ceilinged_add(variable_size(size), size);
};

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
