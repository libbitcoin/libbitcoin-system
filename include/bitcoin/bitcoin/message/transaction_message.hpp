/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_MESSAGE_TRANSACTION_MESSAGE_HPP
#define LIBBITCOIN_MESSAGE_TRANSACTION_MESSAGE_HPP

#include <cstdint>
#include <cstddef>
#include <istream>
#include <memory>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>

namespace libbitcoin {
namespace message {

class BC_API transaction_message
  : public chain::transaction
{
public:
    typedef std::vector<transaction_message> list;
    typedef std::shared_ptr<transaction_message> ptr;
    typedef std::vector<ptr> ptr_list;
    typedef std::vector<size_t> indexes;

    static transaction_message factory_from_data(const data_chunk& data);
    static transaction_message factory_from_data(std::istream& stream);
    static transaction_message factory_from_data(reader& source);

    uint64_t originator() const;
    void set_originator(uint64_t value);

private:
    uint64_t originator_ = 0;
};

} // namspace message
} // namspace libbitcoin

#endif
