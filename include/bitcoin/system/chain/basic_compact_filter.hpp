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

#ifndef LIBBITCOIN_SYSTEM_CHAIN_BASIC_COMPACT_FILTER_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_BASIC_COMPACT_FILTER_HPP

#include <istream>
#include <memory>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/chain/block.hpp>
#include <bitcoin/system/chain/compact_filter.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/reader.hpp>
#include <bitcoin/system/utility/writer.hpp>
#include <bitcoin/system/wallet/payment_address.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API basic_compact_filter
	: public compact_filter
{
public:
    typedef std::shared_ptr<basic_compact_filter> ptr;
    typedef std::shared_ptr<const basic_compact_filter> const_ptr;

    static basic_compact_filter factory(const data_chunk& data);
    static basic_compact_filter factory(std::istream& stream);
    static basic_compact_filter factory(reader& source);

    basic_compact_filter();
    basic_compact_filter(const hash_digest& block_hash,
        const data_chunk& filter);
    basic_compact_filter(hash_digest&& block_hash,
        data_chunk&& filter);
    basic_compact_filter(const basic_compact_filter& other);
    basic_compact_filter(basic_compact_filter&& other);
    basic_compact_filter(const compact_filter& other);
    basic_compact_filter(compact_filter&& other);

    bool match(const script script) const;
    bool match(const script::list scripts) const;
    bool match(const wallet::payment_address address) const;
    bool match(const wallet::payment_address::list addresses) const;
    bool populate(const block validated_block);

    bool is_valid() const override;
    void reset() override;

    // This class is move assignable but not copy assignable.
    basic_compact_filter& operator=(basic_compact_filter&& other);
    basic_compact_filter& operator=(compact_filter&& other);
    void operator=(const basic_compact_filter&) = delete;

    bool operator==(const basic_compact_filter& other) const;
    bool operator==(const compact_filter& other) const;
    bool operator!=(const basic_compact_filter& other) const;
    bool operator!=(const compact_filter& other) const;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
