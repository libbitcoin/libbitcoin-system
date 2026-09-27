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
#ifndef LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_QR_MATRIX_HPP
#define LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_QR_MATRIX_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/wallet/addresses/qr_encoder.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// QR code (ISO/IEC 18004) module matrix.
class BC_API qr_matrix
{
public:
    using recovery_level = qr_encoder::recovery_level;

    /// Modules per side of the version (zero if version is invalid).
    static size_t width(uint8_t version) NOEXCEPT;

    /// Masked modules, row major, one byte per module (0x01 is dark).
    /// Empty if version is invalid or codewords are not of the version.
    static data_chunk encode(const data_chunk& codewords, uint8_t version,
        recovery_level level) NOEXCEPT;

protected:
    static constexpr uint8_t light = 0x00;
    static constexpr uint8_t dark = 0x01;
    static constexpr uint8_t reserved = 0x80;
    static constexpr size_t masks = 8;

    /// Function patterns.
    static data_chunk to_frame(uint8_t version) NOEXCEPT;
    static uint32_t format_bits(recovery_level level, size_t mask) NOEXCEPT;
    static uint32_t version_bits(uint8_t version) NOEXCEPT;

    /// Codeword placement.
    static void place(data_chunk& frame, size_t width,
        const data_chunk& codewords) NOEXCEPT;

    /// Masking.
    static bool is_masked(size_t mask, size_t row, size_t column) NOEXCEPT;
    static void apply_mask(data_chunk& out, const data_chunk& frame,
        size_t width, size_t mask) NOEXCEPT;
    static void write_format(data_chunk& frame, size_t width,
        recovery_level level, size_t mask) NOEXCEPT;
    static size_t penalty(const data_chunk& modules, size_t width) NOEXCEPT;
    static size_t penalty_runs(const std_vector<size_t>& runs) NOEXCEPT;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
