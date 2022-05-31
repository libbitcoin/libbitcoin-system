/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_CHECKPOINT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_CHECKPOINT_HPP

#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <boost/json.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// TODO: derive from (or alias) point.
class BC_API checkpoint
{
public:
    // Constructors.
    // ------------------------------------------------------------------------

    /// Default checkpoint is an invalid object.
    checkpoint() noexcept;

    /// Defaults.
    checkpoint(checkpoint&&) = default;
    checkpoint(const checkpoint&) = default;
    checkpoint& operator=(checkpoint&&) = default;
    checkpoint& operator=(const checkpoint&) = default;
    ~checkpoint() = default;

    checkpoint(hash_digest&& hash, size_t height) noexcept;
    checkpoint(const hash_digest& hash, size_t height) noexcept;
    explicit checkpoint(const std::string& hash, size_t height) noexcept;

    template <size_t Size, if_size<Size, add1(two * hash_size)> = true>
    checkpoint(const char(&string)[Size], size_t height) noexcept
      : checkpoint(std::string(string), height)
    {
    }

    // Deserialization.
    // ------------------------------------------------------------------------

    ////bool from_string(const std::string& text) noexcept;
    ////bool from_string(std::istream& stream) noexcept;
    ////bool from_string(reader& source) noexcept;

    bool is_valid() const noexcept;

    // Serialization.
    // ------------------------------------------------------------------------

    std::string to_string() const noexcept;
    ////bool to_string(std::ostream& stream) const noexcept;
    ////bool to_string(writer& sink) const noexcept;

    // Properties.
    // ------------------------------------------------------------------------

    size_t height() const noexcept;
    const hash_digest& hash() const noexcept;

protected:
    checkpoint(hash_digest&& hash, size_t height, bool valid) noexcept;
    checkpoint(const hash_digest& hash, size_t height, bool valid) noexcept;

private:
    static checkpoint from_string(const std::string& hash,
        size_t height) noexcept;

    hash_digest hash_;
    size_t height_;
    bool valid_;
};

bool operator<(const checkpoint& left, const checkpoint& right) noexcept;
bool operator==(const checkpoint& left, const checkpoint& right) noexcept;
bool operator!=(const checkpoint& left, const checkpoint& right) noexcept;

// TODO: rationalize with config.
std::ostream& operator<<(std::ostream& stream, const checkpoint& in) noexcept;
std::istream& operator>>(std::istream& stream, checkpoint& out) noexcept;

typedef std::vector<checkpoint> checkpoints;

DECLARE_JSON_VALUE_CONVERTORS(checkpoint);

} // namespace chain
} // namespace system
} // namespace libbitcoin

namespace std
{
template<>
struct hash<bc::system::chain::checkpoint>
{
    size_t operator()(const bc::system::chain::checkpoint& value) const noexcept
    {
        return bc::system::hash_combine(
            std::hash<bc::system::hash_digest>{}(value.hash()), value.height());
    }
};
} // namespace std

#endif
