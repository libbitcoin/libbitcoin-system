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
#include <bitcoin/bitcoin.hpp>
using namespace bc;

extern data_chunk raw_block44987;
extern data_chunk raw_block44989;

int main()
{
    satoshi_exporter ex;
    const message::block& block44987 = ex.load_block(raw_block44987);
    const message::block& block44989 = ex.load_block(raw_block44989);

    const auto& tx_a = block44987.transactions[1];
    const auto& tx_b = block44989.transactions[16];

    auto output_script = tx_a.outputs[1].output_script;
    auto input_script = tx_b.inputs[0].input_script;

    log_debug() << output_script.pretty();
    log_debug();
    log_debug() << input_script.pretty();

    if (output_script.type() == payment_type::script_hash)
        log_debug() << "scrhash";

    if (output_script.run(input_script, tx_b, 0))
        log_debug() << "pass";
    return 0;
}

