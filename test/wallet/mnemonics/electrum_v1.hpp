/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_TEST_ELECTRUM_V1_HPP
#define LIBBITCOIN_SYSTEM_TEST_ELECTRUM_V1_HPP

#include <string>
#include <vector>
#include <bitcoin/system.hpp>

struct electrum_v1_result
{
    std::string entropy;
    std::string mnemonic;
    std::string passphrase;
    std::string seed;
};

typedef std::vector<electrum_v1_result> electrum_v1_result_list;

const auto sentence2 = "blind faith";

const auto sentence12 =
    "blind faith blind faith blind faith "
    "blind faith blind faith blind faith";

const auto sentence24 =
    "blind faith blind faith blind faith "
    "blind faith blind faith blind faith "
    "blind faith blind faith blind faith "
    "blind faith blind faith blind faith";

const auto sentence26 =
    "blind faith blind faith blind faith "
    "blind faith blind faith blind faith "
    "blind faith blind faith blind faith "
    "blind faith blind faith blind faith "
    "blind faith";

const string_list words2
{
    "blind", "faith"
};

const string_list words12
{
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith"
};

const string_list words24
{
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith"
};

const string_list words26
{
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith", "blind", "faith", "blind", "faith",
    "blind", "faith"
};


// TODO: add Electrum v1 vectors.

#endif
