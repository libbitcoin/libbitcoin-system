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
#ifndef LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_URI_READER_HPP
#define LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_URI_READER_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/wallet/addresses/uri.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// Interface for URI deserialization.
/// The URI parser calls these methods as it extracts each URI component.
/// A false return from any setter is expected to terminate the parser.
class BC_API uri_reader
{
public:

    /// Parses any URI string into its individual components.
    /// @param[in]  uri     The URI to parse.
    /// @param[in]  strict  Only accept properly-escaped parameters.
    /// @return The parsed URI or a default instance if the `uri` is malformed
    /// according to the  `UriReader`.
    template <class UriReader>
    static UriReader parse(const std::string& uri, bool strict=true) NOEXCEPT
    {
        wallet::uri parsed;
        if (!parsed.decode(uri, strict))
            return {};

        UriReader out;
        out.set_strict(strict);
        out.set_scheme(parsed.scheme());
        if (parsed.has_authority() && !out.set_authority(parsed.authority()))
            return {};

        if (!parsed.path().empty() && !out.set_path(parsed.path()))
            return {};

        if (parsed.has_fragment() && !out.set_fragment(parsed.fragment()))
            return {};

        const auto query = parsed.decode_query();
        for (const auto& term: query)
        {
            const auto& key = term.first;
            const auto& value = term.second;
            if (!key.empty() && !out.set_parameter(key, value))
                return {};
        }

        return out;
    }

    /// uri_reader interface.
    virtual void set_strict(bool strict) NOEXCEPT = 0;
    virtual bool set_scheme(const std::string& scheme) NOEXCEPT = 0;
    virtual bool set_authority(const std::string& authority) NOEXCEPT = 0;
    virtual bool set_path(const std::string& path) NOEXCEPT = 0;
    virtual bool set_fragment(const std::string& fragment) NOEXCEPT = 0;
    virtual bool set_parameter(const std::string& key,
        const std::string& value) NOEXCEPT = 0;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
