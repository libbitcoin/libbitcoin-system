/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_ERROR_JSON_ERROR_T_HPP
#define LIBBITCOIN_SYSTEM_ERROR_JSON_ERROR_T_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/macros.hpp>

namespace libbitcoin {
namespace system {
namespace error {

enum json_error_t : uint8_t
{
    json_success = 0,

    // parse
    json_syntax,
    json_extra_data,
    json_incomplete,
    json_too_deep,
    json_invalid_escape,
    json_expected_hex_digit,
    json_expected_utf16_escape,
    json_illegal_leading_surrogate,
    json_illegal_trailing_surrogate,
    json_invalid_utf8,
    json_exponent_overflow,
    json_number_too_large,
    json_string_too_large,
    json_key_too_large,
    json_array_too_large,
    json_object_too_large,

    // access
    json_not_null,
    json_not_bool,
    json_not_number,
    json_not_int64,
    json_not_uint64,
    json_not_double,
    json_not_string,
    json_not_array,
    json_not_object,
    json_not_exact,
    json_out_of_range,
    json_size_mismatch,
    json_exhausted_variants
};

DECLARE_ERROR_T_CODE_CATEGORY(json_error);

} // namespace error
} // namespace system
} // namespace libbitcoin

DECLARE_STD_ERROR_REGISTRATION(bc::system::error::json_error)

#endif
