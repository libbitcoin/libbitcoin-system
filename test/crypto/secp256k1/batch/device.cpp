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
#include "../../../test.hpp"

BC_AUTO_TEST_SUITE(secp256k1_batch_device_tests)

BC_AUTO_TEST_CASE(secp256k1_batch_device__cuda_ecc__always__implies_cuda_device)
{
    const auto ecc = cuda_ecc();
    BC_REQUIRE(!ecc || cuda_device());
}

BC_AUTO_TEST_CASE(secp256k1_batch_device__cuda_ecc_enabled__always__implies_cuda_ecc)
{
    const auto enabled = cuda_ecc_enabled();
    BC_REQUIRE(!enabled || cuda_ecc());
}

BC_AUTO_TEST_SUITE_END()
