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
#include <bitcoin/bitcoin.hpp>
using namespace bc;

void output(log_level level, const std::string& domain, const std::string& body)
{
    constexpr size_t lvl_spaces = 7;
    constexpr size_t domain_spaces = 10;
    if (body.empty())
        return;
    std::cout << "[";
    std::string lvlrep = level_repr(level);
    std::cout << lvlrep;
    BITCOIN_ASSERT(lvlrep.size() <= lvl_spaces);
    for (size_t i = 0; i < (lvl_spaces - lvlrep.size()); ++i)
        std::cout << " ";
    std::cout << "] ";
    if (!domain.empty())
    {
        BITCOIN_ASSERT(domain.size() <= domain_spaces);
        std::cout << "[" << domain;
        for (size_t i = 0; i < (domain_spaces - domain.size()); ++i)
            std::cout << " ";
        std::cout << "] ";
    }
    std::cout << body << std::endl;
}

int main()
{
    log_debug().set_output_function(output);
    log_info().set_output_function(output);
    log_warning().set_output_function(output);
    log_error().set_output_function(output);
    log_fatal().set_output_function(output);

    log_debug(LOG_PROTOCOL) << "Connected to 500 nodes.";
    log_warning(LOG_BLOCKCHAIN) << "Couldn't start blockchain.";
    return 0;
}

