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

#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

DEFINE_JSON_TO_TAG(point)
{
    return
    {
        decode_hash<hash_size>(value.at("hash").as_string()),
        value.at("index").to_number<uint32_t>()
    };
}

DEFINE_JSON_FROM_TAG(point)
{
    value =
    {
        { "hash", encode_hash(instance.hash()) },
        { "index", instance.index() }
    };
}

DEFINE_JSON_TO_TAG(point::cptr)
{
    return to_shared(tag_invoke(to_tag<point>{}, value));
}

DEFINE_JSON_FROM_TAG(point::cptr)
{
    tag_invoke(from_tag{}, value, *instance);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
