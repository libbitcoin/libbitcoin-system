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
#include <bitcoin/system/crypto/secp256k1/algorithm.hpp>
#include "kernels.hpp"

namespace libbitcoin {
namespace system {
namespace secp256k1 {

class verifier
  : public algorithm
{
public:
    using algorithm::slice_count;
    using algorithm::table_words;
    using algorithm::comb_part_count;

    /// Entries and words of a comb table part.
    static constexpr size_t comb_part_size = comb_part_windows * comb_size;
    static constexpr size_t comb_part_words = comb_part_size * comb_words;

    /// Point window bits, uniform across the threads of a warp.
    static constexpr size_t window_bits = 4;

    static __device__ uint32_t row() NOEXCEPT
    {
        return __nvvm_read_ptx_sreg_ctaid_x() * __nvvm_read_ptx_sreg_ntid_x() +
            __nvvm_read_ptx_sreg_tid_x();
    }

    static __device__ uint8_t verify(const hash_digest& digest,
        const ec_compressed& key, const ec_signature& signature) NOEXCEPT
    {
        affine_t<uint64_t> point{};
        if (!decode(point, key))
            return 0;

        scalar_t z{};
        /* bool */ from_bytes(z, digest);
        return verify_ecdsa<window_bits>(point, z,
            array_cast<uint64_t, array_count<scalar_t>>(signature),
            array_cast<uint64_t, array_count<scalar_t>, sizeof(scalar_t)>(
                signature)) ? 1 : 0;
    }

    static __device__ uint8_t verify(const hash_digest& message,
        const ec_xonly& key, const ec_signature& signature) NOEXCEPT
    {
        constexpr auto size = array_count<bytes_t>;
        const auto& r = array_cast<uint8_t, size>(signature);
        const auto& s = array_cast<uint8_t, size, size>(signature);
        const auto challenge = sha256::hash(
            tagged_midstate<"BIP0340/challenge">, r, key, message);

        return verify_schnorr<window_bits>(key, challenge, r, s) ? 1 : 0;
    }

    /// The output key prefix of the receiver for the point (k = 0), then of
    /// that key plus each label key [bip352].
    static __device__ uint8_t scan(cuda::prefix* prefixes,
        const ec_compressed& encoded,
        const cuda::scan_arguments& arguments) NOEXCEPT
    {
        affine_t<uint64_t> point{};
        if (!decode(point, encoded))
            return 0;

        // The device computes in variable time, a server-side optimization
        // that is opted out of by not compiling CUDA.
        jacobian_t<uint64_t> sum{};
        const scalars_t<uint64_t> ks{ arguments.scan };
        const auto uncomputed = multiply_windows<window_bits, false, false>(sum, {},
            point, ks);
        if (f::any(uncomputed))
            multiply_complete(sum, {}, point, arguments.scan);

        if (f::any(sum.infinity))
            return 0;

        // The tagged hash of the shared point and k = 0, as four zero bytes.
        affine_t<uint64_t> shared{};
        ec_compressed key{};
        data_array<ec_compressed_size + sizeof(uint32_t)> data{};
        to_affine(shared, sum);
        to_bytes(key, shared);
        for (size_t byte{}; byte < key.size(); ++byte)
            data[byte] = key[byte];

        scalar_t tweak{};
        constexpr auto midstate = tagged_midstate<"BIP0352/SharedSecret">;
        const auto hash = sha256::hash(midstate, data);
        if (!from_bytes(tweak, hash) || is_zero_scalar(tweak))
            return 0;

        uint64_t faults{};
        sum = {};
        sum.infinity = max_uint64;
        add_comb(sum, tweak, faults);
        add_point<false>(sum, arguments.spend, faults);
        if (is_nonzero(faults))
            multiply_complete(sum, tweak, arguments.spend, { 1 });

        if (f::any(sum.infinity))
            return 0;

        write(prefixes[0], sum);
        for (size_t index{}; index < arguments.label_count; ++index)
        {
            jacobian_t<uint64_t> labeled{};
            add_complete(labeled, sum, arguments.labels[index]);
            if (f::any(labeled.infinity))
                return 0;

            write(prefixes[add1(index)], labeled);
        }

        return 1;
    }

    /// The point, hash * sum [bip352].
    static __device__ uint8_t compute(ec_compressed& out,
        const ec_compressed& sum, const ec_secret& hash) NOEXCEPT
    {
        affine_t<uint64_t> point{};
        if (!decode(point, sum))
            return 0;

        scalar_t k{};
        if (!from_bytes(k, hash) || is_zero_scalar(k))
            return 0;

        jacobian_t<uint64_t> product{};
        const scalars_t<uint64_t> ks{ k };
        const auto uncomputed = multiply_windows<window_bits, false, false>(
            product, {}, point, ks);
        if (f::any(uncomputed))
            multiply_complete(product, {}, point, k);

        if (f::any(product.infinity))
            return 0;

        affine_t<uint64_t> normal{};
        ec_compressed key{};
        to_affine(normal, product);
        to_bytes(key, normal);
        for (size_t byte{}; byte < key.size(); ++byte)
            out[byte] = key[byte];

        return 1;
    }

private:
    // Key rows are unaligned (33 bytes), so x is decoded from a copy.
    static __device__ bool decode(affine_t<uint64_t>& out,
        const ec_compressed& key) NOEXCEPT
    {
        const auto sign = key.front();
        if (sign != ec_even_sign && sign != ec_odd_sign)
            return false;

        alignas(sizeof(uint64_t)) bytes_t x_bytes{};
        for (size_t byte{}; byte < x_bytes.size(); ++byte)
            x_bytes[byte] = key[add1(byte)];

        field_t<uint64_t> x{};
        const auto odd = sign == ec_odd_sign ? max_uint64 : 0_u64;
        return from_bytes(x, x_bytes) && is_nonzero(lift(out, x, odd));
    }

    static __device__ void write(cuda::prefix& out,
        const jacobian_t<uint64_t>& point) NOEXCEPT
    {
        affine_t<uint64_t> normal{};
        bytes_t x{};
        to_affine(normal, point);
        to_bytes(x, normal.x);
        for (size_t byte{}; byte < out.size(); ++byte)
            out[byte] = x[byte];
    }
};

extern "C"
{
    __device__ uint64_t generator_table[verifier::slice_count]
        [verifier::table_words];
    __device__ uint64_t comb_table[verifier::comb_part_count]
        [verifier::comb_part_words];
}

__device__ const std_array<const uint64_t*, 16> generator_slices
{
    generator_table[0], generator_table[1], generator_table[2],
    generator_table[3], generator_table[4], generator_table[5],
    generator_table[6], generator_table[7], generator_table[8],
    generator_table[9], generator_table[10], generator_table[11],
    generator_table[12], generator_table[13], generator_table[14],
    generator_table[15]
};

__device__ const std_array<const uint64_t*, verifier::comb_part_count>
    comb_parts
{
    comb_table[0], comb_table[1], comb_table[2]
};

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

using namespace libbitcoin::system::secp256k1;

extern "C" __global__ void verify_ecdsa(cuda::ecdsa_arguments arguments)
{
    const auto row = verifier::row();
    if (row < arguments.count)
        arguments.results[row] = verifier::verify(arguments.digests[row],
            arguments.keys[row], arguments.signatures[row]);
}

extern "C" __global__ void verify_schnorr(cuda::schnorr_arguments arguments)
{
    const auto row = verifier::row();
    if (row < arguments.count)
        arguments.results[row] = verifier::verify(arguments.messages[row],
            arguments.keys[row], arguments.signatures[row]);
}

extern "C" __global__ void scan_silent(cuda::scan_arguments arguments)
{
    const auto row = verifier::row();
    if (row < arguments.count)
        arguments.valid[row] = verifier::scan(
            &arguments.prefixes[row * (arguments.label_count + 1u)],
            arguments.points[row], arguments);
}

extern "C" __global__ void compute_silent(
    cuda::silent_arguments arguments)
{
    const auto row = verifier::row();
    if (row < arguments.count)
        arguments.valid[row] = verifier::compute(
            arguments.points[row], arguments.sums[row],
            arguments.hashes[row]);
}
