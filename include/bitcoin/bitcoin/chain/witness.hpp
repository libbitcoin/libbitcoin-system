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
#ifndef LIBBITCOIN_CHAIN_WITNESS_HPP
#define LIBBITCOIN_CHAIN_WITNESS_HPP

#include <cstddef>
#include <istream>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/machine/operation.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/writer.hpp>

namespace libbitcoin {
namespace chain {

class BC_API witness
{
public:
    typedef machine::operation operation;

    // Constructors.
    //-------------------------------------------------------------------------

    witness();

    witness(witness&& other);
    witness(const witness& other);

    witness(operation::list&& ops);
    witness(const operation::list& ops);

    witness(data_chunk&& encoded, bool prefix);
    witness(const data_chunk& encoded, bool prefix);

    // Operators.
    //-------------------------------------------------------------------------

    /// This class is move assignable and copy assignable.
    witness& operator=(witness&& other);
    witness& operator=(const witness& other);

    bool operator==(const witness& other) const;
    bool operator!=(const witness& other) const;

    // Deserialization (from witness stack).
    //-------------------------------------------------------------------------
    // Prefixed data assumed valid here though caller may confirm with is_valid.

    static witness factory(const data_chunk& encoded, bool prefix);
    static witness factory(std::istream& stream, bool prefix);
    static witness factory(reader& source, bool prefix);

    /// Deserialization invalidates the iterator.
    bool from_data(const data_chunk& encoded, bool prefix);
    bool from_data(std::istream& stream, bool prefix);
    bool from_data(reader& source, bool prefix);

    /// Deserialization invalidates the iterator.
    void from_operations(operation::list&& ops);
    void from_operations(const operation::list& ops);

    /// The witness deserialized ccording to count and size prefixing.
    bool is_valid() const;

    // Serialization.
    //-------------------------------------------------------------------------

    data_chunk to_data(bool prefix) const;
    void to_data(std::ostream& stream, bool prefix) const;
    void to_data(writer& sink, bool prefix) const;

    // Iteration.
    //-------------------------------------------------------------------------

    void clear();
    bool empty() const;
    size_t size() const;
    const operation& front() const;
    const operation& back() const;
    operation::iterator begin() const;
    operation::iterator end() const;
    const operation& operator[](size_t index) const;

    // Properties (size, accessors, cache).
    //-------------------------------------------------------------------------

    size_t serialized_size(bool prefix) const;
    const operation::list& operations() const;

protected:
    // So that input may call reset from its own.
    friend class input;

    void reset();

private:
    static size_t serialized_size(const operation::list& ops);
    static data_chunk operations_to_data(const operation::list& ops);

    // These are protected by mutex.
    mutable bool valid_;
    mutable operation::list operations_;
    mutable upgrade_mutex mutex_;
};

} // namespace chain
} // namespace libbitcoin

#endif
