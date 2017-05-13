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
#ifndef LIBBITCOIN_CHAIN_INPUT_HPP
#define LIBBITCOIN_CHAIN_INPUT_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <vector>
#include <bitcoin/bitcoin/chain/output_point.hpp>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>

namespace libbitcoin {
namespace chain {

class BC_API input
{
public:
    typedef std::vector<input> list;

    // Constructors.
    //-------------------------------------------------------------------------

    input();

    input(input&& other);
    input(const input& other);

    input(output_point&& previous_output, chain::script&& script,
        uint32_t sequence);
    input(const output_point& previous_output, const chain::script& script,
        uint32_t sequence);

    // Operators.
    //-------------------------------------------------------------------------

    /// This class is move assignable and copy assignable.
    input& operator=(input&& other);
    input& operator=(const input& other);

    bool operator==(const input& other) const;
    bool operator!=(const input& other) const;

    // Deserialization.
    //-------------------------------------------------------------------------

    static input factory(const data_chunk& data, bool wire=true);
    static input factory(std::istream& stream, bool wire=true);
    static input factory(reader& source, bool wire=true);

    bool from_data(const data_chunk& data, bool wire=true);
    bool from_data(std::istream& stream, bool wire=true);
    bool from_data(reader& source, bool wire=true);

    bool is_valid() const;

    // Serialization.
    //-------------------------------------------------------------------------

    data_chunk to_data(bool wire=true) const;
    void to_data(std::ostream& stream, bool wire=true) const;
    void to_data(writer& sink, bool wire=true) const;

    // Properties (size, accessors, cache).
    //-------------------------------------------------------------------------

    size_t serialized_size(bool wire=true) const;

    // Deprecated (unsafe).
    output_point& previous_output();

    const output_point& previous_output() const;
    void set_previous_output(const output_point& value);
    void set_previous_output(output_point&& value);

    // Deprecated (unsafe).
    chain::script& script();

    const chain::script& script() const;
    void set_script(const chain::script& value);
    void set_script(chain::script&& value);

    uint32_t sequence() const;
    void set_sequence(uint32_t value);

    /// The payment address extracted from this input as a standard script.
    wallet::payment_address address() const;

    // Validation.
    //-------------------------------------------------------------------------

    bool is_final() const;
    size_t signature_operations(bool bip16_active) const;

protected:
    void reset();
    void invalidate_cache() const;

private:
    mutable upgrade_mutex mutex_;
    mutable wallet::payment_address::ptr address_;

    output_point previous_output_;
    chain::script script_;
    uint32_t sequence_;
};

} // namespace chain
} // namespace libbitcoin

#endif
