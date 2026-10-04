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

bool verify(data_chunk& out, const stopper& cancel,
    const ecdsa::batch& batch) NOEXCEPT
{
    return context::instance().verify(out, cancel, batch.digests,
        batch.points, batch.signatures, true);
}

bool verify(data_chunk& out, const stopper& cancel,
    const schnorr::batch& batch) NOEXCEPT
{
    return context::instance().verify(out, cancel, batch.digests,
        batch.points, batch.signatures, true);
}

bool scan(std::vector<prefix>& prefixes, data_chunk& valid,
    const stopper& cancel, const std::span<const ec_compressed>& summaries,
    const silent::batch::receiver& keys) NOEXCEPT
{
    if (keys.labels.size() > maximum_labels)
        return false;

    silent_arguments arguments{};
    arguments.scan = keys.scan;
    arguments.label_count = possible_narrow_cast<uint32_t>(keys.labels.size());
    arguments.spend = keys.spend;
    std::copy(keys.labels.cbegin(), keys.labels.cend(),
        arguments.labels.begin());

    return context::instance().scan(prefixes, valid, cancel, summaries,
        arguments);
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
