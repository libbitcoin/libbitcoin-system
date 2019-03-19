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
#ifndef LIBBITCOIN__CHAIN_INPUT_HPP
#define LIBBITCOIN__CHAIN_INPUT_HPP

//#include <cstddef>
//#include <cstdint>
//#include <istream>
//#include <memory>
//#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/input.hpp>
//#include <bitcoin/bitcoin/math/hash.hpp>
//#include <bitcoin/bitcoin/utility/reader.hpp>
//#include <bitcoin/bitcoin/utility/thread.hpp>
//#include <bitcoin/bitcoin/utility/writer.hpp>
#include <bitcoin/chain_output_point.hpp>
#include <bitcoin/chain_script.hpp>
#include <bitcoin/chain_witness.hpp>
#include <bitcoin/utility_data_chunk.hpp>
#include <bitcoin/wallet_payment_address.hpp>

namespace libbitcoin {
//namespace chain {
namespace api {

class BC_API chain_input
{
public:
//    typedef std::vector<input> list;

    // Constructors.
    //-------------------------------------------------------------------------

    chain_input();

//    chain_input(input&& other);
    chain_input(const chain_input& other);

//    chain_input(chain_output_point&& previous_output, chain::script&& script,
//        uint32_t sequence);
    chain_input(const chain_output_point& previous_output, const chain_script& script,
        uint32_t sequence);

//    chain_input(chain_output_point&& previous_output, chain::script&& script,
//        chain::witness&& witness, uint32_t sequence);
    chain_input(const chain_output_point& previous_output, const chain_script& script,
        const chain_witness& witness, uint32_t sequence);

    // Operators.
    //-------------------------------------------------------------------------

//    input& operator=(input&& other);
    chain_input& assign(chain_input&& other);
//    input& operator=(const input& other);

//    bool operator==(const input& other) const;
    bool eq(const chain_input& other) const;
//    bool operator!=(const input& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    static chain_input factory(const utility_data_chunk& data, bool wire=true, bool witness=false);
//    static input factory(std::istream& stream, bool wire=true, bool witness=false);
//    static input factory(reader& source, bool wire=true, bool witness=false);

    bool from_data(const utility_data_chunk& data, bool wire=true, bool witness=false);
//    bool from_data(std::istream& stream, bool wire=true, bool witness=false);
//    bool from_data(reader& source, bool wire=true, bool witness=false);

    bool is_valid() const;

    // Serialization.
    //-------------------------------------------------------------------------

    utility_data_chunk to_data(bool wire=true, bool witness=false) const;
//    void to_data(std::ostream& stream, bool wire=true, bool witness=false) const;
//    void to_data(writer& sink, bool wire=true, bool witness=false) const;

    // Properties (size, accessors, cache).
    //-------------------------------------------------------------------------

    /// This accounts for wire witness, but does not read or write it.
    size_t serialized_size(bool wire=true, bool witness=false) const;

//    chain_output_point& previous_output();
    const chain_output_point& previous_output() const;
//    void set_previous_output(const chain_output_point& value);
//    void set_previous_output(chain_output_point&& value);

    const chain_script& script() const;
    void set_script(const chain_script& value);
//    void set_script(chain::script&& value);

    const chain_witness& witness() const;
    void set_witness(const chain_witness& value);
//    void set_witness(chain::witness&& value);

    uint32_t sequence() const;
    void set_sequence(uint32_t value);

    /// The first payment address extracted (may be invalid).
    libbitcoin::api::wallet_payment_address address() const;

    /// The payment addresses extracted from this input as a standard script.
    wallet_payment_address_list addresses() const;

    // Utilities.
    //-------------------------------------------------------------------------

    /// Clear witness.
    void strip_witness();

    // Validation.
    //-------------------------------------------------------------------------

    bool is_final() const;
    bool is_segregated() const;
    bool is_locked(size_t block_height, uint32_t median_time_past) const;
    size_t signature_operations(bool bip16, bool bip141) const;
    bool extract_reserved_hash(math_hash_digest& out) const;
    bool extract_embedded_script(chain_script& out) const;
    ////bool extract_witness_script(chain::script& out,
    ////    const chain::script& prevout) const;

public:
    chain::input getValue() {
        return value;
    }

    void setValue(chain::input value) {
        this->value = value;
    }
private:
    chain::input value;

//protected:
//    void reset();
//    void invalidate_cache() const;

//private:
//    typedef std::shared_ptr<wallet::payment_address::list> addresses_ptr;
//
//    addresses_ptr addresses_cache() const;
//
//    mutable upgrade_mutex mutex_;
//    mutable addresses_ptr addresses_;
//
//    chain_output_point previous_output_;
//    chain::script script_;
//    chain::witness witness_;
//    uint32_t sequence_;
};

} // namespace api
//} // namespace chain
} // namespace libbitcoin

#endif
