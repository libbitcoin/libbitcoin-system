/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_QR_CODE_HPP
#define LIBBITCOIN_SYSTEM_QR_CODE_HPP

#include <cstdint>
#include <iostream>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/utility/data.hpp>

namespace libbitcoin {
namespace system {

class BC_API qr_code
{
public:
    enum class encode_mode
    {
        numeric = 0,
        alpha_numeric,
        eight_bit,
        kanji,
        unused,
        eci_mode,
        fcn1_1,
        fcn1_2
    };

    enum class recovery_level
    {
        low = 0,
        medium,
        high,
        highest
    };

    /// Create a TIFF qrcode representing the given string value.
    static bool encode(std::ostream& out, const std::string& value,
        uint32_t version=0, uint16_t scale=8, uint16_t margin=2,
        bool case_sensitive=true, recovery_level level=recovery_level::low,
        encode_mode mode=encode_mode::eight_bit);

    /// Convert qr encoded data stream to bit stream with margin and scaling.
    static data_chunk to_image_data(const data_chunk& coded, uint16_t scale=8,
        uint16_t margin=2);
};

} // namespace system
} // namespace libbitcoin

#endif
