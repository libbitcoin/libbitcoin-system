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
#ifndef LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_SILENT_PAYMENT_HPP
#define LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_SILENT_PAYMENT_HPP

#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// BIP352 silent payment receiving (prevouts summary and output scanning).
class BC_API silent_payment
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(silent_payment);

    /// A label value that denotes an unlabeled match.
    static constexpr uint32_t unlabeled = max_uint32;

    /// Pay-to-taproot output of a transaction (index and x-only key).
    struct scan_output
    {
        uint32_t index{};
        ec_xonly key{};
    };

    using scan_outputs = std_vector<scan_output>;

    /// Output found by a scan, with its spend key tweak and label.
    struct scan_match
    {
        bool operator<(const scan_match& other) const NOEXCEPT;

        uint32_t index{};
        ec_secret tweak{};
        uint32_t label{ unlabeled };
    };

    using scan_matches = std_vector<scan_match>;

    /// Serialized outpoint.
    using outpoint = data_array<chain::point::serialized_size()>;

    /// The transaction's prevouts summary (input hash times the sum of its
    /// eligible input keys), false if the transaction is not eligible.
    /// Requires populated prevouts.
    static bool summarize(ec_compressed& out,
        const chain::transaction& tx) NOEXCEPT;
    static bool summarize(ec_compressed& out,
        const chain::view::transaction& tx) NOEXCEPT;

    /// The transaction's pay-to-taproot outputs, false if there are none.
    static bool get_outputs(scan_outputs& out,
        const chain::transaction& tx) NOEXCEPT;
    static bool get_outputs(scan_outputs& out,
        const chain::view::transaction& tx) NOEXCEPT;

    /// The address of the scan and spend keys under the prefix.
    static std::string to_address(const ec_compressed& scan_key,
        const ec_compressed& spend_key, const std::string& prefix) NOEXCEPT;

    /// Scanner for the given scan secret, spend key and labels.
    silent_payment(const ec_secret& scan_secret,
        const ec_compressed& spend_key,
        const std_vector<uint32_t>& labels) NOEXCEPT;

    /// False if the scan secret, spend key or a label is invalid.
    operator bool() const NOEXCEPT;

    /// Find all outputs paying the receiver (one transaction, all k).
    bool scan(scan_matches& out, const ec_compressed& summary,
        const scan_outputs& outputs) const NOEXCEPT;

    /// Determine if any output pays the receiver (one transaction, k = 0).
    bool match(bool& out, const ec_compressed& summary,
        const scan_outputs& outputs) const NOEXCEPT;

protected:
    /// Maximum number of outputs considered for one receiver (k bound).
    static constexpr size_t maximum_outputs = 2323;

    /// BIP341 NUMS point H (unspendable internal key).
    static constexpr ec_xonly nums_key = base16_array(
        "50929b74c1a04954b78b4b6035e97a5e078a5a0f28ec96d547bfee9ace803ac0");

    /// The prevouts summary of a non-coinbase transaction's inputs.
    template <typename Iterator>
    static bool summarize(ec_compressed& out, const Iterator& begin,
        const Iterator& end) NOEXCEPT;

    /// BIP352 tagged hashes.
    static bool input_hash(ec_secret& out, const outpoint& smallest,
        const ec_compressed& sum) NOEXCEPT;
    static bool shared_tweak(ec_secret& out, const ec_compressed& shared,
        uint32_t k) NOEXCEPT;
    static bool label_tweak(ec_secret& out, const ec_secret& scan_secret,
        uint32_t label) NOEXCEPT;

    /// The public key contributed by an input, false if none.
    template <typename Input>
    static bool get_input_key(ec_compressed& out, const Input& input) NOEXCEPT;
    template <typename Input>
    static bool get_key_hash_key(ec_compressed& out,
        const Input& input) NOEXCEPT;
    template <typename Input>
    static bool get_taproot_key(ec_compressed& out,
        const Input& input) NOEXCEPT;
    static bool get_witness_key(ec_compressed& out,
        const chain::witness& witness) NOEXCEPT;
    static bool is_nums_spend(const chain::witness& witness) NOEXCEPT;

    /// The output with the key that is not yet matched, false if none.
    static bool find_output(uint32_t& out, const scan_outputs& outputs,
        const ec_xonly& key, const scan_matches& matches) NOEXCEPT;

private:
    bool shared_secret(ec_compressed& out,
        const ec_compressed& summary) const NOEXCEPT;

    ec_secret scan_secret_{};
    ec_uncompressed spend_key_{};
    std_vector<uint32_t> labels_{};
    ec_secrets label_tweaks_{};
    ec_uncompresseds label_keys_{};
    bool valid_{};
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
