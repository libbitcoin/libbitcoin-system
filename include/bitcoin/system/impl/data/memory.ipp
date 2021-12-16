/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_DATA_MEMORY_IPP
#define LIBBITCOIN_SYSTEM_DATA_MEMORY_IPP

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

namespace libbitcoin {
namespace system {

/// Create shared pointer to vector of const shared ptr from the moved vector.
template <typename Type>
std::shared_ptr<std::vector<std::shared_ptr<const Type>>> to_shareds(
    std::vector<Type>&& values)
{
    auto out = std::make_shared<std::vector<std::shared_ptr<const Type>>>(
        values.size());

    std::transform(values.begin(), values.end(), out->begin(),
        [](Type& value)
    {
        return to_shared(std::move(value));
    });

    return out;
}

/// Create shared pointer to vector of const shared ptr from the copied vector.
template <typename Type>
std::shared_ptr<std::vector<std::shared_ptr<const Type>>> to_shareds(
    const std::vector<Type>& values)
{
    auto out = std::make_shared<std::vector<std::shared_ptr<const Type>>>(
        values.size());

    std::transform(values.begin(), values.end(), out->begin(),
        [](const Type& value)
    {
        return to_shared(value);
    });

    return out;
}

} // namespace system
} // namespace libbitcoin

#endif
