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
#include "bech32.h"

#include <stddef.h>
#include <stdint.h>
#include "zeroize.h"

#define BECH32_CHECKSUM_SIZE 6
const size_t bech32_checksum_size = BECH32_CHECKSUM_SIZE;

size_t bech32_expanded_prefix_size(size_t prefix_size)
{
    return 2 * prefix_size + 1;
}

void bech32_expand_prefix(uint8_t out[], const char prefix[], size_t size)
{
    size_t index;
    char character;
    out[size] = 0;

    for (index = 0; index < size; ++index)
    {
        character = prefix[index];
        out[index] = character >> 5;
        out[size + 1 + index] = character & 31;
    }
}

/* The top two bits of the 32 bit checksum are discarded. */
/* This is not standard base_32 expansion because of truncation. */
void bech32_expand_checksum(uint8_t out[BECH32_CHECKSUM_SIZE],
    uint32_t checksum)
{
    out[0] = (checksum >> 25) & 31;
    out[1] = (checksum >> 20) & 31;
    out[2] = (checksum >> 15) & 31;
    out[3] = (checksum >> 10) & 31;
    out[4] = (checksum >>  5) & 31;
    out[5] = (checksum >>  0) & 31;
}

uint32_t bech32_checksum(const uint8_t expanded_data[], size_t size)
{
    static const uint32_t generator[] =
    {
        0x3b6a57b2, 0x26508e6d, 0x1ea119fa, 0x3d4233dd, 0x2a1462b3
    };

    size_t index;
    uint32_t coeficient;
    uint32_t checksum = 1;

    for (index = 0; index < size; ++index)
    {
        coeficient = (checksum >> 25);
        checksum = ((checksum & 0x1ffffff) << 5) ^ expanded_data[index];
        if (coeficient & 0x01) checksum ^= generator[0];
        if (coeficient & 0x02) checksum ^= generator[1];
        if (coeficient & 0x04) checksum ^= generator[2];
        if (coeficient & 0x08) checksum ^= generator[3];
        if (coeficient & 0x10) checksum ^= generator[4];
    }

    return checksum;
}

/* Version zero implies bech32, otherwise bech32m. */
uint32_t constant(uint8_t version)
{
    return version > 0 ? 0x2bc830a3 : 0x00000001;
}

void bech32_insert_checksum(uint8_t expanded_data[], size_t size,
    uint8_t version)
{
    /* Zeroize the checksum buffer. */
    uint8_t* offset = &expanded_data[size - bech32_checksum_size];
    zeroize(offset, bech32_checksum_size);

    /* Compute the checksum for the given version. */
    const uint32_t checksum = bech32_checksum(expanded_data, size) ^
        constant(version);

    /* Insert the expanded checksum into the checksum buffer. */
    bech32_expand_checksum(offset, checksum);
}

int bech32_verify_checksum(const uint8_t expanded_data[], size_t size,
    uint8_t version)
{
    /* Compute the checksum and compare to versioned expectation. */
    return bech32_checksum(expanded_data, size) == constant(version) ? 0 : -1;
}
