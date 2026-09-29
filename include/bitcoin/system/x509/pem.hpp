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
#ifndef LIBBITCOIN_SYSTEM_X509_PEM_HPP
#define LIBBITCOIN_SYSTEM_X509_PEM_HPP

#include <string>
#include <vector>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace x509 {

/// A textual encoding block (rfc7468): its label and decoded content.
struct pem
{
    std::string label{};
    data_chunk data{};
};

using pems = std::vector<pem>;

/// Encode the data as a block with the label, in 64 character lines.
BC_API std::string encode_pem(const std::string& label,
    const_byte_span data) NOEXCEPT;

/// Decode all blocks of the text, ignoring text between blocks.
/// False if any block is malformed or has encapsulated headers.
BC_API bool decode_pem(pems& out, const std::string& text) NOEXCEPT;

} // namespace x509
} // namespace system
} // namespace libbitcoin

#endif
