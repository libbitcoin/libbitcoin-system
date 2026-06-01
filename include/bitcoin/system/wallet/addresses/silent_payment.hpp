/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_SILENT_PAYMENT_HPP
#define LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_SILENT_PAYMENT_HPP

#include <optional>
#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class transaction;

} // namespace chain
namespace wallet {
namespace silent_payment {

struct BC_API pay_witness_taproot_output
{
    uint32_t index{};
    ec_xonly key{};
};

struct BC_API scan_record
{
    ec_compressed prevouts_summary{};
    std_vector<pay_witness_taproot_output> outputs{};
};

struct BC_API scan_match
{
    uint32_t index{};
    ec_secret tweak{};
    std::optional<uint32_t> label{};

    bool operator<(const scan_match& other) const NOEXCEPT;
};

typedef std_vector<scan_match> scan_matches;

class BC_API scanner
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(scanner);

    scanner(const ec_secret& scan_secret, const ec_compressed& spend_public,
        const std_vector<uint32_t>& labels) NOEXCEPT;

    operator bool() const NOEXCEPT;

    /// Scan outputs from one transaction, with unique output indexes.
    bool scan(scan_matches& out, const ec_compressed& prevouts_summary,
        const std_vector<pay_witness_taproot_output>& outputs)
        const NOEXCEPT;
    bool scan(scan_matches& out, const ec_uncompressed& prevouts_summary,
        const std_vector<pay_witness_taproot_output>& outputs)
        const NOEXCEPT;

    /// Match outputs from one transaction, with unique output indexes.
    bool match(bool& out, const ec_compressed& prevouts_summary,
        const std_vector<pay_witness_taproot_output>& outputs)
        const NOEXCEPT;
    bool match(bool& out, const ec_uncompressed& prevouts_summary,
        const std_vector<pay_witness_taproot_output>& outputs)
        const NOEXCEPT;

private:
    bool valid() const NOEXCEPT;
    bool to_shared_secret(ec_compressed& out,
        const ec_compressed& prevouts_summary) const NOEXCEPT;
    bool to_shared_secret(ec_compressed& out,
        const ec_uncompressed& prevouts_summary) const NOEXCEPT;
    bool scan_shared(scan_matches& out, const ec_compressed& shared_secret,
        const std_vector<pay_witness_taproot_output>& outputs)
        const NOEXCEPT;
    bool match_shared(bool& out, const ec_compressed& shared_secret,
        const std_vector<pay_witness_taproot_output>& outputs)
        const NOEXCEPT;

    ec_secret scan_secret_{};
    ec_uncompressed spend_point_{};
    std_vector<uint32_t> labels_{};
    secret_list label_tweaks_{};
    uncompressed_list label_points_uncompressed_{};
    bool valid_{};
};

BC_API bool compute_scan_record(scan_record& out,
    const chain::transaction& tx) NOEXCEPT;

} // namespace silent_payment
} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
