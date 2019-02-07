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
#ifndef LIBBITCOIN_CONSENSUS_CONSENSUS_API_HPP
#define LIBBITCOIN_CONSENSUS_CONSENSUS_API_HPP

#include "p_unsigned_char.hpp"

namespace libbitcoin {
namespace api {
//namespace consensus {
// These are not published in the public header but are exposed here for test.
//BCK_API verify_result_type script_error_to_verify_result(ScriptError_t code);
//BCK_API unsigned int verify_flags_to_script_flags(unsigned int flags);
// This function is published. The implementation exposes no satoshi internals.
libbitcoin::consensus::verify_result_type verify_script(
  libbitcoin::api::p_unsigned_char transaction,
  libbitcoin::api::p_unsigned_char prevout_script,
  unsigned long long prevout_value,
  unsigned int tx_input_index,
  unsigned int flags);
//} // namespace consensus
} // api
} // namespace libbitcoin

#endif
