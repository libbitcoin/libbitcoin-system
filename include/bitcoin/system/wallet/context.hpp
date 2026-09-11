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
#ifndef LIBBITCOIN_SYSTEM_WALLET_CONEXT_HPP
#define LIBBITCOIN_SYSTEM_WALLET_CONEXT_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/wallet/keys/ec_private.hpp>
#include <bitcoin/system/wallet/keys/hd_private.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// Prefix namespace tree.
/// Values are those emitted by each network, provide an authoritative URL for
/// any values added here.
namespace prefix
{
    typedef struct hds
    {
        uint32_t prv;
        uint32_t pub;
    } hds;

    /// Witness address prefixes.
    namespace p2w
    {
        // Can be compared during execution to validate a parameter.
        constexpr auto undefined = "";

        // github.com/bitcoin/bips/blob/master/bip-0173.mediawiki
        // github.com/litecoin-project/litecoin/blob/master/src/chainparams.cpp
        namespace main
        {
            constexpr auto btc = "bc";
            constexpr auto ltc = "ltc";
        }
        namespace test
        {
            constexpr auto btc = "tb";
            constexpr auto ltc = "tltc";
        }
        // Not in bip173, defined by each implementation.
        // github.com/bitcoin/bitcoin/blob/master/src/kernel/chainparams.cpp
        namespace regtest
        {
            constexpr auto btc = "bcrt";
            constexpr auto ltc = "rltc";
        }
    }

    /// WIF key prefix bytes.
    namespace wif
    {
        // Can be compared during execution to validate a parameter.
        // en.bitcoin.it/wiki/list_of_address_prefixes
        constexpr auto undefined = 0xff;

        namespace main
        {
            constexpr auto btc = 0x80;
            constexpr auto ltc = 0xb0;
        }
        namespace test
        {
            constexpr auto btc = 0xef;
            constexpr auto ltc = 0xef;
        }
        namespace regtest
        {
            constexpr auto btc = test::btc;
            constexpr auto ltc = test::ltc;
        }
    }

    /// Pay to key hash address version bytes.
    namespace p2kh
    {
        // Can be compared during execution to validate a parameter.
        // en.bitcoin.it/wiki/list_of_address_prefixes
        constexpr auto undefined = 0xff;

        namespace main
        {
            constexpr auto btc = 0x00;
            constexpr auto ltc = 0x30;
        }
        namespace test
        {
            constexpr auto btc = 0x6f;
            constexpr auto ltc = 0x6f;
        }
        namespace regtest
        {
            constexpr auto btc = test::btc;
            constexpr auto ltc = test::ltc;
        }
    }

    /// Pay to script hash address version bytes.
    namespace p2sh
    {
        // Can be compared during execution to validate a parameter.
        // en.bitcoin.it/wiki/list_of_address_prefixes
        constexpr auto undefined = 0xff;

        namespace main
        {
            constexpr auto btc = 0x05;

            // Litecoin also accepts 0x05, but emits this value.
            constexpr auto ltc = 0x32;
        }
        namespace test
        {
            constexpr auto btc = 0xc4;

            // Litecoin also accepts 0xc4, but emits this value.
            constexpr auto ltc = 0x3a;
        }
        namespace regtest
        {
            constexpr auto btc = test::btc;
            constexpr auto ltc = test::ltc;
        }
    }

    /// Extended (hd) key prefixes.
    namespace hd
    {
        // Can be compared during execution to validate a parameter.
        constexpr auto undefined = hds{ 0xffffffff, 0xffffffff };

        // github.com/bitcoin/bips/blob/master/bip-0032.mediawiki
        // github.com/litecoin-project/litecoin/blob/master/src/chainparams.cpp
        namespace main
        {
            constexpr auto btc = hds{ 0x0488ade4, 0x0488b21e };
            constexpr auto ltc = btc;
        }
        namespace test
        {
            constexpr auto btc = hds{ 0x04358394, 0x043587cf };
            constexpr auto ltc = btc;
        }
        namespace regtest
        {
            constexpr auto btc = test::btc;
            constexpr auto ltc = test::ltc;
        }
    }
}

/// Portable context for keys and addresses, the set of consistent prefixes
/// for one network.
struct BC_API context
{
    /// Extended (hd) private/public key prefixes.
    prefix::hds hd;

    /// Witness address prefix.
    std::string p2w;

    /// Pay to key hash address version byte.
    uint8_t p2kh;

    /// Pay to script hash address version byte.
    uint8_t p2sh;

    /// WIF (legacy private key) version byte.
    uint8_t wif;

    uint64_t hd_prefixes() const NOEXCEPT
    {
        return hd_private::to_prefixes(hd.prv, hd.pub);
    }

    uint16_t versions() const NOEXCEPT
    {
        return ec_private::to_versions(p2kh, wif);
    }
};

/// Predefined wallet contexts.
/// Custom contexts can also be defined by simply populating a context struct.
namespace ctx
{
    namespace btc
    {
        const context main
        {
            prefix::hd::main::btc,
            prefix::p2w::main::btc,
            prefix::p2kh::main::btc,
            prefix::p2sh::main::btc,
            prefix::wif::main::btc
        };
        const context test
        {
            prefix::hd::test::btc,
            prefix::p2w::test::btc,
            prefix::p2kh::test::btc,
            prefix::p2sh::test::btc,
            prefix::wif::test::btc
        };
        const context regtest
        {
            prefix::hd::regtest::btc,
            prefix::p2w::regtest::btc,
            prefix::p2kh::regtest::btc,
            prefix::p2sh::regtest::btc,
            prefix::wif::regtest::btc
        };
    }
    namespace ltc
    {
        const context main
        {
            prefix::hd::main::ltc,
            prefix::p2w::main::ltc,
            prefix::p2kh::main::ltc,
            prefix::p2sh::main::ltc,
            prefix::wif::main::ltc
        };
        const context test
        {
            prefix::hd::test::ltc,
            prefix::p2w::test::ltc,
            prefix::p2kh::test::ltc,
            prefix::p2sh::test::ltc,
            prefix::wif::test::ltc
        };
        const context regtest
        {
            prefix::hd::regtest::ltc,
            prefix::p2w::regtest::ltc,
            prefix::p2kh::regtest::ltc,
            prefix::p2sh::regtest::ltc,
            prefix::wif::regtest::ltc
        };
    }
}

// TODO: these are static initializations.

/// Predefined contexts for use as default parameter values.
extern const context btc_mainnet;
extern const context btc_testnet;
extern const context btc_regtest;
extern const context ltc_mainnet;
extern const context ltc_testnet;
extern const context ltc_regtest;

extern const context btc;

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
