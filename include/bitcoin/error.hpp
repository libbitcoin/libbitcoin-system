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
        object_already_exists,
        unspent_output,
        // transaction_pool errors
        bad_transaction,
        // network errors
        resolve_failed,
        network_unreachable,
        address_in_use,
        listen_failed,
        accept_failed,
        bad_stream,
        channel_stopped,
        channel_timeout,
        // validate
        previous_block_invalid,
        // check_transaction()
        empty_transaction,
        output_value_overflow,
        invalid_coinbase_script_size,
        previous_output_null,
        // check_block()
        size_limits,
        proof_of_work,
        futuristic_timestamp,
        first_not_coinbase,
        extra_coinbases,
        too_many_sigs,
        merkle_mismatch,
        // accept_block()
        accept_block,
        // connect_block()
        connect_block
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

