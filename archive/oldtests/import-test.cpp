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
#include <boost/lexical_cast.hpp>
#include <bitcoin/bitcoin.hpp>
using namespace bc;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

void blockchain_started(const std::error_code& ec);
void resume_copy(const std::error_code& ec, size_t last_height,
    blockchain* chain_1, blockchain* chain_2);
void copy_block(const std::error_code& ec, const block_type& blk,
    size_t height, blockchain* chain_1, blockchain* chain_2);
void handle_import(const std::error_code& ec,
    size_t height, const hash_digest& hash,
    blockchain* chain_1, blockchain* chain_2);

void blockchain_started(const std::error_code& ec)
{
    if (ec)
        log_error() << "Blockchain init error: " << ec.message();
    else
        log_info() << "Blockchain initialized!";
}

size_t end_height = 0;

void begin_import(const std::error_code& ec, size_t last_height,
    blockchain* chain_1, blockchain* chain_2)
{
    if (ec)
    {
        log_error() << "Fetching stop height: " << last_height;
        return;
    }
    end_height = last_height;
    log_info() << "Stopping at block #" << end_height;
    chain_2->fetch_last_height(
        std::bind(resume_copy, _1, _2, chain_1, chain_2));
}

void resume_copy(const std::error_code& ec, size_t last_height,
    blockchain* chain_1, blockchain* chain_2)
{
    size_t resume_height = last_height + 1;
    if (ec == error::not_found)
        resume_height = 0;
    else if (ec)
    {
        log_error() << "Error fetch last height from DEST: " << ec.message();
        return;
    }
    fetch_block(*chain_1, resume_height,
        std::bind(copy_block, _1, _2, resume_height, chain_1, chain_2));
}

void copy_block(const std::error_code& ec, const block_type& blk,
    size_t height, blockchain* chain_1, blockchain* chain_2)
{
    if (ec)
        log_error() << "Fetch error: " << ec.message();
    log_info() << "Fetched block.";
    log_info() << "Importing...";
    chain_2->import(blk, height,
        std::bind(&handle_import, _1,
            height, hash_block_header(blk), chain_1, chain_2));
}

void handle_import(const std::error_code& ec,
    size_t height, const hash_digest& hash,
    blockchain* chain_1, blockchain* chain_2)
{
    if (ec)
    {
        log_error() << "Import error: " << ec.message();
        return;
    }
    log_info() << "Imported block #" << height << " " << hash;
    log_info() << "Fetching...";
    if (height == end_height)
    {
        log_info() << "Finished.";
        return;
    }
    fetch_block(*chain_1, height + 1,
        std::bind(copy_block, _1, _2, height + 1, chain_1, chain_2));
}

void output_to_file(std::ofstream& file, log_level level,
    const std::string& domain, const std::string& body)
{
    if (body.empty())
        return;
    file << level_repr(level);
    if (!domain.empty())
        file << " [" << domain << "]";
    file << ": " << body << std::endl;
}
void output_cerr_and_file(std::ofstream& file, log_level level,
    const std::string& domain, const std::string& body)
{
    if (body.empty())
        return;
    std::ostringstream output;
    output << level_repr(level);
    if (!domain.empty())
        output << " [" << domain << "]";
    output << ": " << body;
    std::cerr << output.str() << std::endl;
}

int main(int argc, char** argv)
{
    std::ofstream outfile("debug.log"), errfile("error.log");
    log_debug().set_output_function(
        std::bind(output_to_file, std::ref(outfile), _1, _2, _3));
    log_info().set_output_function(
        std::bind(output_to_file, std::ref(outfile), _1, _2, _3));
    log_warning().set_output_function(
        std::bind(output_to_file, std::ref(errfile), _1, _2, _3));
    log_error().set_output_function(
        std::bind(output_cerr_and_file, std::ref(errfile), _1, _2, _3));
    log_fatal().set_output_function(
        std::bind(output_cerr_and_file, std::ref(errfile), _1, _2, _3));

    if (argc != 3)
    {
        log_info() << "Usage: import SOURCE DEST";
        return 1;
    }
    threadpool pool(1);
    bdb_blockchain chain_1(pool);
    leveldb_blockchain chain_2(pool);
    chain_1.start(argv[1], blockchain_started);
    chain_2.start(argv[2], blockchain_started);
    chain_1.fetch_last_height(
        std::bind(begin_import, _1, _2, &chain_1, &chain_2));
    std::cin.get();
    log_info() << "Exiting...";
    pool.stop();
    pool.join();
    chain_1.stop();
    chain_2.stop();
    return 0;
}

