/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_WALLET_KEYS_EK_PUBLIC_HPP
#define LIBBITCOIN_SYSTEM_WALLET_KEYS_EK_PUBLIC_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/wallet/keys/encrypted_keys.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// Use to pass an encrypted public key.
class BC_API ek_public
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(ek_public);

    /// Constructors.
    ek_public() NOEXCEPT;
    ek_public(const std::string& encoded) NOEXCEPT;
    ek_public(const encrypted_public& key) NOEXCEPT;

    /// Operators.
    bool operator<(const ek_public& other) const NOEXCEPT;
    bool operator==(const ek_public& other) const NOEXCEPT;
    bool operator!=(const ek_public& other) const NOEXCEPT;
    friend std::istream& operator>>(std::istream& in, ek_public& to);
    friend std::ostream& operator<<(std::ostream& out,
        const ek_public& of) NOEXCEPT;

    /// Cast operators.
    operator bool() const NOEXCEPT;
    operator const encrypted_public&() const NOEXCEPT;

    /// Serializer.
    std::string encoded() const NOEXCEPT;

    /// Accessors.
    const encrypted_public& public_key() const NOEXCEPT;

private:
    /// Factories.
    static ek_public from_string(const std::string& encoded) NOEXCEPT;

    /// Members.
    /// These should be const, apart from the need to implement assignment.
    bool valid_;
    encrypted_public public_;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
