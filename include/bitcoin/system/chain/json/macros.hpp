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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_JSON_MACROS_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_JSON_MACROS_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {

/// Tags for alternative encodings.
struct native_tag {};
struct bitcoind_tag {};
struct bitcoind_hashed_tag {};
struct bitcoind_verbose_tag {};
struct bitcoind_embedded_tag {};
struct electrumx_tag {};

/// Reference wrapper.
template<class Tag, class Type>
struct wrapped
{
    const Type& value;
    explicit wrapped(const Type& value) NOEXCEPT
      : value(value)
    {
    }
};

/// Convenience aliases.
template<class Type>
inline auto native(const Type& value) NOEXCEPT
{
    return wrapped<native_tag, Type>{ value };
}
template<class Type>
inline auto bitcoind(const Type& value) NOEXCEPT
{
    return wrapped<bitcoind_tag, Type>{ value };
}
template<class Type>
inline auto bitcoind_hashed(const Type& value) NOEXCEPT
{
    return wrapped<bitcoind_hashed_tag, Type>{ value };
}
template<class Type>
inline auto bitcoind_verbose(const Type& value) NOEXCEPT
{
    return wrapped<bitcoind_verbose_tag, Type>{ value };
}
template<class Type>
inline auto bitcoind_embedded(const Type& value) NOEXCEPT
{
    return wrapped<bitcoind_embedded_tag, Type>{ value };
}
template<class Type>
inline auto electrumx(const Type& value) NOEXCEPT
{
    return wrapped<electrumx_tag, Type>{ value };
}

/// json aliases
template <typename Value, typename ...Args>
inline auto value_to(Args&&... args) THROWS
{
    return boost::json::value_to<Value>(std::forward<Args>(args)...);
}
template <typename ...Args>
inline auto value_from(Args&&... args) THROWS
{
    return boost::json::value_from(std::forward<Args>(args)...);
}
template <typename Type>
using to_tag = boost::json::value_to_tag<Type>;
typedef boost::json::value_from_tag from_tag;

} // namespace libbitcoin

// Declares ADL free functions for use with boost-json (native/tagged).
#define DECLARE_JSON_TAG_INVOKE(type) \
    BC_API type tag_invoke(to_tag<type>, \
        const boost::json::value& value) THROWS; \
    BC_API void tag_invoke(from_tag, boost::json::value& value, \
        const type& instance) THROWS

#define DECLARE_JSON_TAGGED_INVOKE(tag, type) \
    BC_API void tag_invoke(from_tag, boost::json::value& value, \
        const libbitcoin::wrapped<tag, type>& instance) THROWS

// Defines ADL free from functions for use with boost-json (native/tagged).
#define DEFINE_JSON_FROM_TAG(type) \
    void tag_invoke(from_tag, boost::json::value& value, \
        const type& instance) THROWS

#define DEFINE_JSON_FROM_TAGGED(tag, type) \
    void tag_invoke(from_tag, boost::json::value& value, \
        const libbitcoin::wrapped<tag, type>& instance) THROWS

// Defines ADL free to functions for use with boost-json (native/tagged).
#define DEFINE_JSON_TO_TAG(type) \
    type tag_invoke(to_tag<type>, const boost::json::value& value) THROWS

#define DEFINE_JSON_TO_TAGGED(tag, type) \
    type tag_invoke(to_tag<libbitcoin::wrapped<tag, type>>, \
        const boost::json::value& value) THROWS

#endif
