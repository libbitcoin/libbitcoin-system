#ifndef LIBBITCOIN_STORAGE_ERRORS_H
#define LIBBITCOIN_STORAGE_ERRORS_H

#include <system_error>

namespace libbitcoin {

namespace error
{
    // Specific errors
    enum error_code_t
    {
        // storage errors
        missing_object = 1,
        duplicate,
        unspent_output,
        // network errors
        resolve_failed,
        network_unreachable,
        address_in_use,
        listen_failed,
        accept_failed,
        bad_stream,
        channel_stopped,
        channel_timeout,
        // validate tx
        coinbase_transaction,
        is_not_standard,
        double_spend,
        input_not_found,
        // check_transaction()
        empty_transaction,
        output_value_overflow,
        invalid_coinbase_script_size,
        previous_output_null,
        // validate block
        previous_block_invalid,
        // check_block()
        size_limits,
        proof_of_work,
        futuristic_timestamp,
        first_not_coinbase,
        extra_coinbases,
        too_many_sigs,
        merkle_mismatch,
        // accept_block()
        incorrect_proof_of_work,
        timestamp_too_early,
        checkpoints_failed,
        // connect_block()
        duplicate_or_spent,
        validate_inputs_failed,
        fees_out_of_range,
        coinbase_too_large
    };

    enum error_condition_t
    {
        // validate
        validate_failed = 1
    };

    std::error_code make_error_code(error_code_t e);
    std::error_condition make_error_condition(error_condition_t e);
}

class error_category_impl
  : public std::error_category
{
public:
    virtual const char* name() const;
    virtual std::string message(int ev) const;
    virtual std::error_condition default_error_condition(int ev) const;
};

const std::error_category& error_category();

} // namespace libbitcoin

namespace std
{
    template <>
    struct is_error_code_enum<libbitcoin::error::error_code_t>
      : public true_type {};

    template <>
    struct is_error_condition_enum<libbitcoin::error::error_condition_t>
      : public true_type {};
}

#endif

