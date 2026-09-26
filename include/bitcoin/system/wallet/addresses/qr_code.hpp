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
#ifndef LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_QR_CODE_HPP
#define LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_QR_CODE_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/wallet/addresses/qr_encoder.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

class BC_API qr_code
{
public:
    using encode_mode = qr_encoder::encode_mode;
    using recovery_level = qr_encoder::recovery_level;

    static constexpr uint8_t maximum_version = qr_encoder::maximum_version;

    /// Masked modules, row major, one byte per module (0x01 is dark).
    /// Empty on failure, otherwise width is the symbol width in modules.
    static data_chunk to_modules(size_t& width, const std::string& value,
        uint8_t version=0, recovery_level level=recovery_level::low,
        encode_mode mode=encode_mode::eight_bit,
        bool case_sensitive=true) NOEXCEPT;

    /// False if version > maximum_version or size > tiff::max_image_bytes.
    /// Create a TIFF formatter QR code representing the given string value.
    static bool encode(std::ostream& out, const std::string& value,
        uint8_t version=0, uint16_t scale=8, uint16_t margin=2,
        recovery_level level=recovery_level::low,
        encode_mode mode=encode_mode::eight_bit,
        bool case_sensitive=true) NOEXCEPT;

protected:
    /// Convert QR encoded data stream to bit stream with margin and scaling.
    static data_chunk to_pixels(const data_chunk& coded, uint32_t width_coded,
        uint16_t scale=8, uint16_t margin=2) NOEXCEPT;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
