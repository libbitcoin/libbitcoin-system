/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_STEALTH_DATABASE_HPP
#define LIBBITCOIN_STEALTH_DATABASE_HPP

#include <stdint.h>
#include <functional>
#include <bitcoin/utility/mmfile.hpp>

namespace libbitcoin {

// https://wiki.unsystem.net/index.php/DarkWallet/Stealth#Database_file_format

class stealth_database
{
public:
    typedef std::function<void (uint8_t*)> write_function;
    typedef std::function<void (const uint8_t*)> read_function;
    stealth_database(mmfile& file);
    void store(write_function write);
    void sync(uint32_t block_height);

    // Linearly scans all entries from offset calculated using from_height.
    // Repeatedly call read() with each entry until we reach the end.
    void scan(read_function read, uint32_t from_height);

private:
    uint64_t calculate_entry_offset(uint32_t index);
    void advise_kernel();

    // sync() sub-methods
    void add_header_entry_index(uint32_t block_height);
    void modify_entries_count();
    void reset();

    // scan() sub-methods
    uint32_t read_start_entry_index(uint32_t from_height);
    void read_entry(uint32_t entry_index, read_function read);
    mmfile& file_;
    // Sector offsets
    uint64_t metadata_sector_ = 0;
    uint64_t header_sector_;
    uint64_t entries_sector_;
    // Metadata
    uint32_t version_;
    uint32_t max_header_rows_;
    uint32_t entries_count_;

    // store() increments this
    // sync() resets to 0
    size_t entries_written_count_ = 0;
};

} // namespace libbitcoin

#endif