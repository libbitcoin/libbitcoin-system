/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/chain/json/json.hpp>

#include <bitcoin/system/chain/checkpoint.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// hash_digest gets bypassed (up as an array of numbers) by boost.
////DEFINE_JSON_TO_TAG(hash_digest)
////{
////    hash_digest hash{};
////    if (decode_hash(hash, value.as_string()))
////        return hash;
////
////    return {};
////}
////
////DEFINE_JSON_FROM_TAG(hash_digest)
////{
////    value = encode_hash(instance);
////}

DEFINE_JSON_TO_TAG(checkpoint)
{
    return
    {
        decode_hash<hash_size>(value.at("hash").as_string()),
        value.at("height").to_number<size_t>()
    };
}

DEFINE_JSON_FROM_TAG(checkpoint)
{
    value =
    {
        { "hash", encode_hash(instance.hash()) },
        { "height", instance.height() }
    };
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
