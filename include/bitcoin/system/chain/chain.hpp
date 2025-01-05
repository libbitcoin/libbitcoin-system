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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_CHAIN_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_CHAIN_HPP

#include <bitcoin/system/chain/block.hpp>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/chain/chain_state.hpp>
#include <bitcoin/system/chain/checkpoint.hpp>
#include <bitcoin/system/chain/compact.hpp>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/enums/coverage.hpp>
#include <bitcoin/system/chain/enums/flags.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/enums/numbers.hpp>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/chain/enums/policy.hpp>
#include <bitcoin/system/chain/enums/selection.hpp>
#include <bitcoin/system/chain/enums/script_pattern.hpp>
#include <bitcoin/system/chain/enums/script_version.hpp>
#include <bitcoin/system/chain/header.hpp>
#include <bitcoin/system/chain/input.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/chain/prevout.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/stripper.hpp>
#include <bitcoin/system/chain/transaction.hpp>
#include <bitcoin/system/chain/witness.hpp>

// Byte copy cost is computed as ceilinged divide of total member bits by 8 (128 bits per shared_ptr).
// shared_ptr, data_ptr                                           16 byte copy (but 16 bytes extra storage cost)
// header(hash, hash)                                             81 byte copy (never copied, ptr->block/set)
// operation(data_ptr)                                            18 byte copy (copied by script, data_ptrs to stack)
// script(vector(operation))                                      xx byte copy (copied for legacy sighash subscript)
// witness(vector(data_ptr))                                      xx byte copy (0..2 data_ptrs to program stack)
// output(script::ptr)                                            25 byte copy (never copied, ptr=>tx [sighash, p2p])
// prevout:output                                                 41 byte copy (never copied, ptr=>input [connect])
// point(hash)                                                    37 byte copy (never copied, ptr=>input [sighash])
// input(point::ptr, script::ptr, witness::ptr, prevout::ptr)     69 byte copy (never copied, ptr=>tx [sighash,p2p])
// transaction(vector(input::ptr)::ptr, vector(output::ptr)::ptr) 41 byte copy (never copied, ptr=>block [p2p])
// block(header::ptr, vector(transaction::ptr)::ptr)              33 byte copy (never copied)

// Wire deserialization.
// Objects deserialized off the p2p/server protocol are not aggregated into
// composites. Message objects are fully self-deserializing. However it is
// often necessary to iterate over collections of hashes and headers using a
// sequence of closures, where one element is passed to the closure and the
// next passed on its completion handler until all are processed. To avoid
// copying, each element could be deserialized into vectors of shared pointers.
// However a shared pointer to the containing object must be passed into the
// closure for iteration, which keeps references to its members alive. Instead
// an element may be passed as std::ref(element), avoiding the overhead of a
// shared pointer copy, in the case where the parent object maintains lifetime.

// Wire serialization.
// Objects obtained from the store in response to p2p/server queries are
// deserialized in parts and aggregated. For example, inputs and outputs are
// obtained for a transaction, and then passed into the transaction, the header
// and transactions are obtained for a block and then passed into the block.
// To avoid costly object copies/moves of collections, we store inputs::cptr,
// and outputs::cptr on transaction, and transactions::cptr on block. Otherwise
// a copy/move operation is required for at least a vector of pointers, which
// may number in the thousands, where pointers are not used in the vector this
// would apply to all object instances as well.

// Consensus validation.
// Op.data is stored as a data_ptr, allowing all push_data to be stacked with a
// pointer copy. Operations are smaller than pointers, so these are copied.
// Witnesses and scripts contain vectors of pointers and operations,
// respectively, so these two objects are always moved as pointers. Point and
// header contain hashes, so these are always moved as pointers. Points,
// prevouts, outputs, inputs and transactions are much larger than pointers, so
// these can reasonably be moved by pointer, although at a 16 byte per instance
// storage cost. Inputs, and outputs must be created as sets and then moved
// into a new transaction for signature hashing, so these are retained by
// transaction as a vector::ptr. A transaction set is never added to a block
// for validation, but transactions are retained as vector::ptr for wire
// serialization.

#endif
