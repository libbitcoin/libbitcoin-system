/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/chain/witness.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <iterator>
#include <numeric>
#include <string>
#include <utility>
#include <boost/algorithm/string.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/error.hpp>
#include <bitcoin/system/machine/operation.hpp>
#include <bitcoin/system/machine/program.hpp>
#include <bitcoin/system/machine/script_pattern.hpp>
#include <bitcoin/system/message/messages.hpp>
#include <bitcoin/system/utility/assert.hpp>
#include <bitcoin/system/utility/collection.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/container_source.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/istream_reader.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

using namespace bc::system::machine;

// Constructors.
//-----------------------------------------------------------------------------

// A default instance is invalid (until modified).
witness::witness()
  : valid_(false)
{
}

witness::witness(witness&& other)
  : stack_(std::move(other.stack_)), valid_(other.valid_)
{
}

witness::witness(const witness& other)
  : stack_(other.stack_), valid_(other.valid_)
{
}

witness::witness(const data_stack& stack)
{
    stack_ = stack;
}

witness::witness(data_stack&& stack)
{
    stack_ = std::move(stack);
}

witness::witness(data_chunk&& encoded, bool prefix)
{
    from_data(encoded, prefix);
}

witness::witness(const data_chunk& encoded, bool prefix)
{
    from_data(encoded, prefix);
}

// Operators.
//-----------------------------------------------------------------------------

witness& witness::operator=(witness&& other)
{
    reset();
    stack_ = std::move(other.stack_);
    valid_ = other.valid_;
    return *this;
}

witness& witness::operator=(const witness& other)
{
    reset();
    stack_ = other.stack_;
    valid_ = other.valid_;
    return *this;
}
bool witness::operator==(const witness& other) const
{
    return stack_ == other.stack_;
}

bool witness::operator!=(const witness& other) const
{
    return !(*this == other);
}

// Deserialization.
//-----------------------------------------------------------------------------

// static
witness witness::factory(const data_chunk& encoded, bool prefix)
{
    witness instance;
    instance.from_data(encoded, prefix);
    return instance;
}

// static
witness witness::factory(std::istream& stream, bool prefix)
{
    witness instance;
    instance.from_data(stream, prefix);
    return instance;
}

// static
witness witness::factory(reader& source, bool prefix)
{
    witness instance;
    instance.from_data(source, prefix);
    return instance;
}

bool witness::from_data(const data_chunk& encoded, bool prefix)
{
    data_source istream(encoded);
    return from_data(istream, prefix);
}

bool witness::from_data(std::istream& stream, bool prefix)
{
    istream_reader source(stream);
    return from_data(source, prefix);
}

// Prefixed data assumed valid here though caller may confirm with is_valid.
bool witness::from_data(reader& source, bool prefix)
{
    reset();
    valid_ = true;

    const auto read_element = [](reader& source)
    {
        // Tokens encoded as variable integer prefixed byte array (bip144).
        const auto size = source.read_size_little_endian();

        // The max_script_size and max_push_data_size constants limit
        // evaluation, but not all stacks evaluate, so use max_block_weight
        // to guard memory allocation here.
        if (size > max_block_weight)
        {
            source.invalidate();
            return data_chunk{};
        }

        return source.read_bytes(size);
    };

    // TODO: optimize store serialization to avoid loop, reading data directly.
    if (prefix)
    {
        // Witness prefix is an element count, not byte length (unlike script).
        // On wire each witness is prefixed with number of elements (bip144).
        for (auto count = source.read_size_little_endian(); count > 0; --count)
             stack_.push_back(read_element(source));
    }
    else
    {
        while (!source.is_exhausted())
            stack_.push_back(read_element(source));
    }

    if (!source)
        reset();

    return source;
}

// private/static
size_t witness::serialized_size(const data_stack& stack)
{
    const auto sum = [](size_t total, const data_chunk& element)
    {
        // Tokens encoded as variable integer prefixed byte array (bip144).
        const auto size = element.size();
        return total + message::variable_uint_size(size) + size;
    };

    return std::accumulate(stack.begin(), stack.end(), size_t(0), sum);
}

// protected
void witness::reset()
{
    valid_ = false;
    stack_.clear();
    stack_.shrink_to_fit();
}

bool witness::is_valid() const
{
    // Witness validity is consistent with stack validity (unlike script).
    return valid_;
}

// Serialization.
//-----------------------------------------------------------------------------

data_chunk witness::to_data(bool prefix) const
{
    data_chunk data;
    const auto size = serialized_size(prefix);
    data.reserve(size);
    data_sink ostream(data);
    to_data(ostream, prefix);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == size);
    return data;
}

void witness::to_data(std::ostream& stream, bool prefix) const
{
    ostream_writer sink(stream);
    to_data(sink, prefix);
}

void witness::to_data(writer& sink, bool prefix) const
{
    // Witness prefix is an element count, not byte length (unlike script).
    if (prefix)
        sink.write_variable_little_endian(stack_.size());

    const auto serialize = [&sink](const data_chunk& element)
    {
        // Tokens encoded as variable integer prefixed byte array (bip144).
        sink.write_variable_little_endian(element.size());
        sink.write_bytes(element);
    };

    // TODO: optimize store serialization to avoid loop, writing data directly.
    std::for_each(stack_.begin(), stack_.end(), serialize);
}

std::string witness::to_string() const
{
    if (!valid_)
        return "<invalid>";

    std::string text;
    const auto serialize = [&text](const data_chunk& element)
    {
        text += "[" + encode_base16(element) + "] ";
    };

    std::for_each(stack_.begin(), stack_.end(), serialize);
    return boost::trim_copy(text);
}

