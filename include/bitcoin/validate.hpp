#ifndef LIBBITCOIN_VALIDATE_H
#define LIBBITCOIN_VALIDATE_H

#include <thread>
#include <memory>
#include <boost/optional/optional.hpp>

#include <bitcoin/messages.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/utility/threads.hpp>
#include <bitcoin/transaction_pool.hpp>

namespace libbitcoin {

class validate_transaction
  : public std::enable_shared_from_this<validate_transaction>
{
public:
    typedef std::function<void (const std::error_code&)> validate_handler;

    validate_transaction(blockchain_ptr chain, exporter_ptr saver,
        const message::transaction& tx,
        const pool_buffer& pool, io_service::strand& async_strand);
    void start(validate_handler handle_validate);

    static std::error_code check_transaction(
        const message::transaction& tx);
    static size_t number_script_sig_operations(
        const message::transaction& tx);
    static bool connect_input(
        const message::transaction& tx, size_t current_input,
        const message::transaction& previous_tx, size_t parent_depth,
        size_t last_block_depth, uint64_t& value_in);
    static bool tally_fees(const message::transaction& tx,
        uint64_t value_in, uint64_t& fees);

private:
    bool basic_checks() const;
    bool is_standard() const;
    bool exists(const hash_digest& tx_hash) const;

    void handle_duplicate_check(const std::error_code& ec);
    bool is_spent(const message::output_point outpoint) const;

    void set_last_depth(const std::error_code& ec, size_t last_depth);
    void fetch_next_previous_transaction();
    void fetch_input_transaction_index(const std::error_code& ec,
        size_t parent_depth);
    void fetch_input_transaction(const std::error_code& ec,
        const message::transaction& previous_tx, size_t parent_depth);

    void check_double_spend(const std::error_code& ec);

    void check_fees();

    blockchain_ptr chain_;
    exporter_ptr exporter_;
    const message::transaction tx_;
    const hash_digest tx_hash_;
    const pool_buffer& pool_;
    size_t last_block_depth_;
    uint64_t value_in_;
    size_t current_input_;
    io_service::strand& strand_;
    validate_handler handle_validate_;
};

typedef std::shared_ptr<validate_transaction> validate_transaction_ptr;

class validate_block
{
public:
    std::error_code start();

protected:
    validate_block(exporter_ptr saver, size_t depth,
        const message::block& current_block);

    virtual uint32_t previous_block_bits() = 0;
    virtual uint64_t actual_timespan(const uint64_t interval) = 0;
    virtual uint64_t median_time_past() = 0;
    virtual bool transaction_exists(const hash_digest& tx_hash) = 0;
    virtual bool is_output_spent(const message::output_point& outpoint) = 0;
    // These have optional implementations that can be overriden
    virtual bool validate_inputs(const message::transaction& tx, 
        size_t index_in_parent, uint64_t& value_in);
    virtual bool connect_input(size_t index_in_parent,
        const message::transaction& current_tx,
        size_t input_index, uint64_t& value_in);
    virtual bool fetch_transaction(message::transaction& tx, 
        size_t& previous_depth, const hash_digest& tx_hash) = 0;
    virtual bool is_output_spent(
        const message::output_point& previous_output,
        size_t index_in_parent, size_t input_index) = 0;

private:
    std::error_code check_block();
    bool check_proof_of_work(hash_digest hash, uint32_t bits);
    bool check_transaction(const message::transaction& tx);
    size_t number_script_sig_operations();

    bool accept_block();
    uint32_t work_required();
    bool passes_checkpoints();

    bool connect_block();
    bool not_duplicate_or_spent(const message::transaction& tx);

    exporter_ptr exporter_;
    chrono_clock_ptr clock_;

    const size_t depth_;
    const message::block& current_block_;
};

} // libbitcoin

#endif

