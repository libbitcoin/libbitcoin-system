#ifndef LIBBITCOIN_VALIDATE_H
#define LIBBITCOIN_VALIDATE_H

#include <thread>
#include <memory>
#include <boost/optional/optional.hpp>

#include <bitcoin/messages.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/transaction_pool.hpp>

namespace libbitcoin {

class validate_transaction
  : public std::enable_shared_from_this<validate_transaction>
{
public:
    typedef std::function<
        void (const std::error_code&, const index_list&)> validate_handler;

    validate_transaction(
        blockchain_ptr chain, const message::transaction& tx,
        const pool_buffer& pool, io_service::strand& async_strand);
    void start(validate_handler handle_validate);

    static std::error_code check_transaction(
        const message::transaction& tx);
    static bool connect_input(
        const message::transaction& tx, size_t current_input,
        const message::transaction& previous_tx, size_t parent_depth,
        size_t last_block_depth, uint64_t& value_in);
    static bool tally_fees(const message::transaction& tx,
        uint64_t value_in, uint64_t& fees);

private:
    std::error_code basic_checks() const;
    bool is_standard() const;
    const message::transaction* fetch(const hash_digest& tx_hash) const;

    void handle_duplicate_check(const std::error_code& ec);
    bool is_spent(const message::output_point outpoint) const;

    // Used for checking coinbase maturity
    void set_last_depth(const std::error_code& ec, size_t last_depth);
    // Begin looping through the inputs, fetching the previous tx
    void next_previous_transaction();
    void previous_tx_index(const std::error_code& ec, size_t parent_depth);
    // If previous_tx_index didn't find it then check in pool instead
    void search_pool_previous_tx();
    void handle_previous_tx(const std::error_code& ec,
        const message::transaction& previous_tx, size_t parent_depth);
    // After running connect_input, we check whether this
    // validated previous output wasn't already spent by
    // another input in the blockchain.
    // is_spent() earlier already checked in the pool.
    void check_double_spend(const std::error_code& ec);
    // next_previous_transaction();

    void check_fees();

    io_service::strand& strand_;
    blockchain_ptr chain_;

    const message::transaction tx_;
    const hash_digest tx_hash_;
    const pool_buffer& pool_;
    size_t last_block_depth_;
    uint64_t value_in_;
    size_t current_input_;
    index_list unconfirmed_;
    validate_handler handle_validate_;
};

typedef std::shared_ptr<validate_transaction> validate_transaction_ptr;

class validate_block
{
public:
    std::error_code start();

protected:
    validate_block(size_t depth, const message::block& current_block);

    virtual uint32_t previous_block_bits() = 0;
    virtual uint64_t actual_timespan(const uint64_t interval) = 0;
    virtual uint64_t median_time_past() = 0;
    virtual bool transaction_exists(const hash_digest& tx_hash) = 0;
    virtual bool is_output_spent(const message::output_point& outpoint) = 0;
    // These have optional implementations that can be overriden
    virtual bool validate_inputs(const message::transaction& tx, 
        size_t index_in_parent, uint64_t& value_in, size_t& total_sigops);
    virtual bool connect_input(size_t index_in_parent,
        const message::transaction& current_tx,
        size_t input_index, uint64_t& value_in, size_t& total_sigops);
    virtual bool fetch_transaction(message::transaction& tx, 
        size_t& previous_depth, const hash_digest& tx_hash) = 0;
    virtual bool is_output_spent(
        const message::output_point& previous_output,
        size_t index_in_parent, size_t input_index) = 0;

    static size_t script_hash_signature_operations_count(
        const script& output_script, const script& input_script);

private:
    std::error_code check_block();
    bool check_proof_of_work(hash_digest hash, uint32_t bits);
    bool check_transaction(const message::transaction& tx);

    size_t legacy_sigops_count();

    std::error_code accept_block();
    uint32_t work_required();
    bool passes_checkpoints();

    std::error_code connect_block();
    bool not_duplicate_or_spent(const message::transaction& tx);

    chrono_clock_ptr clock_;

    const size_t depth_;
    const message::block& current_block_;
};

} // namespace libbitcoin

#endif