// Iteration.
//-----------------------------------------------------------------------------
// These are syntactic sugar that allow the caller to iterate stack directly.

void witness::clear()
{
    reset();
}

bool witness::empty() const
{
    return stack_.empty();
}

size_t witness::size() const
{
    return stack_.size();
}

const data_chunk& witness::front() const
{
    BITCOIN_ASSERT(!stack_.empty());
    return stack_.front();
}

const data_chunk& witness::back() const
{
    BITCOIN_ASSERT(!stack_.empty());
    return stack_.back();
}

const data_chunk& witness::operator[](size_t index) const
{
    BITCOIN_ASSERT(index < stack_.size());
    return stack_[index];
}

witness::iterator witness::begin() const
{
    return stack_.begin();
}

witness::iterator witness::end() const
{
    return stack_.end();
}

// Properties (size).
//-----------------------------------------------------------------------------

size_t witness::serialized_size(bool prefix) const
{
    // Witness prefix is an element count, not a byte length (unlike script).
    return (prefix ? message::variable_uint_size(stack_.size()) : 0u) +
        serialized_size(stack_);
}

const data_stack& witness::stack() const
{
    return stack_;
}

// Utilities.
//-----------------------------------------------------------------------------

// static
bool witness::is_push_size(const data_stack& stack)
{
    const auto push_size = [](const data_chunk& element)
    {
        return element.size() <= max_push_data_size;
    };

    return std::all_of(stack.begin(), stack.end(), push_size);
}

// static
// The (only) coinbase witness must be (arbitrary) 32-byte value (bip141).
bool witness::is_reserved_pattern(const data_stack& stack)
{
    return stack.size() == 1 &&
        stack[0].size() == hash_size;
}

// private
// This is an internal optimization over using script::to_pay_key_hash_pattern.
operation::list witness::to_pay_key_hash(data_chunk&& program)
{
    BITCOIN_ASSERT(program.size() == short_hash_size);

    return operation::list
    {
        { opcode::dup },
        { opcode::hash160 },
        { std::move(program) },
        { opcode::equalverify },
        { opcode::checksig }
    };
}

// The return script is useful only for sigop counting.
// Returns true if is a witness program - even if potentially invalid.
bool witness::extract_sigop_script(script& out_script,
    const script& program_script) const
{
    out_script.clear();

    switch (program_script.version())
    {
        case script_version::zero:
        {
            switch (program_script.witness_program().size())
            {
                case short_hash_size:
                    out_script.from_operations({ { opcode::checksig } });
                    return true;

                case hash_size:
                    if (!stack_.empty())
                        out_script.from_data(stack_.back(), false);

                    return true;

                default:
                    return true;
            }
        }

        case script_version::reserved:
            return true;

        case script_version::unversioned:
        default:
            return false;
    }
}

// Extract P2WPKH or P2WSH script as indicated by program script.
bool witness::extract_embedded_script(script& out_script,
    data_stack& out_stack, const script& program_script) const
{
    switch (program_script.version())
    {
        // The v0 program size must be either 20 or 32 bytes (bip141).
        case script_version::zero:
        {
            auto program = program_script.witness_program();
            const auto program_size = program.size();
            out_stack = stack_;

            // always: <signature> <pubkey>
            if (program_size == short_hash_size)
            {
                // Stack must be 2 elements, within push size limit (bip141).
                if (out_stack.size() != 2 || !is_push_size(out_stack))
                    return false;

                // The hash160 of public key must match the program (bip141).
                // This is enforced by script evaluation, so optimized out here.
                ////if (!std::equal(program.begin(), program.end(),
                ////    bitcoin_short_hash(out_stack[1]).begin()))
                ////    return false;

                // The script is derived from the stack (bip141).
                out_script.from_operations(to_pay_key_hash(std::move(program)));
                return true;
            }

            // example: 0 <signature1> <1 <pubkey1> <pubkey2> 2 CHECKMULTISIG>
            if (program_size == hash_size)
            {
                // The witness must consist of at least 1 item (bip141).
                if (out_stack.empty())
                    return false;

                // The script is popped off the initial witness stack (bip141).
                out_script.from_data(pop(out_stack), false);

                // Stack elements must be within push size limit (bip141).
                if (!is_push_size(out_stack))
                    return false;

                // The SHA256 of the witness script must match program (bip141).
                return std::equal(program.begin(), program.end(),
                    sha256_hash(out_script.to_data(false)).begin());
            }

            return false;
        }

        // These versions are reserved for future extensions (bip141).
        case script_version::reserved:
            return true;

        case script_version::unversioned:
        default:
            return false;
    }
}

// Validation.
//-----------------------------------------------------------------------------

// static
// The program script is either a prevout script or an embedded script.
// It validates this witness, from which the witness script is derived.
code witness::verify(const transaction& tx, uint32_t input_index,
    uint32_t forks, const script& program_script, uint64_t value) const
{
    const auto version = program_script.version();

    switch (version)
    {
        case script_version::zero:
        {
            code ec;
            script script;
            data_stack stack;

            if (!extract_embedded_script(script, stack, program_script))
                return error::invalid_witness;

            program witness(script, tx, input_index, forks, std::move(stack),
                value, version);

            if ((ec = witness.evaluate()))
                return ec;

            // A v0 script must succeed with a clean true stack (bip141).
            if (!witness.stack_result(true))
                return error::stack_false;
        }

        // These versions are reserved for future extensions (bip141).
        case script_version::reserved:
            return error::success;

        case script_version::unversioned:
        default:
            return error::operation_failed;
    }
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
