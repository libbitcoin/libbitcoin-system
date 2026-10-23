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
#include <bitcoin/system/error/json_error_t.hpp>

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/macros.hpp>

namespace libbitcoin {
namespace system {
namespace error {

DEFINE_ERROR_T_MESSAGE_MAP(json_error)
{
    { json_success, "json success" },

    // parse
    { json_syntax, "json syntax error" },
    { json_extra_data, "json extra data" },
    { json_incomplete, "json incomplete" },
    { json_too_deep, "json too deep" },
    { json_invalid_escape, "json invalid escape" },
    { json_expected_hex_digit, "json expected hex digit" },
    { json_expected_utf16_escape, "json expected utf16 escape" },
    { json_illegal_leading_surrogate, "json illegal leading surrogate" },
    { json_illegal_trailing_surrogate, "json illegal trailing surrogate" },
    { json_invalid_utf8, "json invalid utf8" },
    { json_exponent_overflow, "json exponent overflow" },
    { json_number_too_large, "json number too large" },
    { json_string_too_large, "json string too large" },
    { json_key_too_large, "json key too large" },
    { json_array_too_large, "json array too large" },
    { json_object_too_large, "json object too large" },

    // access
    { json_not_null, "json not null" },
    { json_not_bool, "json not bool" },
    { json_not_number, "json not number" },
    { json_not_int64, "json not int64" },
    { json_not_uint64, "json not uint64" },
    { json_not_double, "json not double" },
    { json_not_string, "json not string" },
    { json_not_array, "json not array" },
    { json_not_object, "json not object" },
    { json_not_exact, "json not exact" },
    { json_out_of_range, "json out of range" },
    { json_size_mismatch, "json size mismatch" },
    { json_exhausted_variants, "json exhausted variants" }
};

DEFINE_ERROR_T_CATEGORY(json_error, "json", "json code")

} // namespace error
} // namespace system
} // namespace libbitcoin
