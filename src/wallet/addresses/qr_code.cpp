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
#include <bitcoin/system/wallet/addresses/qr_code.hpp>

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>
#include <bitcoin/system/wallet/addresses/qr_encoder.hpp>
#include <bitcoin/system/wallet/addresses/qr_matrix.hpp>
#include <bitcoin/system/wallet/addresses/tiff.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

data_chunk qr_code::to_modules(size_t& width, const std::string& value,
    uint8_t version, recovery_level level, encode_mode mode,
    bool case_sensitive) NOEXCEPT
{
    auto actual = version;
    const auto codewords = qr_encoder::encode(actual, value, level, mode,
        case_sensitive);

    auto modules = qr_matrix::encode(codewords, actual, level);
    width = modules.empty() ? zero : qr_matrix::width(actual);
    return modules;
}

bool qr_code::encode(std::ostream& out, const std::string& value,
    uint8_t version, uint16_t scale, uint16_t margin, recovery_level level,
    encode_mode mode, bool case_sensitive) NOEXCEPT
{
    size_t coded_width{};
    const auto modules = to_modules(coded_width, value, version, level, mode,
        case_sensitive);

    if (modules.empty())
        return false;

    // Bound: 2^1 * 2^16 + 2^16 * 2^8 < 2^64.
    const auto width = two * wide_cast<uint64_t>(margin) +
        wide_cast<uint64_t>(scale) * coded_width;

    // Guard: TIFF parameter overflow.
    if (width > max_uint16)
        return false;

    const auto pixels = to_pixels(modules,
        narrow_cast<uint32_t>(coded_width), scale, margin);

    return tiff::to_image(out, pixels, narrow_cast<uint16_t>(width));
}

// TODO: accept and return stream and split out scaling and margining.
// Scale may move the image off of a byte-aligned square of pixels in bytes.
// So the dimensions cannot be derived from the result, caller must retain.
// pixel_width = 2 * margin + scale * coded_width.
data_chunk qr_code::to_pixels(const data_chunk& coded, uint32_t width_coded,
    uint16_t scale, uint16_t margin) NOEXCEPT
{
    // Pixel is the least significant bit of a module byte.
    constexpr auto pixel_mask = uint8_t{ 0x01 };
    constexpr auto pixels_off = uint8_t{ 0x00 };
    constexpr auto pixel_off = false;

    // For readability (image is always square).
    const auto height_coded = width_coded;

    // Guard: mismatched sizes.
    if (is_multiply_overflow(width_coded, height_coded) ||
        coded.size() != width_coded * height_coded)
        return {};

    // Bound: 2^16 * 2^32 < 2^48 < 2^64.
    const auto width_scaled = wide_cast<uint64_t>(scale) * width_coded;

    // Bound: 2^48 + 2^1 * 2^16 < 2^48 + 2^17 < 2^1 * 2^48 < 2^64.
    const auto width_pixels = (margin + width_scaled + margin);

    // Guard: empty image and division by zero.
    if (is_zero(width_pixels))
        return {};

    // Guard: area overflow (all below limited to size_t).
    if (max_size_t / width_pixels < width_pixels)
        return {};

    // Cast guarded width and define height for readability.
    const auto width = possible_narrow_cast<size_t>(width_pixels);
    const auto height = possible_wide_cast<size_t>(height_coded);

    // Horizontal margins and full row copies can be done bytewise.
    const auto row_bytes = ceilinged_divide(width, byte_bits);
    const auto row_margin = data_chunk(row_bytes, pixels_off);

    // Bound: (2^16 - 1)^2 < 2^32 or (2^32 - 1)^2 < 2^64.
    const auto area_bytes = height * row_bytes;

    // For reading the qrcode byte stream.
    read::bytes::copy image_reader(coded);

    // For writing the image bit stream.
    data_chunk image_out;
    image_out.reserve(area_bytes);
    write::bits::data image_bit_sink(image_out);

    // ------------------------- Write top margin -------------------------
    for (size_t row{}; row < margin; ++row)
        image_bit_sink.write_bytes(row_margin);
    // --------------------------------------------------------------------

    // Write each row.
    for (size_t row{}; row < height_coded; ++row)
    {
        // For repeatedly writing a row buffer.
        data_chunk row_out;
        row_out.reserve(row_bytes);
        write::bits::data row_bit_sink(row_out);

        // ------------------------ Buffer left margin ------------------------
        for (size_t column{}; column < margin; ++column)
            row_bit_sink.write_bit(pixel_off);
        // --------------------------------------------------------------------

        // Buffer scaled row pixels.
        for (size_t column{}; column < width_coded; ++column)
        {
            // Read byte and extract pixel (least significant) bit.
            const auto pixel_on = !is_zero(bit_and(image_reader.read_byte(),
                pixel_mask));

            // Buffer scaled pixel.
            for (size_t scaled{}; scaled < scale; ++scaled)
                row_bit_sink.write_bit(pixel_on);
        }

        // ------------------------ Buffer right margin -----------------------
        for (size_t column{}; column < margin; ++column)
            row_bit_sink.write_bit(pixel_off);
        // --------------------------------------------------------------------

        // Flush to row_out.
        row_bit_sink.flush();

        // Write row buffer scale times.
        for (size_t scaled{}; scaled < scale; ++scaled)
            image_bit_sink.write_bytes(row_out);
    }

    // ------------------------ Write bottom margin -----------------------
    for (size_t row{}; row < margin; ++row)
        image_bit_sink.write_bytes(row_margin);
    // --------------------------------------------------------------------

    // Guard against writer failure and unexpected stream length.
    if (!image_bit_sink || !image_reader.is_exhausted())
        return {};

    // Flush to image_out.
    image_bit_sink.flush();
    return image_out;
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
