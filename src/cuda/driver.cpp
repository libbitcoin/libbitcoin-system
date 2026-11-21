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
#include "driver.hpp"

#include <atomic>
#include <span>
#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include "kernels.hpp"

#if defined(HAVE_CUDA)
    #include "context.hpp"
#endif

namespace libbitcoin {
namespace system {
namespace cuda {

#if defined(HAVE_CUDA)

// Interface.
// ----------------------------------------------------------------------------

static std::atomic_bool gate{ true };

bool compiled() NOEXCEPT
{
    return true;
}

void enable(bool enabled) NOEXCEPT
{
    gate.store(enabled);
}

bool available() NOEXCEPT
{
    return gate.load() && context::instance().available();
}

bool verify(const stopper& cancel, data_chunk& out,
    const ecdsa::batch& batch) NOEXCEPT
{
    return context::instance().verify(cancel, out, batch.digests,
        batch.points, batch.signatures, true);
}

bool verify(const stopper& cancel, data_chunk& out,
    const schnorr::batch& batch) NOEXCEPT
{
    return context::instance().verify(cancel, out, batch.digests,
        batch.points, batch.signatures, true);
}

bool scan(const stopper& cancel, std::vector<prefix>& prefixes,
    data_chunk& valid, const std::span<const ec_compressed>& points,
    const scan::batch::receiver& keys) NOEXCEPT
{
    return context::instance().scan(cancel, prefixes, valid, points, keys);
}

bool compute(const stopper& cancel, const std::span<ec_compressed>& out,
    const std::span<uint8_t>& valid,
    const std::span<const ec_compressed>& sums,
    const std::span<const ec_secret>& hashes) NOEXCEPT
{
    return context::instance().compute(cancel, out, valid, sums, hashes);
}

bool label(pool::report_t& out, const std::span<uint32_t>& cells,
    const std::span<const pool::algorithm::edges_t>& edges,
    const std::span<const uint32_t>& rows,
    const std::span<const uint32_t>& rivals) NOEXCEPT
{
    return available() &&
        context::instance().label(out, cells, edges, rows, rivals);
}

bool settle(pool::report_t& out, const std::span<uint32_t>& cells,
    const std::span<const pool::algorithm::edges_t>& edges,
    const std::span<const uint32_t>& rows,
    const std::span<const uint32_t>& rivals) NOEXCEPT
{
    return available() &&
        context::instance().settle(out, cells, edges, rows, rivals);
}

#else

bool compiled() NOEXCEPT
{
    return false;
}

void enable(bool) NOEXCEPT
{
}

bool available() NOEXCEPT
{
    return false;
}

LCOV_EXCL_START("Not called where gpu is not available.")
bool verify(const stopper&, data_chunk&, const ecdsa::batch&) NOEXCEPT
{
    return false;
}

bool verify(const stopper&, data_chunk&, const schnorr::batch&) NOEXCEPT
{
    return false;
}

bool scan(const stopper&, std::vector<prefix>&, data_chunk&,
    const std::span<const ec_compressed>&,
    const scan::batch::receiver&) NOEXCEPT
{
    return false;
}

bool compute(const stopper&, const std::span<ec_compressed>&,
    const std::span<uint8_t>&, const std::span<const ec_compressed>&,
    const std::span<const ec_secret>&) NOEXCEPT
{
    return false;
}

bool label(pool::report_t&, const std::span<uint32_t>&,
    const std::span<const pool::algorithm::edges_t>&,
    const std::span<const uint32_t>&,
    const std::span<const uint32_t>&) NOEXCEPT
{
    return false;
}

bool settle(pool::report_t&, const std::span<uint32_t>&,
    const std::span<const pool::algorithm::edges_t>&,
    const std::span<const uint32_t>&,
    const std::span<const uint32_t>&) NOEXCEPT
{
    return false;
}
LCOV_EXCL_STOP()

#endif

} // namespace cuda
} // namespace system
} // namespace libbitcoin
