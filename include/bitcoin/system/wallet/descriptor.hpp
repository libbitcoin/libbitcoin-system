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
#ifndef LIBBITCOIN_SYSTEM_WALLET_DESCRIPTOR_HPP
#define LIBBITCOIN_SYSTEM_WALLET_DESCRIPTOR_HPP

#include <optional>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/wallet/keys/hd_private.hpp>
#include <bitcoin/system/wallet/keys/hd_public.hpp>
#include <bitcoin/system/wallet/psbt/entry.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// A bip380 output script descriptor. The script function set is pk, pkh,
/// wpkh, sh, wsh, multi, sortedmulti, combo, tr (key path), addr and raw.
class BC_API descriptor
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(descriptor);

    typedef std_vector<descriptor> list;

    /// The bip380 expression checksum (empty on invalid characters).
    static std::string to_checksum(const std::string& body) NOEXCEPT;

    /// Constructors.
    descriptor() NOEXCEPT;
    descriptor(const std::string& expression) NOEXCEPT;

    /// Operators.
    bool operator==(const descriptor& other) const NOEXCEPT;
    bool operator!=(const descriptor& other) const NOEXCEPT;

    /// Cast operators.
    operator bool() const NOEXCEPT;

    /// Serializer (the normalized expression with its checksum).
    std::string encoded() const NOEXCEPT;

    /// Accessors.
    std::string checksum() const NOEXCEPT;

    /// The expression contains a wildcard key path element.
    bool ranged() const NOEXCEPT;

    /// The expression contains private key material.
    bool has_private_keys() const NOEXCEPT;

    /// All contained scripts can be derived from key material.
    bool solvable() const NOEXCEPT;

    /// Methods.
    /// The output script(s) at the index (multiple for combo, empty on a
    /// hardened wildcard or derivation failure). Index applies to wildcards.
    chain::scripts scripts(uint32_t index) const NOEXCEPT;

protected:
    enum class function : uint8_t
    {
        pk, pkh, wpkh, sh, wsh, multi, sortedmulti, combo, tr, addr, raw,
        invalid
    };

    enum class key_form : uint8_t
    {
        point, extended_public, extended_private, secret, invalid
    };

    /// A parsed key expression.
    struct key_expression
    {
        key_form form{ key_form::invalid };
        std::optional<psbt::key_origin> origin{};
        data_chunk point{};
        hd_public extended_public{};
        hd_private extended_private{};
        std_vector<uint32_t> path{};
        bool wildcard{};
        bool hardened{};

        bool derive(data_chunk& out, uint32_t index) const NOEXCEPT;
    };

    struct node
    {
        function type{ function::invalid };
        uint8_t required{};
        std_vector<key_expression> keys{};
        data_chunk raw{};
        std::string address{};
        std::shared_ptr<node> child{};
    };

    static bool parse(node& out, const std::string& body) NOEXCEPT;
    static bool parse_key(key_expression& out,
        const std::string& text) NOEXCEPT;
    static bool derive_scripts(chain::scripts& out, const node& tree,
        uint32_t index, bool top) NOEXCEPT;

private:
    static descriptor from_string(const std::string& expression) NOEXCEPT;

    descriptor(bool valid, std::string&& body, node&& tree) NOEXCEPT;

    bool valid_{};
    std::string body_{};
    node tree_{};
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
