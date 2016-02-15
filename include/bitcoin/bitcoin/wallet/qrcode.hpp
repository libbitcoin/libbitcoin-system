/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_QRENCODE_HPP
#define LIBBITCOIN_QRENCODE_HPP

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

#ifdef WITH_QRENCODE
#include "qrencode.h"
#endif


namespace libbitcoin {
namespace wallet {

#ifdef WITH_QRENCODE

typedef uint32_t qr_version;
typedef QRecLevel qr_rec_level;
typedef QRencodeMode qr_encode_mode;
typedef uint32_t qr_case_sensitive;

namespace qrcode {

BC_CONSTEXPR qr_version version = 0;
BC_CONSTEXPR qr_rec_level level = QR_ECLEVEL_L;
BC_CONSTEXPR qr_encode_mode hint = QR_MODE_8;
BC_CONSTEXPR qr_case_sensitive case_sensitive = 1;

} // namespace qrcode

/**
 * A method that takes an input stream and writes the encoded qr data
 * to the specified output stream with default parameter values.
 */
BC_API bool qrencode_data(std::istream& in, std::ostream& out);

/**
 * A method that takes a data chunk and returns the encoded qr data as
 * a data_chunk with default parameter values.
 */
BC_API data_chunk qrencode_data(const data_chunk& data);

/**
 * A method that takes a data chunk and returns the encoded qr data as
 * a data chunk with the specified parameter values.
 */
BC_API data_chunk qrencode_data(const data_chunk& data,
    const qr_version version, const qr_rec_level level,
    const qr_encode_mode hint, const qr_case_sensitive cs);

/**
 * A method that takes an input stream and writes the encoded qr data
 * to the output stream with the specified parameter values.
 */
BC_API bool qrencode_data(std::istream& in, const qr_version version,
    const qr_rec_level level, const qr_encode_mode hint,
    const qr_case_sensitive cs, std::ostream& out);

#endif

} // namespace wallet
} // namespace libbitcoin

#endif

