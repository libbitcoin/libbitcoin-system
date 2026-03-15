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

#include <bitcoin/system/chain/witness.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

DEFINE_JSON_TO_TAG(witness)
{
    return witness{ value.as_string()  };
}

DEFINE_JSON_FROM_TAG(witness)
{
    value = instance.to_string();
}

DEFINE_JSON_TO_TAG(witness::cptr)
{
    return to_shared(tag_invoke(to_tag<witness>{}, value));
}

DEFINE_JSON_FROM_TAG(witness::cptr)
{
    tag_invoke(from_tag{}, value, *instance);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
