/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_TEST_MACHINE_ACCESSOR_HPP
#define LIBBITCOIN_SYSTEM_TEST_MACHINE_ACCESSOR_HPP

// Exposes the protected machine surface for isolated unit testing.
template <typename Stack, typename Program = machine::program<Stack>>
class interpreter_accessor
  : public machine::interpreter<Stack, Program>
{
public:
    using base = machine::interpreter<Stack, Program>;
    using base::base;

    // Operation dispatch.
    using base::run_op;

    // Operation handlers.
    using base::op_unevaluated;
    using base::op_nop;
    using base::op_push_number;
    using base::op_push_size;
    using base::op_push_one_size;
    using base::op_push_two_size;
    using base::op_push_four_size;
    using base::op_ver;
    using base::op_if;
    using base::op_notif;
    using base::op_verif;
    using base::op_vernotif;
    using base::op_else;
    using base::op_endif;
    using base::op_verify;
    using base::op_return;
    using base::op_to_alt_stack;
    using base::op_from_alt_stack;
    using base::op_drop2;
    using base::op_dup2;
    using base::op_dup3;
    using base::op_over2;
    using base::op_rot2;
    using base::op_swap2;
    using base::op_if_dup;
    using base::op_depth;
    using base::op_drop;
    using base::op_dup;
    using base::op_nip;
    using base::op_over;
    using base::op_pick;
    using base::op_roll;
    using base::op_rot;
    using base::op_swap;
    using base::op_tuck;
    using base::op_cat;
    using base::op_substr;
    using base::op_left;
    using base::op_right;
    using base::op_size;
    using base::op_invert;
    using base::op_and;
    using base::op_or;
    using base::op_xor;
    using base::op_equal;
    using base::op_equal_verify;
    using base::op_add1;
    using base::op_sub1;
    using base::op_mul2;
    using base::op_div2;
    using base::op_negate;
    using base::op_abs;
    using base::op_not;
    using base::op_nonzero;
    using base::op_add;
    using base::op_sub;
    using base::op_mul;
    using base::op_div;
    using base::op_mod;
    using base::op_lshift;
    using base::op_rshift;
    using base::op_bool_and;
    using base::op_bool_or;
    using base::op_num_equal;
    using base::op_num_equal_verify;
    using base::op_num_not_equal;
    using base::op_less_than;
    using base::op_greater_than;
    using base::op_less_than_or_equal;
    using base::op_greater_than_or_equal;
    using base::op_min;
    using base::op_max;
    using base::op_within;
    using base::op_ripemd160;
    using base::op_sha1;
    using base::op_sha256;
    using base::op_hash160;
    using base::op_hash256;
    using base::op_codeseparator;
    using base::op_check_sig;
    using base::op_check_sig_verify;
    using base::op_check_multisig_verify;
    using base::op_check_multisig;
    using base::op_check_locktime_verify;
    using base::op_check_sequence_verify;
    using base::op_check_sig_add;

    // Connect handlers.
    using base::connect_embedded;
    using base::connect_witness;

    // Program constants.
    using base::initialize;
    using base::begin;
    using base::end;
    using base::tx;
    using base::input;
    using base::is_enabled;
    using base::equal_chunks;

    // Primary stack (push).
    // push_chunk overloads are forwarded, as one base overload is private.
    void push_chunk(data_chunk&& datum) NOEXCEPT
    {
        base::push_chunk(std::move(datum));
    }

    void push_chunk(const chunk_cptr& datum) NOEXCEPT
    {
        base::push_chunk(datum);
    }

    using base::push_bool;
    using base::push_signed64;
    using base::push_length;
    using base::push_variant;

    // Primary stack (pop).
    using base::pop_chunk_;
    using base::pop_strict_bool_;
    using base::pop_bool_;
    using base::pop_chunks;
    using base::pop_signed32;
    using base::pop_signed32_;
    using base::pop_binary32;
    using base::pop_ternary32;
    using base::pop_index32;
    using base::pop_;

    // Primary stack (peek).
    using base::peek_bool_;
    using base::peek_unsigned32;
    using base::peek_unsigned40;
    using base::peek_size;
    using base::peek_;

    // Primary stack (variant).
    using base::swap_;
    using base::erase_;
    using base::drop_;

    // Primary stack (state).
    using base::stack_size;
    using base::stack_nonempty;
    using base::is_stack_empty;
    using base::is_stack_overflow;

    // Alternate stack.
    using base::is_alternate_empty;
    using base::push_alternate;
    using base::pop_alternate_;

    // Conditional stack.
    using base::begin_if;
    using base::else_if_;
    using base::end_if_;
    using base::is_balanced;
    using base::is_success;
    using base::if_;

    // Accumulators.
    using base::sigops_increment;
    using base::ops_increment;

    // Endorsement parsing.
    using base::schnorr_split;
    using base::ecdsa_split;
    using base::decode_signature;

    // Signature subscripting.
    using base::set_subscript;
    using base::subscript;

    // Signature hashing.
    using base::signature_hash;
    using base::cached;
    using base::uncache;
    using base::cached_hash;
    using base::set_hash;

    // Signature verify.
    using base::verify_ecdsa_signature;
    using base::try_batch_multisig_verification;
    using base::verify_schnorr_signature;
};

