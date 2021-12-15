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
#ifndef LIBBITCOIN_SYSTEM_DATA_DATA_HPP
#define LIBBITCOIN_SYSTEM_DATA_DATA_HPP

#include <cstddef>
#include <cstdint>
#include <bitcoin/system/data/collection.hpp>
#include <bitcoin/system/data/data_array.hpp>
#include <bitcoin/system/data/data_chunk.hpp>
#include <bitcoin/system/data/data_reference.hpp>
#include <bitcoin/system/data/data_slab.hpp>
#include <bitcoin/system/data/data_slice.hpp>
#include <bitcoin/system/data/set.hpp>
#include <bitcoin/system/data/string.hpp>
#include <bitcoin/system/data/uintx.hpp>

#include <memory>
#include <utility>

/// Create empty shared pointer to const.
template <typename Type>
inline std::shared_ptr<Type> to_shared()
{
    return std::make_shared<Type>();
}

/// Create shared pointer to const from the moved instance.
template <typename Type>
inline std::shared_ptr<const Type> to_shared(Type&& value)
{
    return std::make_shared<const Type>(std::forward<Type>(value));
}

/// Create shared pointer to const from the copied instance.
template <typename Type>
inline std::shared_ptr<const Type> to_shared(const Type& value)
{
    return std::make_shared<const Type>(value);
}

/// Create shared pointer to vector of const shared ptr from the moved vector.
template <typename Type>
std::shared_ptr<std::vector<std::shared_ptr<const Type>>> to_shareds(
    std::vector<Type>&& values)
{
    auto out = std::make_shared<std::vector<std::shared_ptr<const Type>>>(
        values.size());

    std::transform(values.begin(), values.end(), out->begin(),
        [](Type&& value)
        {
            // TODO: this may not be forwarding.
            return to_shared(std::forward<Type>(value));
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

/// Cast pointer to vector of shared pointers to that of shared const pointers.
template <typename Type>
std::shared_ptr<std::vector<std::shared_ptr<const Type>>>
to_const(const std::shared_ptr<std::vector<std::shared_ptr<Type>>>& pointers)
{
    auto out = std::make_shared<std::vector<std::shared_ptr<const Type>>>(
        pointers->size());

    std::transform(pointers->begin(), pointers->end(), out->begin(),
        [](const std::shared_ptr<Type>& pointer)
        {
            return std::const_pointer_cast<const Type>(pointer);
        });

    return out;
}

#endif
