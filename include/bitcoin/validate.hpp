/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_VALIDATE_HPP
#define LIBBITCOIN_VALIDATE_HPP

#include <thread>
#include <memory>
#include <boost/optional/optional.hpp>

#include <bitcoin/primitives.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/threadpool.hpp>
#include <bitcoin/transaction_pool.hpp>

namespace libbitcoin {

/**
 * If you're looking to validate a transaction, then use the simpler
 * transaction_pool::validate() method instead.
 */
class validate_transaction
  : public std::enable_shared_from_this<validate_transaction>
{
public:
    typedef std::function<
        void (const std::error_code&, const index_list&)> validate_handler;

    validate_transaction(
        blockchain& chain, const transaction_type& tx,
        const pool_buffer& pool, async_strand& strand);
    void start(validate_handler handle_validate);

    static std::error_code check_transaction(
        const transaction_type& tx);
    static bool connect_input(
        const transaction_type& tx, size_t current_input,
        const transaction_type& previous_tx, size_t parent_height,
        size_t last_block_height, uint64_t& value_in);
    static bool tally_fees(const transaction_type& tx,
        uint64_t value_in, uint64_t& fees);

private:
    std::error_code basic_checks() const;
    bool is_standard() const;
    const transaction_type* fetch(const hash_digest& tx_hash) const;

    void handle_duplicate_check(const std::error_code& ec);
    bool is_spent(const output_point outpoint) const;

    // Used for checking coinbase maturity
    void set_last_height(const std::error_code& ec, size_t last_height);
    // Begin looping through the inputs, fetching the previous tx
    void next_previous_transaction();
    void previous_tx_index(const std::error_code& ec, size_t parent_height);
    // If previous_tx_index didn't find it then check in pool instead
    void search_pool_previous_tx();
    void handle_previous_tx(const std::error_code& ec,
        const transaction_type& previous_tx, size_t parent_height);
    // After running connect_input, we check whether this
    // validated previous output wasn't already spent by
    // another input in the blockchain.
    // is_spent() earlier already checked in the pool.
    void check_double_spend(const std::error_code& ec);
    // next_previous_transaction();

    void check_fees();

    async_strand& strand_;
    blockchain& chain_;

    const transaction_type tx_;
    const hash_digest tx_hash_;
    const pool_buffer& pool_;
    size_t last_block_height_;
    uint64_t value_in_;
    size_t current_input_;
    index_list unconfirmed_;
    validate_handler handle_validate_;
};

typedef std::shared_ptr<validate_transaction> validate_transaction_ptr;

class validate_block
{
public:
    std::error_code check_block();
    std::error_code accept_block();
    std::error_code connect_block();

protected:
    validate_block(size_t height, const block_type& current_block);

    virtual uint32_t previous_block_bits() = 0;
    virtual uint64_t actual_timespan(const uint64_t interval) = 0;
    virtual uint64_t median_time_past() = 0;
    virtual bool transaction_exists(const hash_digest& tx_hash) = 0;
    virtual bool is_output_spent(const output_point& outpoint) = 0;
    // These have optional implementations that can be overriden
    virtual bool validate_inputs(const transaction_type& tx,
        size_t index_in_parent, uint64_t& value_in, size_t& total_sigops);
    virtual bool connect_input(size_t index_in_parent,
        const transaction_type& current_tx,
        size_t input_index, uint64_t& value_in, size_t& total_sigops);
    virtual bool fetch_transaction(transaction_type& tx,
        size_t& previous_height, const hash_digest& tx_hash) = 0;
    virtual bool is_output_spent(
        const output_point& previous_output,
        size_t index_in_parent, size_t input_index) = 0;
    virtual block_header_type fetch_block(size_t fetch_height) = 0;

private:
    // check_block()
    bool check_proof_of_work(hash_digest hash, uint32_t bits);
    bool check_transaction(const transaction_type& tx);

    size_t legacy_sigops_count();

    // accept_block()
    uint32_t work_required();
    bool passes_checkpoints();
    bool coinbase_height_match();

    // connect_block()
    bool not_duplicate_or_spent(const transaction_type& tx);

    const size_t height_;
    const block_type& current_block_;
};

} // namespace libbitcoin

#endif