// Substitutes signature decoding, hashing and verification for isolation.
template <typename Stack>
class mock_program
  : public machine::program<Stack>
{
public:
    using machine::program<Stack>::program;

    // Results returned by the mocked methods.
    bool decode_result{ true };
    bool hash_result{ true };
    bool ecdsa_result{ true };
    bool schnorr_result{ true };

    // Number of leading ecdsa verifications to fail before ecdsa_result.
    mutable size_t ecdsa_failures{ 0 };

protected:
    bool decode_signature(ec_signature&, const data_slice&,
        bool) const NOEXCEPT override
    {
        return decode_result;
    }

    bool signature_hash(hash_digest& out, uint8_t) const NOEXCEPT override
    {
        out = one_hash;
        return hash_result;
    }

    bool signature_hash(hash_digest& out, const chain::script&,
        uint8_t) const NOEXCEPT override
    {
        out = one_hash;
        return hash_result;
    }

    bool set_hash(uint8_t) const NOEXCEPT override
    {
        return hash_result;
    }

    void set_hash(const chain::script&, uint8_t) const NOEXCEPT override
    {
    }

    const hash_digest& cached_hash() const NOEXCEPT override
    {
        return one_hash;
    }

    bool verify_ecdsa_signature(const data_chunk&, const hash_digest&,
        const ec_signature&, bool=true) const NOEXCEPT override
    {
        if (is_nonzero(ecdsa_failures))
        {
            --ecdsa_failures;
            return false;
        }

        return ecdsa_result;
    }

    bool try_batch_multisig_verification(const chunk_xptrs&,
        const chunk_xptrs&) const NOEXCEPT override
    {
        return false;
    }

    bool verify_schnorr_signature(const data_chunk&, const hash_digest&,
        const ec_signature&) const NOEXCEPT override
    {
        return schnorr_result;
    }
};

// Single-input transaction for input-script program construction.
inline chain::transaction accessor_transaction(const chain::script& script,
    uint32_t sequence=chain::max_input_sequence, uint32_t locktime=0,
    uint32_t version=1) NOEXCEPT
{
    const chain::point outpoint{ one_hash, 0u };
    const chain::inputs inputs{ chain::input{ outpoint, script, sequence } };
    const chain::outputs outputs{ chain::output{ 0u, chain::script{} } };
    return chain::transaction{ version, inputs, outputs, locktime };
}

// Composes an interpreter_accessor with the transaction it references.
template <typename Stack, typename Program = machine::program<Stack>>
class machine_accessor
{
public:
    machine_accessor(const chain::script& script, uint32_t active_flags,
        uint32_t sequence=chain::max_input_sequence, uint32_t locktime=0,
        uint32_t version=1) NOEXCEPT
      : transaction_(accessor_transaction(script, sequence, locktime, version)),
        accessor_(transaction_, transaction_.inputs_ptr()->begin(),
            active_flags, capture_)
    {
    }

    interpreter_accessor<Stack, Program>* operator->() NOEXCEPT
    {
        return &accessor_;
    }

    const chain::transaction& transaction() const NOEXCEPT
    {
        return transaction_;
    }

private:
    const chain::signatures capture_{};
    const chain::transaction transaction_;
    interpreter_accessor<Stack, Program> accessor_;
};

#endif
