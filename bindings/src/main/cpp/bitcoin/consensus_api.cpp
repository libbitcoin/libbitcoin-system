/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include "bitcoin/consensus.hpp"
#include "consensus_api.hpp"

namespace libbitcoin {
//namespace consensus {
namespace api {

// This function is published. The implementation exposes no satoshi internals.
libbitcoin::consensus::verify_result_type verify_script(
  libbitcoin::api::p_unsigned_char transaction,
  libbitcoin::api::p_unsigned_char prevout_script,
  unsigned long long prevout_value,
  unsigned int tx_input_index,
  unsigned int flags)
{
  return libbitcoin::consensus::verify_script(
    transaction.cast(),
    transaction.getSize(),
    prevout_script.cast(),
    prevout_script.getSize(),
    prevout_value,
    tx_input_index,
    flags);
}

} // namespace api
//} // namespace consensus
} // namespace libbitcoin
