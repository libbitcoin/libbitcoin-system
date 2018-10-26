/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_PROPERTY_TREE_HPP
#define LIBBITCOIN_PROPERTY_TREE_HPP

#include <map>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/block.hpp>
#include <bitcoin/bitcoin/chain/points_value.hpp>
#include <bitcoin/bitcoin/config/header.hpp>
#include <bitcoin/bitcoin/config/input.hpp>
#include <bitcoin/bitcoin/config/output.hpp>
#include <bitcoin/bitcoin/config/transaction.hpp>

namespace pt = boost::property_tree;

namespace libbitcoin {

class base2;
class wrapper;

/**
 * A tuple to represent settings and serialized values.
 */
typedef std::map<std::string, std::string> settings_list;

/**
 * Generate a property list for a block.
 * @param[in]  block  The block.
 * @param[in]  json   Use json array formatting.
 * @return            A property list.
 */
BC_API pt::ptree property_list(const chain::block& block, bool json);

/**
 * Generate a property tree for a block.
 * @param[in]  block  The block.
 * @param[in]  json   Use json array formatting.
 * @return            A property tree.
 */
BC_API pt::ptree property_tree(const chain::block& block, bool json);

/**
 * Generate a property list for a block header.
 * @param[in]  header  The header.
 * @return             A property list.
 */
BC_API pt::ptree property_list(const config::header& header);

/**
 * Generate a property tree for a block header.
 * @param[in]  header  The header.
 * @return             A property tree.
 */
BC_API pt::ptree property_tree(const config::header& header);

/**
 * Generate a property tree for a set of headers.
 * @param[in]  headers  The set of headers.
 * @param[in]  json     Use json array formatting.
 * @return              A property tree.
 */
BC_API pt::ptree property_tree(const std::vector<config::header>& headers,
    bool json);

/**
 * Generate a property list for a transaction input.
 * @param[in]  tx_input  The input.
 * @return               A property list.
 */
BC_API pt::ptree property_list(const chain::input& tx_input);

/**
 * Generate a property tree for a transaction input.
 * @param[in]  tx_input  The input.
 * @return               A property tree.
 */
BC_API pt::ptree property_tree(const chain::input& tx_input);

/**
 * Generate a property tree for a set of transaction inputs.
 * @param[in]  tx_inputs  The set of transaction inputs.
 * @param[in]  json       Use json array formatting.
 * @return                A property tree.
 */
BC_API pt::ptree property_tree(const chain::input::list& tx_inputs, bool json);

/**
 * Generate a property list for an input.
 * @param[in]  input  The input.
 * @return            A property list.
 */
BC_API pt::ptree property_list(const config::input& input);

/**
 * Generate a property tree for an input.
 * @param[in]  input  The input.
 * @return            A property tree.
 */
BC_API pt::ptree property_tree(const config::input& input);

/**
 * Generate a property tree for a set of inputs.
 * @param[in]  inputs  The set of inputs.
 * @param[in]  json    Use json array formatting.
 * @return             A property tree.
 */
BC_API pt::ptree property_tree(const std::vector<config::input>& inputs,
    bool json);

/**
 * Generate a property list for a transaction output.
 * @param[in]  tx_output  The transaction output.
 * @return                A property list.
 */
BC_API pt::ptree property_list(const chain::output& tx_output);

/**
 * Generate a property tree for a transaction output.
 * @param[in]  tx_output  The transaction output.
 * @return                A property tree.
 */
BC_API pt::ptree property_tree(const chain::output& tx_output);

/**
 * Generate a property tree for a set of transaction outputs.
 * @param[in]  tx_outputs  The set of transaction outputs.
 * @param[in]  json        Use json array formatting.
 * @return                 A property tree.
 */
BC_API pt::ptree property_tree(const chain::output::list& tx_outputs,
    bool json);

/**
 * Generate a property list for a point value.
 * @param[in]  point  The point value.
 * @return            A property list.
 */
BC_API pt::ptree property_list(const chain::point_value& point);

/**
 * Generate a property tree for points value.
 * @param[in]  info  The points value.
 * @param[in]  json  Use json array formatting.
 * @return           A property tree.
 */
BC_API pt::ptree property_tree(const chain::points_value& values, bool json);

/**
 * Generate a property list for a transaction.
 * @param[in]  transaction  The transaction.
 * @param[in]  json         Use json array formatting.
 * @return                  A property list.
 */
BC_API pt::ptree property_list(const config::transaction& transaction, bool json);

/**
 * Generate a property tree for a transaction.
 * @param[in]  transaction  The transaction.
 * @param[in]  json         Use json array formatting.
 * @return                  A property tree.
 */
BC_API pt::ptree property_tree(const config::transaction& transaction, bool json);

/**
 * Generate a property tree for a set of transactions.
 * @param[in]  transactions  The set of transactions.
 * @param[in]  json          Use json array formatting.
 * @return                   A property tree.
 */
BC_API pt::ptree property_tree(const std::vector<config::transaction>& transactions,
    bool json);

/**
 * Generate a property list for a wrapper.
 * @param[in]  wrapper  The wrapper instance.
 * @return              A property list.
 */
BC_API pt::ptree property_list(const wallet::wrapped_data& wrapper);

/**
 * Generate a property tree for a wrapper.
 * @param[in]  wrapper  The wrapper instance.
 * @return              A property tree.
 */
BC_API pt::ptree property_tree(const wallet::wrapped_data& wrapper);

/**
 * Create a property list for the fetch-tx-index command.
 * @param[in]  hash    The block hash.
 * @param[in]  height  The block height.
 * @param[in]  index   The tx index.
 * @returns            A new property list containing the list.
 */
BC_API pt::ptree property_list(const hash_digest& hash, size_t height,
    size_t index);

/**
 * Create a property tree for the fetch-tx-index command.
 * @param[in]  hash    The block hash.
 * @param[in]  height  The block height.
 * @param[in]  index   The tx index.
 * @returns            A new property tree containing the list.
 */
BC_API pt::ptree property_tree(const hash_digest& hash, size_t height,
    size_t index);

/**
 * Create a property tree for the settings command.
 * @param[in]  settings   The list of settings.
 * @returns               A new property tree containing the settings.
 */
BC_API pt::ptree property_tree(const settings_list& settings);

/**
 * Create a property tree for an error code with a corresponding sequence.
 * @param[in]  code     The error code.
 * @param[in]  sequence The sequence.
 * @returns             A new property tree containing the error and sequence.
 */
BC_API pt::ptree property_tree(const std::error_code& code, uint32_t sequence);

/**
 * Create a property tree for a block height with a corresponding sequence.
 * @param[in]  height   The block height.
 * @param[in]  sequence The sequence.
 * @returns             A new property tree containing the height and sequence.
 */
BC_API pt::ptree property_tree(uint64_t height, uint32_t sequence);

/**
 * Create a property tree from a json-string.
 * @param[in]  json   A string containing json data.
 * @param[out] out    A new property tree containing the json equivalent
 *                    fields on success.
 * @returns           True on success. False on error.
 */
BC_API bool property_tree(pt::ptree& out, const std::string& json);

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/property_tree.ipp>

#endif
