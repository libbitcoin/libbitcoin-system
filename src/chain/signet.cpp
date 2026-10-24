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
#include <bitcoin/system/chain/signet.hpp>

#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/enums/flags.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/enums/opcode.hpp>
#include <bitcoin/system/chain/header.hpp>
#include <bitcoin/system/chain/input.hpp>
#include <bitcoin/system/chain/operation.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/transaction.hpp>
#include <bitcoin/system/chain/witness.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

bool signet::verify(const chain::header& header, const transaction& coinbase,
    hashes&& txids, const script& challenge) NOEXCEPT
{
    const auto spend = to_sign(header, coinbase, std::move(txids), challenge);
    if (!spend.is_valid())
        return false;

    constexpr auto rules = bip16_rule | bip66_rule | bip141_rule |
        bip143_rule | bip147_rule;

    const context state{ rules, 0, 0, 0, 0, 0, 0 };
    return !spend.connect(state);
}

// protected
// ----------------------------------------------------------------------------

// The first push of head and solution is cleared to head [bip325].
bool signet::get_solution(data_chunk& solution, data_chunk& cleared,
    const script& commitment) NOEXCEPT
{
    constexpr auto head = to_big_endian(signet_head);
    auto found = false;
    for (const auto& op: commitment.ops())
    {
        if (op.is_underflow())
            break;

        const auto& data = op.data();
        if (data.empty())
        {
            cleared.push_back(static_cast<uint8_t>(op.code()));
        }
        else if (!found && data.size() > head.size() &&
            std::equal(head.cbegin(), head.cend(), data.cbegin()))
        {
            found = true;
            solution.assign(std::next(data.cbegin(), head.size()), data.cend());
            extend(cleared, operation{ to_chunk(head), false }.to_data());
        }
        else
        {
            extend(cleared, operation{ data, false }.to_data());
        }
    }

    return found;
}

transaction signet::to_spend(const chain::header& header,
    const hash_digest& root, const script& challenge) NOEXCEPT
{
    data_chunk preimage(two * sizeof(uint32_t) + two * hash_size);
    stream::out::fast ostream(preimage);
    write::bytes::fast out(ostream);
    out.write_4_bytes_little_endian(header.version());
    out.write_bytes(header.previous_block_hash());
    out.write_bytes(root);
    out.write_4_bytes_little_endian(header.timestamp());

    const operations ops{ { opcode::push_size_0 }, { preimage, false } };
    return
    {
        0,
        chain::inputs
        {
            {
                point{ null_hash, point::null_index },
                script{ ops },
                0
            }
        },
        chain::outputs{ { 0, challenge } },
        0
    };
}

transaction signet::to_sign(const chain::header& header,
    const transaction& coinbase, hashes&& txids,
    const script& challenge) NOEXCEPT
{
    if (txids.empty())
        return {};

    const auto& outs = *coinbase.outputs_ptr();
    const auto commitment = std::find_if(outs.crbegin(), outs.crend(),
        [](const auto& out) NOEXCEPT
        {
            return script::is_commitment_pattern(out->script().ops());
        });

    // The witness commitment is required [bip325].
    if (commitment == outs.crend())
        return {};

    // An absent solution is an empty script and witness [bip325].
    data_chunk solution{};
    data_chunk cleared{};
    script sign_script{};
    witness sign_witness{};
    output_cptrs replaced{ outs };
    if (get_solution(solution, cleared, (*commitment)->script()))
    {
        stream::in::fast stream{ solution };
        read::bytes::fast source{ stream };
        sign_script = script{ source, true };
        sign_witness = witness{ source, true };
        if (!source || !source.is_exhausted())
            return {};

        const auto position = std::distance(std::next(commitment),
            outs.crend());
        *std::next(replaced.begin(), position) = to_shared<output>(
            (*commitment)->value(), script{ cleared, false });
    }

    const transaction modified
    {
        coinbase.version(),
        coinbase.inputs_ptr(),
        to_shared(std::move(replaced)),
        coinbase.locktime()
    };

    txids.front() = modified.hash(false);
    const auto root = sha256::merkle_root(std::move(txids));
    const auto spend = to_spend(header, root, challenge);

    transaction sign
    {
        0,
        chain::inputs
        {
            {
                point{ spend.hash(false), 0 },
                std::move(sign_script),
                std::move(sign_witness),
                0
            }
        },
        chain::outputs{ { 0, script{ operations{ { opcode::op_return } } } } },
        0
    };

    sign.inputs_ptr()->front()->prevout = spend.outputs_ptr()->front();
    return sign;
}

BC_POP_WARNING()

} // namespace chain
} // namespace system
} // namespace libbitcoin
