/**
 * Copyright (c) 2011-2016 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/chain/chain_state.hpp>

#include <bitcoin/bitcoin/chain/chain_state.hpp>
#include <bitcoin/bitcoin/chain/header.hpp>
#include <bitcoin/bitcoin/chain/script/opcode.hpp>
#include <bitcoin/bitcoin/chain/script/script.hpp>
#include <bitcoin/bitcoin/config/checkpoint.hpp>
#include <bitcoin/bitcoin/constants.hpp>

namespace libbitcoin {
namespace chain {

using namespace bc::config;

chain_state::chain_state(bool testnet, const checkpoint::list& checkpoints)
  : sample_size(testnet ? testnet_sample : mainnet_sample),
    next_height_(0),
    work_required_(0),
    median_time_past_(0),
    minimum_version_(first_version),
    enabled_forks_(rule_fork::no_rules),
    testnet_(testnet),
    active_(testnet ? testnet_active : mainnet_active),
    enforce_(testnet ? testnet_enforce : mainnet_enforce),
    bip30_exception_height1_(testnet ? testnet_bip30_exception_height1 :
        mainnet_bip30_exception_height1),
    bip30_exception_height2_(testnet ? testnet_bip30_exception_height2 :
        mainnet_bip30_exception_height2),
    bip16_activation_height_(testnet ? testnet_bip16_activation_height :
        mainnet_bip16_activation_height),
    checkpoints_(checkpoints)
{
}

// Properties.
//-----------------------------------------------------------------------------

size_t chain_state::next_height() const
{
    return next_height_;
}

uint32_t chain_state::enabled_forks() const
{
    return enabled_forks_;
}

uint32_t chain_state::minimum_version() const
{
    return minimum_version_;
}

uint32_t chain_state::median_time_past() const
{
    return median_time_past_;
}

uint32_t chain_state::work_required() const
{
    return work_required_;
}

// Forks.
//-----------------------------------------------------------------------------

bool chain_state::is_enabled(rule_fork flag) const
{
    return script::is_enabled(enabled_forks_, flag);
}

bool chain_state::is_enabled(const header& header, rule_fork flag) const
{
    return (is_enabled(flag)) &&
       ((flag == rule_fork::bip65_rule && header.version >= bip65_version) ||
        (flag == rule_fork::bip66_rule && header.version >= bip66_version) ||
        (flag == rule_fork::bip34_rule && header.version >= bip34_version));
}

bool chain_state::is_checkpoint_failure(const header& header) const
{
    return !checkpoint::validate(header.hash(), next_height_, checkpoints_);
}

// Context.
//-----------------------------------------------------------------------------

void chain_state::set_context(size_t height, const versions& history)
{
    //-------------------------------------------------------------------------
    const auto ge_4 = [](uint8_t version) { return version >= bip65_version; };
    const auto ge_3 = [](uint8_t version) { return version >= bip66_version; };
    const auto ge_2 = [](uint8_t version) { return version >= bip34_version; };

    const auto count_4 = std::count_if(history.begin(), history.end(), ge_4);
    const auto count_3 = std::count_if(history.begin(), history.end(), ge_3);
    const auto count_2 = std::count_if(history.begin(), history.end(), ge_2);

    const auto activated = [this](size_t count) { return count >= active_; };
    const auto enforced = [this](size_t count) { return count >= enforce_; };
    //-------------------------------------------------------------------------

    next_height_ = height;
    enabled_forks_ = rule_fork::no_rules;

    // bip65 is activated based on 75% of preceding 1000 mainnet blocks.
    if (activated(count_4))
        enabled_forks_ |= rule_fork::bip65_rule;

    // bip66 is activated based on 75% of preceding 1000 mainnet blocks.
    if (activated(count_3))
        enabled_forks_ |= rule_fork::bip66_rule;

    // bip34 is activated based on 75% of preceding 1000 mainnet blocks.
    if (activated(count_2))
        enabled_forks_ |= rule_fork::bip34_rule;

    // bip30 applies to all but two mainnet blocks that violate the rule.
    if (height != bip30_exception_height1_ &&
        height != bip30_exception_height2_)
        enabled_forks_ |= rule_fork::bip30_rule;

    // bip16 was activated with a one-time test on mainnet/testnet (~55% rule).
    if (height >= bip16_activation_height_)
        enabled_forks_ |= rule_fork::bip16_rule;

    // version 4/3/2 enforced based on 95% of preceding 1000 mainnet blocks.
    if (enforced(count_4))
        minimum_version_ = bip65_version;
    else if (enforced(count_3))
        minimum_version_ = bip66_version;
    else if (enforced(count_2))
        minimum_version_ = bip34_version;
    else
        minimum_version_ = first_version;
}

} // namespace chain
} // namespace libbitcoin
