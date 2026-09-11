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
#include <bitcoin/system/wallet/context.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

const context btc_mainnet = ctx::btc::main;
const context btc_testnet = ctx::btc::test;
const context btc_regtest = ctx::btc::regtest;
const context ltc_mainnet = ctx::ltc::main;
const context ltc_testnet = ctx::ltc::test;
const context ltc_regtest = ctx::ltc::regtest;

const context btc = btc_mainnet;

} // namespace wallet
} // namespace system
} // namespace libbitcoin
