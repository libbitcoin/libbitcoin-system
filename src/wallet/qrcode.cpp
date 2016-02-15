/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/wallet/qrcode.hpp>

#include <iostream>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace wallet {

#ifdef WITH_QRENCODE

data_chunk qrencode_data(const data_chunk& data)
{
    return qrencode_data(data, qrcode::version,
        qrcode::level, qrcode::hint, qrcode::case_sensitive);
}

data_chunk qrencode_data(const data_chunk& data, const qr_version version,
    const qr_rec_level level, const qr_encode_mode hint,
    const qr_case_sensitive cs)
{
    data_chunk out;
    data_sink ostream(out);
    data_source istream(data);

    if (qrencode_data(istream, version, level, hint, cs, ostream))
        return out;

    return data_chunk();
}

bool qrencode_data(std::istream& in, std::ostream& out)
{
    return qrencode_data(in, qrcode::version,
        qrcode::level, qrcode::hint, qrcode::case_sensitive, out);
}

bool qrencode_data(std::istream& in, const qr_version version,
    const qr_rec_level level, const qr_encode_mode hint,
    const qr_case_sensitive cs, std::ostream& out)
{
    std::string qr_string;
    getline(in, qr_string);

    QRcode* qrcode = QRcode_encodeString(qr_string.c_str(), version,
        level, hint, cs);
    if (qrcode)
    {
        // Write out raw format of QRcode structure (defined in
        // qrencode.h). Format written is:
        // uint32_t version
        // uint32_t width
        // unsigned char* data (of width * width length)
        ostream_writer sink(out);
        sink.write_data(reinterpret_cast<const uint8_t*>(&qrcode->version),
            sizeof(uint32_t));
        sink.write_data(reinterpret_cast<const uint8_t*>(&qrcode->width),
            sizeof(uint32_t));
        sink.write_data(qrcode->data, qrcode->width * qrcode->width);
        out.flush();
    }
    return (qrcode != nullptr);
}

#endif

} // namespace wallet
} // namespace libbitcoin
