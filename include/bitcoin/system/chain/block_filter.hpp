/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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

// Sponsored in part by Digital Contract Design, LLC

#ifndef LIBBITCOIN_SYSTEM_CHAIN_BLOCK_FILTER_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_BLOCK_FILTER_HPP

#include <istream>
#include <memory>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/reader.hpp>
#include <bitcoin/system/utility/writer.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API block_filter
{
public:
    typedef std::shared_ptr<block_filter> ptr;
    typedef std::shared_ptr<const block_filter> const_ptr;

    // THIS IS FOR LIBRARY USE ONLY, DO NOT CREATE A DEPENDENCY ON IT.
    struct validation
    {
        /// This is a non-consensus sentinel value.
        static const uint64_t unlinked;

        /// Derived from hash table or block reference.
        /// Filter currently exists at the link (in any state).
        uint64_t link = unlinked;
    };

    static block_filter factory(const data_chunk& data, bool wire);
    static block_filter factory(std::istream& stream, bool wire);
    static block_filter factory(reader& source, bool wire);

    block_filter();
    block_filter(uint8_t filter_type, const hash_digest& header,
        const data_chunk& filter);
    block_filter(uint8_t filter_type, hash_digest&& header,
        data_chunk&& filter);
    block_filter(const block_filter& other);
    block_filter(block_filter&& other);

    uint8_t filter_type() const;
    void set_filter_type(uint8_t value);

    hash_digest& header();
    const hash_digest& header() const;
    void set_header(const hash_digest& value);
    void set_header(hash_digest&& value);

    data_chunk& filter();
    const data_chunk& filter() const;
    void set_filter(const data_chunk& value);
    void set_filter(data_chunk&& value);

    bool is_valid() const;
    void reset();

    bool from_data(const data_chunk& data, bool wire);
    bool from_data(std::istream& stream, bool wire);
    bool from_data(reader& source, bool wire);
    data_chunk to_data(bool wire) const;
    void to_data(std::ostream& stream, bool wire) const;
    void to_data(writer& sink, bool wire) const;
    size_t serialized_size(bool wire) const;

    bool from_data(reader& source, uint8_t filter_type);

    // This class is move assignable but not copy assignable.
    block_filter& operator=(block_filter&& other);
    void operator=(const block_filter&) = delete;

    bool operator==(const block_filter& other) const;
    bool operator!=(const block_filter& other) const;

    // THIS IS FOR LIBRARY USE ONLY, DO NOT CREATE A DEPENDENCY ON IT.
    mutable validation metadata;

private:
    uint8_t filter_type_;
    hash_digest header_;
    data_chunk filter_;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
