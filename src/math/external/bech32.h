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
#ifndef LIBBITCOIN_SYSTEM_BECH32_H
#define LIBBITCOIN_SYSTEM_BECH32_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" { 
#endif

static const size_t bech32_checksum_size;

/* This is the 'out' allocation size required by bech32_expand_prefix. */
size_t bech32_expanded_prefix_size(size_t prefix_size);

/* This is not base_32 expansion: "bc" => [b][c][0][b][c] */
/* There are no limits on prefix characters, caller may restrict as desired. */
/* The 'out' parameter must be allocated to at least (2 * size + 1) bytes. */
void bech32_expand_prefix(uint8_t out[], const char prefix[], size_t size);

/* Insert the checksum into the expanded data for a given bech32 version. */
/* Caller must append a bech32_checksum_size buffer to data for checksum. */
void bech32_insert_checksum(uint8_t expanded_data[], size_t size,
    uint8_t version);

/* Verify the checked data for a given bech32 version (zero if valid). */
int bech32_verify_checksum(const uint8_t expanded_data[], size_t size,
    uint8_t version);

#ifdef __cplusplus
}
#endif
#endif
