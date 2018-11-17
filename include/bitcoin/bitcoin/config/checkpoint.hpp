/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_CHECKPOINT_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_CHECKPOINT_HPP

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/formats/base_16.hpp>

namespace libbitcoin {
namespace config {

/**
 * Serialization helper for a blockchain checkpoint.
 * This is a container for a {block hash, block height} tuple.
 */
class BC_API checkpoint
{
public:
    /**
     * A list of checkpoints.
     * This must provide operator<< for ostream in order to be used as a
     * boost::program_options default_value.
     */
    typedef std::vector<checkpoint> list;

    /**
     * Created a sorted copy of the list of checkpoints.
     * @param[in]  checks  The list of checkpoints.
     * @return             The sorted list of checkpoints.
     */
    static list sort(const list& checks);

    /**
     * Confirm a checkpoint is in the range of a sorted list of checkpoints.
     * @param[in]  height  The height of checkpoint.
     * @param[in]  checks  The list of checkpoints.
     */
    static bool covered(size_t height, const list& checks);

    /**
     * Validate a checkpoint against a set of checkpoints.
     * @param[in]  hash    The hash of the checkpoint.
     * @param[in]  height  The height of checkpoint.
     * @param[in]  checks  The set of checkpoints.
     */
    static bool validate(const hash_digest& hash, size_t height,
        const list& checks);

    /**
     * Default constructor.
     */
    checkpoint();

    /**
     * Copy constructor.
     * @param[in]  other  The object to copy into self on construct.
     */
    checkpoint(const checkpoint& other);

    /**
     * Initialization constructor.
     * The height is optional and will be set to zero if not provided.
     * @param[in]  value  The value of the hash[:height] form.
     */
    checkpoint(const std::string& value);

    /**
     * Initialization constructor.
     * @param[in]  hash    The string block hash for the checkpoint.
     * @param[in]  height  The height of the hash.
     */
    checkpoint(const std::string& hash, size_t height);

    /**
     * Initialization constructor.
     * @param[in]  hash    The block hash for the checkpoint.
     * @param[in]  height  The height of the hash.
     */
    checkpoint(const hash_digest& hash, size_t height);

    /**
     * Getter.
     * @return The block hash of the checkpoint.
     */
    const hash_digest& hash() const;

    /**
     * Getter.
     * @return The block height of the checkpoint.
     */
    size_t height() const;

    /**
     * Get the checkpoint as a string.
     * @return The ip address of the authority in the hash:height form.
     */
    std::string to_string() const;

    /**
     * Override the equality operator.
     * @param[in]  other  The other object with which to compare.
     */
    bool operator==(const checkpoint& other) const;

    /**
     * Define stream in. Throws if input is invalid.
     * @param[in]   input     The input stream to read the value from.
     * @param[out]  argument  The object to receive the read value.
     * @return                The input stream reference.
     */
    friend std::istream& operator>>(std::istream& input,
        checkpoint& argument);

    /**
     * Define stream out.
     * @param[in]   output    The output stream to write the value to.
     * @param[out]  argument  The object from which to obtain the value.
     * @return                The output stream reference.
     */
    friend std::ostream& operator<<(std::ostream& output,
        const checkpoint& argument);

private:
    hash_digest hash_;
    size_t height_;
};

} // namespace config
} // namespace libbitcoin

#endif
