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
namespace secp256k1 {
namespace cuda {

#if defined(HAVE_CUDA)

// Interface.
// ----------------------------------------------------------------------------

bool compiled() NOEXCEPT
{
    return true;
}

bool available() NOEXCEPT
{
    return context::instance().available();
}

// Rows are gathered into the columns that the device stages.
template <typename Batch, typename Key>
static bool verify_rows(data_chunk& out, const stopper& cancel,
    const Batch& batch) NOEXCEPT
{
    const auto count = batch.rows.size();
    hashes digests(count);
    std_vector<Key> points(count);
    ec_signatures signatures(count);
    for (size_t row{}; row < count; ++row)
    {
        digests[row] = batch.rows[row].digest;
        points[row] = batch.rows[row].point;
        signatures[row] = batch.rows[row].signature;
    }

    return context::instance().verify<Key>(out, cancel, digests, points,
        signatures, true);
}

bool verify(data_chunk& out, const stopper& cancel,
    const ecdsa::batch& batch) NOEXCEPT
{
    return verify_rows<ecdsa::batch, ec_compressed>(out, cancel, batch);
}

bool verify(data_chunk& out, const stopper& cancel,
    const schnorr::batch& batch) NOEXCEPT
{
    return verify_rows<schnorr::batch, ec_xonly>(out, cancel, batch);
}

bool scan(std::vector<prefix>& prefixes, data_chunk& valid,
    const stopper& cancel, const std::span<const ec_compressed>& summaries,
    const silent::batch::receiver& keys) NOEXCEPT
{
    return context::instance().scan(prefixes, valid, cancel, summaries,
        keys);
}

#else

bool compiled() NOEXCEPT
{
    return false;
}

bool available() NOEXCEPT
{
    return false;
}

LCOV_EXCL_START("Not called where the device is not available.")
bool verify(data_chunk&, const stopper&, const ecdsa::batch&) NOEXCEPT
{
    return false;
}

bool verify(data_chunk&, const stopper&, const schnorr::batch&) NOEXCEPT
{
    return false;
}

bool scan(std::vector<prefix>&, data_chunk&, const stopper&,
    const std::span<const ec_compressed>&,
    const silent::batch::receiver&) NOEXCEPT
{
    return false;
}
LCOV_EXCL_STOP()

#endif

} // namespace cuda
} // namespace secp256k1
} // namespace system
} // namespace libbitcoin
