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
#ifndef LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_URI_HPP
#define LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_URI_HPP

#include <map>
#include <string>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// A parsed URI according to RFC 3986.
class BC_API uri
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(uri);
    uri() NOEXCEPT {}

    /// Decodes a URI from a string.
    /// @param strict Set to false to tolerate unescaped special characters.
    bool decode(const std::string& encoded, bool strict=true) NOEXCEPT;
    std::string encoded() const NOEXCEPT;

    /// Returns the lowercased URI scheme.
    std::string scheme() const NOEXCEPT;
    void set_scheme(const std::string& scheme) NOEXCEPT;

    /// Obtains the unescaped authority part, if any (user@server:port).
    std::string authority() const NOEXCEPT;
    bool has_authority() const NOEXCEPT;
    void set_authority(const std::string& authority) NOEXCEPT;
    void remove_authority() NOEXCEPT;

    /// Obtains the unescaped path part.
    std::string path() const NOEXCEPT;
    void set_path(const std::string& path) NOEXCEPT;

    /// Returns the unescaped query string, if any.
    std::string query() const NOEXCEPT;
    bool has_query() const NOEXCEPT;
    void set_query(const std::string& query) NOEXCEPT;
    void remove_query() NOEXCEPT;

    /// Returns the unescaped fragment string, if any.
    std::string fragment() const NOEXCEPT;
    bool has_fragment() const NOEXCEPT;
    void set_fragment(const std::string& fragment) NOEXCEPT;
    void remove_fragment() NOEXCEPT;

    typedef std::map<std::string, std::string> query_map;

    /// Interprets the query string as a sequence of key-value pairs.
    /// All query strings are valid, so this function cannot fail.
    /// The results are unescaped. Both keys and values can be zero-length,
    /// and if the same key is appears multiple times, the final one wins.
    query_map decode_query() const NOEXCEPT;
    void encode_query(const query_map& map) NOEXCEPT;

private:
    // All parts are stored with their original escaping.
    std::string scheme_;
    std::string authority_;
    std::string path_;
    std::string query_;
    std::string fragment_;

    bool has_authority_ = false;
    bool has_query_ = false;
    bool has_fragment_ = false;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
