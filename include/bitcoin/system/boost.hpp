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
#ifndef LIBBITCOIN_SYSTEM_BOOST_HPP
#define LIBBITCOIN_SYSTEM_BOOST_HPP

#include <utility>

// Apply any warning suppressions to boost.
// Any boost includes within headers will not benefit from suppression, as the
// warnings are included by define.hpp which follows boost includes.
#include <bitcoin/system/warnings.hpp>

// Include boost only from here, so this exclusion works.
// Avoid namespace conflict between boost::placeholders and std::placeholders.
// This arises when including <functional>, which declares std::placeholders.
// www.boost.org/doc/libs/1_78_0/boost/bind.hpp
// Include boost only from here, so placeholders exclusion works.
#define BOOST_BIND_NO_PLACEHOLDERS
#include <boost/bind.hpp>

#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/json.hpp>
#include <boost/locale.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/program_options.hpp>
#include <boost/url.hpp>

// Declares ADL free functions for use with boost-json.
#define DECLARE_JSON_TAG_INVOKE(type) \
BC_API type tag_invoke(to_tag<type>, const boost::json::value& value) THROWS; \
BC_API void tag_invoke(from_tag, boost::json::value& value, \
    const type& instance) THROWS

// Define ADL free functions for use with boost-json.
#define DEFINE_JSON_TO_TAG(type) \
    type tag_invoke(to_tag<type>, const boost::json::value& value) THROWS
#define DEFINE_JSON_FROM_TAG(type) \
    BC_PUSH_WARNING(SMART_PTR_NOT_NEEDED) \
    BC_PUSH_WARNING(NO_VALUE_OR_CONST_REF_SHARED_PTR) \
    void tag_invoke(from_tag, boost::json::value& value, \
        const type& instance) THROWS \
    BC_POP_WARNING() \
    BC_POP_WARNING()

namespace libbitcoin {
namespace system {
namespace asio {
/// asio aliases
typedef boost::asio::ip::address address;
typedef boost::asio::ip::tcp::endpoint endpoint;
} // asio

} // system

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

/// program_options aliases
typedef boost::program_options::variables_map variables_map;
typedef boost::program_options::option_description option_metadata;
typedef boost::program_options::options_description options_metadata;
typedef boost::program_options::positional_options_description arguments_metadata;
} // namespace libbitcoin

#endif
