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
#ifndef LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_QR_ENCODER_HPP
#define LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_QR_ENCODER_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// QR code (ISO/IEC 18004) codeword encoder.
class BC_API qr_encoder
{
public:
    static constexpr uint8_t minimum_version = 1;
    static constexpr uint8_t maximum_version = 40;

    enum class recovery_level
    {
        low = 0,
        medium,
        high,
        highest
    };

    enum class encode_mode
    {
        eight_bit = 2,
        kanji = 3
    };

    /// Total codewords (data and error correction) of the version.
    static size_t codewords(uint8_t version) NOEXCEPT;

    /// Data codewords of the version at the recovery level.
    static size_t data_codewords(uint8_t version, recovery_level level) NOEXCEPT;

    /// Interleaved data and error correction codewords, empty on failure.
    /// Version is the minimum on input (zero for any) and actual on output.
    static data_chunk encode(uint8_t& version, const std::string& value,
        recovery_level level=recovery_level::low,
        encode_mode mode=encode_mode::eight_bit,
        bool case_sensitive=true) NOEXCEPT;

protected:
    enum class segment_mode
    {
        numeric = 0,
        alpha_numeric,
        eight_bit,
        kanji,
        end
    };

    struct segment
    {
        segment_mode mode;
        size_t start;
        size_t size;

        bool operator==(const segment& other) const NOEXCEPT = default;
    };

    using segments = std_vector<segment>;

    /// Specification.
    static size_t error_codewords(uint8_t version,
        recovery_level level) NOEXCEPT;
    static size_t blocks(uint8_t version, recovery_level level) NOEXCEPT;
    static size_t length_bits(segment_mode mode, uint8_t version) NOEXCEPT;
    static size_t payload_bits(segment_mode mode, size_t size) NOEXCEPT;
    static uint8_t minimum_fit(size_t bits, recovery_level level) NOEXCEPT;

    /// Segmentation.
    static segment_mode classify(const std::string& value, size_t index,
        bool kanji) NOEXCEPT;
    static std::string to_upper(const std::string& value, bool kanji) NOEXCEPT;
    static segments split(const std::string& value, uint8_t version,
        bool kanji) NOEXCEPT;

    /// Version selection.
    static size_t estimate_bits(const segments& parts, uint8_t version) NOEXCEPT;
    static size_t stream_bits(const segments& parts, uint8_t version) NOEXCEPT;
    static uint8_t select_version(const segments& parts, uint8_t version,
        recovery_level level) NOEXCEPT;

    /// Codewords.
    static data_chunk to_data(const std::string& value, const segments& parts,
        uint8_t version, recovery_level level) NOEXCEPT;
    static data_chunk to_error(const data_slice& data, size_t size) NOEXCEPT;
    static data_chunk interleave(const data_chunk& data, uint8_t version,
        recovery_level level) NOEXCEPT;

private:
    static size_t eat_numeric(segments& out, const std::string& value,
        size_t start, uint8_t version, bool kanji) NOEXCEPT;
    static size_t eat_alpha_numeric(segments& out, const std::string& value,
        size_t start, uint8_t version, bool kanji) NOEXCEPT;
    static size_t eat_kanji(segments& out, const std::string& value,
        size_t start, bool kanji) NOEXCEPT;
    static size_t eat_eight_bit(segments& out, const std::string& value,
        size_t start, uint8_t version, bool kanji) NOEXCEPT;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
