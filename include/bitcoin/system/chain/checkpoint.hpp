/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// TODO: derive from (or alias) point.
class BC_API checkpoint
{
public:
    typedef std::vector<checkpoint> list;

    DEFAULT_COPY_MOVE_DESTRUCT(checkpoint);

    static bool is_under(const list& checkpoints, size_t height) NOEXCEPT;
    static bool is_conflict(const list& checkpoints, const hash_digest& hash,
        size_t height) NOEXCEPT;

    /// Constructors.
    /// -----------------------------------------------------------------------

    /// Default checkpoint is an invalid object.
    checkpoint() NOEXCEPT;

    checkpoint(hash_digest&& hash, size_t height) NOEXCEPT;
    checkpoint(const hash_digest& hash, size_t height) NOEXCEPT;
    explicit checkpoint(const std::string& hash, size_t height) NOEXCEPT;

    // TODO: move to config serialization wrapper (?).
    template <size_t Size, if_equal<Size, add1(two * hash_size)> = true>
    checkpoint(const char(&string)[Size], size_t height) NOEXCEPT
      : checkpoint(std::string(string), height)
    {
    }

    /// Deserialization.
    /// -----------------------------------------------------------------------

    // TODO: move to config serialization wrapper.
    ////bool from_string(const std::string& text) NOEXCEPT;
    ////bool from_string(std::istream& stream) NOEXCEPT;
    ////bool from_string(reader& source) NOEXCEPT;

    bool is_valid() const NOEXCEPT;

    /// Serialization.
    /// -----------------------------------------------------------------------

    // TODO: move to config serialization wrapper.
    std::string to_string() const NOEXCEPT;
    ////bool to_string(std::ostream& stream) const NOEXCEPT;
    ////bool to_string(writer& sink) const NOEXCEPT;

    /// Properties.
    /// -----------------------------------------------------------------------

    size_t height() const NOEXCEPT;
    const hash_digest& hash() const NOEXCEPT;

protected:
    checkpoint(hash_digest&& hash, size_t height, bool valid) NOEXCEPT;
    checkpoint(const hash_digest& hash, size_t height, bool valid) NOEXCEPT;

private:
    // TODO: move to config serialization wrapper.
    static checkpoint from_string(const std::string& hash,
        size_t height) NOEXCEPT;

    hash_digest hash_;
    size_t height_;
    bool valid_;
};

bool operator<(const checkpoint& left, const checkpoint& right) NOEXCEPT;
bool operator==(const checkpoint& left, const checkpoint& right) NOEXCEPT;
bool operator!=(const checkpoint& left, const checkpoint& right) NOEXCEPT;

// TODO: rationalize with config.
std::ostream& operator<<(std::ostream& stream, const checkpoint& in) NOEXCEPT;
std::istream& operator>>(std::istream& stream, checkpoint& out) NOEXCEPT;

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
    size_t operator()(const bc::system::chain::checkpoint& value) const NOEXCEPT
    {
        return bc::system::hash_combine(
            std::hash<bc::system::hash_digest>{}(value.hash()), value.height());
    }
};
} // namespace std

#endif
