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

// Apply any warning suppressions to boost.
// Any boost includes within headers will not benefit from suppression, as the
// warnings are included by define.hpp which follows boost includes.
#include <bitcoin/system/warnings.hpp>

// Include boost only from here, so exception disable works.
#include <boost/format.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/json.hpp>
#include <boost/locale.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/program_options.hpp>

// ADL free functions for use with boost-json.
#define DECLARE_JSON_VALUE_CONVERTORS(name) \
BC_API name tag_invoke(boost::json::value_to_tag<name>, \
    const boost::json::value& value) NOEXCEPT; \
BC_API void tag_invoke(boost::json::value_from_tag, \
    boost::json::value& value, const name& instance) NOEXCEPT

namespace libbitcoin {

typedef boost::program_options::variables_map variables_map;
typedef boost::program_options::option_description option_metadata;
typedef boost::program_options::options_description options_metadata;
typedef boost::program_options::positional_options_description arguments_metadata;

} // namespace libbitcoin

#endif
