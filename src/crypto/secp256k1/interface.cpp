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
#include "interface.hpp"

#include <algorithm>
#include <new>
#include <bitcoin/system/crypto/maybe_random.hpp>
#include <bitcoin/system/crypto/secp256k1/algorithm.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>

#if !defined(HAVE_SECP256K1) && !defined(HAVE_ULTRAFAST)

BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_ARRAY_TO_POINTER_DECAY)

struct secp256k1_context
{
    unsigned int flags{};
};

namespace libbitcoin {
namespace system {
namespace secp256k1 {

// local
// ----------------------------------------------------------------------------

constexpr auto success = 1;
constexpr auto failure = 0;

class local
  : public algorithm
{
public:
    using affine = affine_t<uint64_t>;
    using jacobian = jacobian_t<uint64_t>;
    using field = field_t<uint64_t>;
    using scalar = scalar_t;
    using bytes = bytes_t;

    using algorithm::add;
    using algorithm::add_complete;
    using algorithm::from_bytes;
    using algorithm::inverse;
    using algorithm::is_high;
    using algorithm::is_odd_element;
    using algorithm::is_zero_scalar;
    using algorithm::lift;
    using algorithm::linear;
    using algorithm::multiply;
    using algorithm::negate;
    using algorithm::nonce_schnorr;
    using algorithm::normalize;
    using algorithm::recover;
    using algorithm::secret_multiply;
    using algorithm::swift_decode;
    using algorithm::swift_fraction;
    using algorithm::to_affine;
    using algorithm::verify_ecdsa;
    using algorithm::verify_schnorr;

    // Representations.
    // ------------------------------------------------------------------------
    // Opaque values hold 64 bit words in native byte order.

    template <size_t Offset, size_t Size>
    static scalar get_words(const data_array<Size>& in) NOEXCEPT
    {
        return array_cast<uint64_t, array_count<scalar>, Offset>(in);
    }

    template <size_t Offset, size_t Size>
    static void set_words(data_array<Size>& out, const scalar& words) NOEXCEPT
    {
        array_cast<uint64_t, array_count<scalar>, Offset>(out) = words;
    }

    // A stored point has nonzero x.
    template <size_t Offset, size_t Size>
    static bool load(affine& r, const data_array<Size>& in) NOEXCEPT
    {
        constexpr auto size = array_count<bytes>;
        const auto x = get_words<Offset>(in);
        const auto y = get_words<Offset + size>(in);
        to_field(r.x, x);
        to_field(r.y, y);
        return !is_zero_scalar(x);
    }

    template <size_t Offset, size_t Size>
    static void save(data_array<Size>& out, affine point) NOEXCEPT
    {
        constexpr auto size = array_count<bytes>;
        scalar x{}, y{};
        normalize(point.x);
        normalize(point.y);
        to_words(x, point.x);
        to_words(y, point.y);
        set_words<Offset>(out, x);
        set_words<Offset + size>(out, y);
    }

    template <size_t Size>
    static void load(scalar& r, scalar& s, const data_array<Size>& in) NOEXCEPT
    {
        r = get_words<zero>(in);
        s = get_words<array_count<bytes>>(in);
    }

    template <size_t Size>
    static void save(data_array<Size>& out, const scalar& r,
        const scalar& s) NOEXCEPT
    {
        set_words<zero>(out, r);
        set_words<array_count<bytes>>(out, s);
    }

    static const bytes& to_array(const uint8_t* data) NOEXCEPT
    {
        return unsafe_array_cast<uint8_t, array_count<bytes>>(data);
    }

    // Value of bytes mod n.
    static scalar to_scalar(const uint8_t* data) NOEXCEPT
    {
        scalar out{};
        /* bool */ from_bytes(out, to_array(data));
        return out;
    }

    // Value of bytes if below n (out reduced otherwise).
    static bool to_scalar(scalar& out, const uint8_t* data) NOEXCEPT
    {
        return from_bytes(out, to_array(data));
    }

    // Value of bytes if nonzero and below n.
    static bool to_secret(scalar& out, const uint8_t* data) NOEXCEPT
    {
        return from_bytes(out, to_array(data)) && !is_zero_scalar(out);
    }

    // Value of bytes mod p (weak).
    static field to_element(const uint8_t* data) NOEXCEPT
    {
        field out{};
        /* bool */ from_bytes(out, to_array(data));
        return out;
    }

    static void to_bytes(uint8_t* out, const scalar& value) NOEXCEPT
    {
        bytes data{};
        algorithm::to_bytes(data, value);
        std::copy(data.begin(), data.end(), out);
    }

    static void to_bytes(uint8_t* out, field value) NOEXCEPT
    {
        bytes data{};
        normalize(value);
        algorithm::to_bytes(data, value);
        std::copy(data.begin(), data.end(), out);
    }

    // Blinds.
    // ------------------------------------------------------------------------

    // Nonzero scalar keyed by the secret, over entropy and a context, so that
    // it remains secret where entropy is not.
    static scalar blind(const scalar& secret, const data_slice& context,
        uint8_t tag) NOEXCEPT
    {
        bytes key{}, entropy{};
        algorithm::to_bytes(key, secret);
        maybe_random::fill(entropy);

        LCOV_EXCL_START("Retry requires a hash of zero or above n.")
        for (uint8_t counter{};; ++counter)
        {
            hmac<sha256> mac{ key };
            mac.write(entropy);
            mac.write(context);
            mac.write(data_array<2>{ tag, counter });

            scalar out{};
            if (from_bytes(out, mac.flush()) && !is_zero_scalar(out))
                return out;
        }
        LCOV_EXCL_STOP()
    }

    // Nonces.
    // ------------------------------------------------------------------------

    // HMAC-SHA256 generator of RFC6979 3.2, over key, message mod n, and
    // optional data and algorithm, returning the output of attempt + 1 calls.
    static int rfc6979(uint8_t* nonce32, const uint8_t* msg32,
        const uint8_t* key32, const uint8_t* algo16, void* data,
        unsigned int attempt) NOEXCEPT
    {
        constexpr data_array<1> zero_byte{ 0x00 };
        constexpr data_array<1> one_byte{ 0x01 };

        bytes message{};
        algorithm::to_bytes(message, to_scalar(msg32));

        data_chunk seed{};
        seed.reserve(112);
        const auto& key = to_array(key32);
        seed.insert(seed.end(), key.begin(), key.end());
        seed.insert(seed.end(), message.begin(), message.end());
        if (!is_null(data))
        {
            LCOV_EXCL_START("Wrappers provide no extra data.")
            const auto& extra = to_array(static_cast<const uint8_t*>(data));
            seed.insert(seed.end(), extra.begin(), extra.end());
            LCOV_EXCL_STOP()
        }

        if (!is_null(algo16))
        {
            LCOV_EXCL_START("Wrappers provide no algorithm.")
            seed.insert(seed.end(), algo16, std::next(algo16, 16));
            LCOV_EXCL_STOP()
        }

        hash_digest k{}, v{};
        v.fill(0x01);

        const auto update = [&](const data_array<1>& separator,
            const data_slice& input) NOEXCEPT
        {
            hmac<sha256> keyed{ k };
            keyed.write(v);
            keyed.write(separator);
            keyed.write(input);
            k = keyed.flush();
            v = hmac<sha256>::code(v, k);
        };

        update(zero_byte, seed);
        update(one_byte, seed);

        LCOV_EXCL_START("Retry requires a nonce of zero or above n.")
        for (unsigned int call{};; ++call)
        {
            if (is_nonzero(call))
                update(zero_byte, {});

            v = hmac<sha256>::code(v, k);
            if (call == attempt)
                break;
        }
        LCOV_EXCL_STOP()

        std::copy(v.begin(), v.end(), nonce32);
        return success;
    }

    // BIP340 nonce of secret, x-only public key and message, where the secret
    // is masked by the tagged hash of the auxiliary data (zeros if null).
    static hash_digest bip340(const bytes& secret, const bytes& key,
        const uint8_t* msg32, const uint8_t* aux32) NOEXCEPT
    {
        constexpr bytes zeros{};
        accumulator<sha256> auxiliary{ tagged_midstate<"BIP0340/aux">, one };
        auxiliary.write(is_null(aux32) ? zeros : to_array(aux32));
        auto masked = auxiliary.flush();
        for (size_t index{}; index < masked.size(); ++index)
            masked[index] = bit_xor(masked[index], secret[index]);

        accumulator<sha256> nonce{ tagged_midstate<"BIP0340/nonce">, one };
        nonce.write(masked);
        nonce.write(key);
        nonce.write(to_array(msg32));
        return nonce.flush();
    }

    static scalar challenge(const bytes& r_x, const bytes& key,
        const data_slice& message) NOEXCEPT
    {
        accumulator<sha256> hasher{ tagged_midstate<"BIP0340/challenge">,
            one };
        hasher.write(r_x);
        hasher.write(key);
        hasher.write(message.size(), message.data());

        scalar out{};
        /* bool */ from_bytes(out, hasher.flush());
        return out;
    }

    static int bip324(uint8_t* output, const uint8_t* x32,
        const uint8_t* ell_a64, const uint8_t* ell_b64, void*) NOEXCEPT
    {
        accumulator<sha256> hasher{ tagged_midstate<
            "bip324_ellswift_xonly_ecdh">, one };
        hasher.write(unsafe_array_cast<uint8_t, ec_ellswift_size>(ell_a64));
        hasher.write(unsafe_array_cast<uint8_t, ec_ellswift_size>(ell_b64));
        hasher.write(to_array(x32));
        hasher.flush(output);
        return success;
    }

    // ECDSA.
    // ------------------------------------------------------------------------

    // An invalid secret signs as one, with false returned and zeroed output.
    static bool sign(scalar& r, scalar& s, uint8_t& id, const uint8_t* msg32,
        const uint8_t* seckey, secp256k1_nonce_function noncefp,
        const void* ndata) NOEXCEPT
    {
        scalar d{};
        const auto valid = to_secret(d, seckey);
        if (!valid)
            d = { 1 };

        const auto nonce = is_null(noncefp) ? &rfc6979 : noncefp;
        const auto z = to_scalar(msg32);
        auto signed_ = false;
        const auto message = data_slice{ to_array(msg32) };

        LCOV_EXCL_START("Retry requires a nonce of zero or above n.")
        for (unsigned int attempt{};; ++attempt)
        {
            bytes nonce32{};
            if (is_zero(nonce(nonce32.data(), msg32, seckey, nullptr,
                const_cast<void*>(ndata), attempt)))
                break;

            scalar k{};
            if (!to_secret(k, nonce32.data()))
                continue;

            const auto m = blind(d, message, 0);
            const auto b = blind(d, message, 1);
            if (sign_ecdsa(r, s, id, d, z, k, m, b))
            {
                signed_ = true;
                break;
            }
        }
        LCOV_EXCL_STOP()

        if (!signed_ || !valid)
        {
            r = {};
            s = {};
            id = 0;
            return false;
        }

        return true;
    }

    // Schnorr.
    // ------------------------------------------------------------------------

    // Keypair is secret (big-endian) then public key, a failed keypair loads
    // as (1, G) and false.
    static bool load(scalar& secret, affine& point,
        const secp256k1_keypair& keypair) NOEXCEPT
    {
        if (load<array_count<bytes>>(point, keypair.data) &&
            to_secret(secret, keypair.data.data()))
            return true;

        LCOV_EXCL_START("Wrappers create keypairs from valid secrets.")
        secret = { 1 };
        point = generator;
        return false;
        LCOV_EXCL_STOP()
    }

    // Elligator swift.
    // ------------------------------------------------------------------------

    static void random(bytes& out, const accumulator<sha256>& hasher,
        uint32_t counter) NOEXCEPT
    {
        auto copy = hasher;
        copy.write(to_little_endian(counter));
        copy.flush(out.data());
    }

    // Draws u and branches from hasher until the branch inverts x (u), then
    // matches the parity of t to that of y.
    static void encode(uint8_t* ell64, const affine& point,
        const accumulator<sha256>& hasher) NOEXCEPT
    {
        bytes branches{}, u32{};
        field t{};
        size_t remaining{};
        uint32_t counter{};

        while (true)
        {
            if (is_zero(remaining))
            {
                random(branches, hasher, counter++);
                remaining = 64;
            }

            --remaining;
            const auto half = branches[remaining / two];
            const auto branch = narrow_cast<uint8_t>(
                (half >> (is_odd(remaining) ? 4u : 0u)) & 7u);

            random(u32, hasher, counter++);
            auto u = to_element(u32.data());
            normalize(u);
            if (swift_inverse(t, point.x, u, branch))
                break;
        }

        if (f::any(is_odd_element(t)) != f::any(is_odd_element(point.y)))
        {
            negate(t, t);
            normalize(t);
        }

        std::copy(u32.begin(), u32.end(), ell64);
        to_bytes(std::next(ell64, array_count<bytes>), t);
    }
};

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

using namespace libbitcoin;
using namespace libbitcoin::system;
using namespace libbitcoin::system::secp256k1;

// Nonce and hash functions.
// ----------------------------------------------------------------------------

const secp256k1_nonce_function secp256k1_nonce_function_rfc6979 =
    &local::rfc6979;

const secp256k1_ellswift_xdh_hash_function
    secp256k1_ellswift_xdh_hash_function_bip324 = &local::bip324;

// Context.
// ----------------------------------------------------------------------------

secp256k1_context* secp256k1_context_create(unsigned int flags) NOEXCEPT
{
    BC_PUSH_WARNING(NO_NEW_OR_DELETE)
    return new (std::nothrow) secp256k1_context{ flags };
    BC_POP_WARNING()
}

void secp256k1_context_destroy(secp256k1_context* ctx) NOEXCEPT
{
    BC_PUSH_WARNING(NO_NEW_OR_DELETE)
    delete ctx;
    BC_POP_WARNING()
}

// Keys.
// ----------------------------------------------------------------------------

int secp256k1_ec_pubkey_parse(const secp256k1_context*,
    secp256k1_pubkey* pubkey, const uint8_t* input, size_t inputlen) NOEXCEPT
{
    pubkey->data = {};
    if (is_null(input))
        return failure;

    local::affine point{};
    if (inputlen == ec_compressed_size)
    {
        const auto& key = unsafe_array_cast<uint8_t, ec_compressed_size>(input);
        if (!local::from_bytes(point, key))
            return failure;
    }
    else if (inputlen == ec_uncompressed_size)
    {
        const auto& key = unsafe_array_cast<uint8_t, ec_uncompressed_size>(
            input);
        if (!local::from_bytes(point, key))
            return failure;
    }
    else
    {
        return failure;
    }

    local::save<zero>(pubkey->data, point);
    return success;
}

int secp256k1_ec_pubkey_serialize(const secp256k1_context*, uint8_t* output,
    size_t* outputlen, const secp256k1_pubkey* pubkey,
    unsigned int flags) NOEXCEPT
{
    constexpr auto size = array_count<local::bytes>;
    const auto compressed = is_nonzero(flags &
        SECP256K1_FLAGS_BIT_COMPRESSION);
    const auto required = compressed ? ec_compressed_size :
        ec_uncompressed_size;

    if (is_null(outputlen) || *outputlen < required || is_null(output))
        return failure;

    std::fill_n(output, *outputlen, uint8_t{});
    *outputlen = 0;
    if ((flags & SECP256K1_FLAGS_TYPE_MASK) != SECP256K1_FLAGS_TYPE_COMPRESSION)
        return failure;

    local::affine point{};
    if (!local::load<zero>(point, pubkey->data))
        return failure;

    local::to_bytes(std::next(output), point.x);
    if (compressed)
    {
        output[0] = f::any(local::is_odd_element(point.y)) ? ec_odd_sign :
            ec_even_sign;
    }
    else
    {
        output[0] = ec_uncompressed_sign;
        local::to_bytes(std::next(output, add1(size)), point.y);
    }

    *outputlen = required;
    return success;
}

int secp256k1_ec_seckey_verify(const secp256k1_context*,
    const uint8_t* seckey) NOEXCEPT
{
    local::scalar secret{};
    return local::to_secret(secret, seckey) ? success : failure;
}

int secp256k1_ec_pubkey_create(const secp256k1_context*,
    secp256k1_pubkey* pubkey, const uint8_t* seckey) NOEXCEPT
{
    pubkey->data = {};
    local::scalar secret{};
    if (!local::to_secret(secret, seckey))
        return failure;

    local::affine point{};
    local::secret_multiply(point, secret, local::blind(secret, {}, 0));
    local::save<zero>(pubkey->data, point);
    return success;
}

int secp256k1_ec_seckey_negate(const secp256k1_context*,
    uint8_t* seckey) NOEXCEPT
{
    local::scalar secret{};
    const auto valid = local::to_secret(secret, seckey);
    if (!valid)
        secret = {};

    local::negate(secret, secret);
    local::to_bytes(seckey, secret);
    return valid ? success : failure;
}

int secp256k1_ec_pubkey_negate(const secp256k1_context*,
    secp256k1_pubkey* pubkey) NOEXCEPT
{
    local::affine point{};
    const auto valid = local::load<zero>(point, pubkey->data);
    pubkey->data = {};
    if (!valid)
        return failure;

    local::negate(point, point);
    local::save<zero>(pubkey->data, point);
    return success;
}

int secp256k1_ec_seckey_tweak_add(const secp256k1_context*, uint8_t* seckey,
    const uint8_t* tweak32) NOEXCEPT
{
    local::scalar secret{}, tweak{};
    auto valid = local::to_secret(secret, seckey);
    valid &= local::to_scalar(tweak, tweak32);
    local::add(secret, secret, tweak);
    valid &= !local::is_zero_scalar(secret);
    if (!valid)
        secret = {};

    local::to_bytes(seckey, secret);
    return valid ? success : failure;
}

int secp256k1_ec_pubkey_tweak_add(const secp256k1_context*,
    secp256k1_pubkey* pubkey, const uint8_t* tweak32) NOEXCEPT
{
    local::affine point{}, sum{};
    local::scalar tweak{};
    const auto valid = local::load<zero>(point, pubkey->data);
    pubkey->data = {};
    if (!valid || !local::to_scalar(tweak, tweak32) ||
        !local::linear(sum, tweak, point))
        return failure;

    local::save<zero>(pubkey->data, sum);
    return success;
}

int secp256k1_ec_seckey_tweak_mul(const secp256k1_context*, uint8_t* seckey,
    const uint8_t* tweak32) NOEXCEPT
{
    local::scalar secret{}, tweak{};
    auto valid = local::to_secret(secret, seckey);
    valid &= local::to_scalar(tweak, tweak32);
    valid &= !local::is_zero_scalar(tweak);
    local::multiply(secret, secret, tweak);
    if (!valid)
        secret = {};

    local::to_bytes(seckey, secret);
    return valid ? success : failure;
}

int secp256k1_ec_pubkey_tweak_mul(const secp256k1_context*,
    secp256k1_pubkey* pubkey, const uint8_t* tweak32) NOEXCEPT
{
    local::affine point{}, product{};
    local::scalar tweak{};
    const auto valid = local::to_scalar(tweak, tweak32) &&
        local::load<zero>(point, pubkey->data);

    pubkey->data = {};
    if (!valid || local::is_zero_scalar(tweak) ||
        !local::linear(product, {}, point, tweak))
        return failure;

    local::save<zero>(pubkey->data, product);
    return success;
}

int secp256k1_ec_pubkey_combine(const secp256k1_context*,
    secp256k1_pubkey* out, const secp256k1_pubkey* const* ins,
    size_t n) NOEXCEPT
{
    out->data = {};
    if (is_zero(n) || is_null(ins))
        return failure;

    local::jacobian sum{};
    sum.infinity = max_uint64;
    for (size_t index{}; index < n; ++index)
    {
        local::affine point{};
        if (is_null(ins[index]) || !local::load<zero>(point, ins[index]->data))
            return failure;

        local::add_complete(sum, sum, point);
    }

    if (f::any(sum.infinity))
        return failure;

    local::affine point{};
    local::to_affine(point, sum);
    local::save<zero>(out->data, point);
    return success;
}

// ECDSA.
// ----------------------------------------------------------------------------

int secp256k1_ecdsa_signature_parse_compact(const secp256k1_context*,
    secp256k1_ecdsa_signature* sig, const uint8_t* input64) NOEXCEPT
{
    local::scalar r{}, s{};
    if (!local::to_scalar(r, input64) ||
        !local::to_scalar(s, std::next(input64, array_count<local::bytes>)))
    {
        sig->data = {};
        return failure;
    }

    local::save(sig->data, r, s);
    return success;
}

// Big-endian integers without leading zeros, other than one where the high
// bit is set.
int secp256k1_ecdsa_signature_serialize_der(const secp256k1_context*,
    uint8_t* output, size_t* outputlen,
    const secp256k1_ecdsa_signature* sig) NOEXCEPT
{
    constexpr auto size = array_count<local::bytes>;
    local::scalar r{}, s{};
    local::load(r, s, sig->data);

    data_array<add1(size)> r_bytes{}, s_bytes{};
    local::to_bytes(std::next(r_bytes.data()), r);
    local::to_bytes(std::next(s_bytes.data()), s);

    const auto trim = [](const data_array<add1(size)>& value) NOEXCEPT
    {
        size_t start{};
        while (start < size && is_zero(value[start]) &&
            value[add1(start)] < 0x80u)
            ++start;

        return start;
    };

    const auto r_start = trim(r_bytes);
    const auto s_start = trim(s_bytes);
    const auto r_size = add1(size) - r_start;
    const auto s_size = add1(size) - s_start;
    const auto total = 6u + r_size + s_size;
    if (*outputlen < total)
    {
        LCOV_EXCL_START("Wrappers provide the maximal buffer.")
        *outputlen = total;
        return failure;
        LCOV_EXCL_STOP()
    }

    *outputlen = total;
    output[0] = 0x30;
    output[1] = narrow_cast<uint8_t>(4u + r_size + s_size);
    output[2] = 0x02;
    output[3] = narrow_cast<uint8_t>(r_size);
    std::copy_n(std::next(r_bytes.begin(), r_start), r_size,
        std::next(output, 4));
    output[4 + r_size] = 0x02;
    output[5 + r_size] = narrow_cast<uint8_t>(s_size);
    std::copy_n(std::next(s_bytes.begin(), s_start), s_size,
        std::next(output, 6 + r_size));
    return success;
}

int secp256k1_ecdsa_signature_serialize_compact(const secp256k1_context*,
    uint8_t* output64, const secp256k1_ecdsa_signature* sig) NOEXCEPT
{
    local::scalar r{}, s{};
    local::load(r, s, sig->data);
    local::to_bytes(output64, r);
    local::to_bytes(std::next(output64, array_count<local::bytes>), s);
    return success;
}

int secp256k1_ecdsa_signature_normalize(const secp256k1_context*,
    secp256k1_ecdsa_signature* sigout,
    const secp256k1_ecdsa_signature* sigin) NOEXCEPT
{
    local::scalar r{}, s{};
    local::load(r, s, sigin->data);
    const auto high = local::is_high(s);
    if (!is_null(sigout))
    {
        if (high)
            local::negate(s, s);

        local::save(sigout->data, r, s);
    }

    return high ? success : failure;
}

int secp256k1_ecdsa_verify(const secp256k1_context*,
    const secp256k1_ecdsa_signature* sig, const uint8_t* msghash32,
    const secp256k1_pubkey* pubkey) NOEXCEPT
{
    local::scalar r{}, s{};
    local::affine point{};
    local::load(r, s, sig->data);
    return !local::is_high(s) && local::load<zero>(point, pubkey->data) &&
        local::verify_ecdsa(point, local::to_scalar(msghash32), r, s) ?
            success : failure;
}

int secp256k1_ecdsa_sign(const secp256k1_context*,
    secp256k1_ecdsa_signature* sig, const uint8_t* msghash32,
    const uint8_t* seckey, secp256k1_nonce_function noncefp,
    const void* ndata) NOEXCEPT
{
    local::scalar r{}, s{};
    uint8_t id{};
    const auto valid = local::sign(r, s, id, msghash32, seckey, noncefp,
        ndata);

    local::save(sig->data, r, s);
    return valid ? success : failure;
}

// ECDSA recovery.
// ----------------------------------------------------------------------------

int secp256k1_ecdsa_recoverable_signature_parse_compact(
    const secp256k1_context*, secp256k1_ecdsa_recoverable_signature* sig,
    const uint8_t* input64, int recid) NOEXCEPT
{
    local::scalar r{}, s{};
    if (recid < 0 || recid > 3 || !local::to_scalar(r, input64) ||
        !local::to_scalar(s, std::next(input64, array_count<local::bytes>)))
    {
        sig->data = {};
        return failure;
    }

    local::save(sig->data, r, s);
    sig->data.back() = narrow_sign_cast<uint8_t>(recid);
    return success;
}

int secp256k1_ecdsa_recoverable_signature_serialize_compact(
    const secp256k1_context*, uint8_t* output64, int* recid,
    const secp256k1_ecdsa_recoverable_signature* sig) NOEXCEPT
{
    local::scalar r{}, s{};
    local::load(r, s, sig->data);
    local::to_bytes(output64, r);
    local::to_bytes(std::next(output64, array_count<local::bytes>), s);
    *recid = sig->data.back();
    return success;
}

int secp256k1_ecdsa_sign_recoverable(const secp256k1_context*,
    secp256k1_ecdsa_recoverable_signature* sig, const uint8_t* msghash32,
    const uint8_t* seckey, secp256k1_nonce_function noncefp,
    const void* ndata) NOEXCEPT
{
    local::scalar r{}, s{};
    uint8_t id{};
    const auto valid = local::sign(r, s, id, msghash32, seckey, noncefp,
        ndata);

    local::save(sig->data, r, s);
    sig->data.back() = id;
    return valid ? success : failure;
}

int secp256k1_ecdsa_recover(const secp256k1_context*,
    secp256k1_pubkey* pubkey, const secp256k1_ecdsa_recoverable_signature* sig,
    const uint8_t* msghash32) NOEXCEPT
{
    local::scalar r{}, s{};
    local::affine point{};
    local::load(r, s, sig->data);
    if (!local::recover(point, local::to_scalar(msghash32), r, s,
        sig->data.back()))
    {
        pubkey->data = {};
        return failure;
    }

    local::save<zero>(pubkey->data, point);
    return success;
}

// Extra keys.
// ----------------------------------------------------------------------------

int secp256k1_xonly_pubkey_parse(const secp256k1_context*,
    secp256k1_xonly_pubkey* pubkey, const uint8_t* input32) NOEXCEPT
{
    pubkey->data = {};
    if (is_null(input32))
        return failure;

    local::field x{};
    local::affine point{};
    if (!local::from_bytes(x, local::to_array(input32)) ||
        !f::any(local::lift(point, x, 0_u64)))
        return failure;

    local::save<zero>(pubkey->data, point);
    return success;
}

int secp256k1_xonly_pubkey_tweak_add_check(const secp256k1_context*,
    const uint8_t* tweaked_pubkey32, int tweaked_pk_parity,
    const secp256k1_xonly_pubkey* internal_pubkey,
    const uint8_t* tweak32) NOEXCEPT
{
    local::affine point{}, sum{};
    local::scalar tweak{};
    if (!local::load<zero>(point, internal_pubkey->data) ||
        !local::to_scalar(tweak, tweak32) ||
        !local::linear(sum, tweak, point))
        return failure;

    local::bytes x{};
    local::to_bytes(x.data(), sum.x);
    const auto odd = f::any(local::is_odd_element(sum.y));
    return x == local::to_array(tweaked_pubkey32) &&
        odd == is_nonzero(tweaked_pk_parity) ? success : failure;
}

int secp256k1_keypair_create(const secp256k1_context*,
    secp256k1_keypair* keypair, const uint8_t* seckey) NOEXCEPT
{
    keypair->data = {};
    local::scalar secret{};
    if (!local::to_secret(secret, seckey))
        return failure;

    local::affine point{};
    local::secret_multiply(point, secret, local::blind(secret, {}, 0));
    local::to_bytes(keypair->data.data(), secret);
    local::save<array_count<local::bytes>>(keypair->data, point);
    return success;
}

// Schnorr.
// ----------------------------------------------------------------------------

// The secret is negated where its point has odd y, as the key is x-only.
int secp256k1_schnorrsig_sign32(const secp256k1_context*, uint8_t* sig64,
    const uint8_t* msg32, const secp256k1_keypair* keypair,
    const uint8_t* aux_rand32) NOEXCEPT
{
    local::scalar secret{};
    local::affine point{};
    auto valid = local::load(secret, point, *keypair);
    if (f::any(local::is_odd_element(point.y)))
        local::negate(secret, secret);

    local::bytes secret_bytes{}, key{}, r_x{};
    local::to_bytes(secret_bytes.data(), secret);
    local::to_bytes(key.data(), point.x);

    local::scalar nonce{};
    local::from_bytes(nonce, local::bip340(secret_bytes, key, msg32,
        aux_rand32));

    valid &= !local::is_zero_scalar(nonce);
    if (!valid)
    {
        LCOV_EXCL_START("Requires an invalid keypair or a zero nonce.")
        nonce = { 1 };
        LCOV_EXCL_STOP()
    }

    const auto message = data_slice{ local::to_array(msg32) };
    local::nonce_schnorr(r_x, nonce, local::blind(secret, message, 0));

    local::scalar s{};
    local::multiply(s, local::challenge(r_x, key, message), secret);
    local::add(s, s, nonce);

    constexpr auto size = array_count<local::bytes>;
    std::copy(r_x.begin(), r_x.end(), sig64);
    local::to_bytes(std::next(sig64, size), s);
    if (!valid)
    {
        LCOV_EXCL_START("Requires an invalid keypair or a zero nonce.")
        std::fill_n(sig64, two * size, uint8_t{});
        LCOV_EXCL_STOP()
    }

    return valid ? success : failure;
}

int secp256k1_schnorrsig_verify(const secp256k1_context*,
    const uint8_t* sig64, const uint8_t* msg, size_t msglen,
    const secp256k1_xonly_pubkey* pubkey) NOEXCEPT
{
    constexpr auto size = array_count<local::bytes>;
    local::field r_x{};
    local::scalar s{};
    local::affine point{};
    if (!local::from_bytes(r_x, local::to_array(sig64)) ||
        !local::to_scalar(s, std::next(sig64, size)) ||
        !local::load<zero>(point, pubkey->data))
        return failure;

    local::bytes key{};
    local::to_bytes(key.data(), point.x);
    const auto message = is_zero(msglen) ? data_slice{} :
        data_slice{ msg, std::next(msg, msglen) };

    return local::verify_schnorr(point, local::challenge(
        local::to_array(sig64), key, message), r_x, s) ? success : failure;
}

// ElligatorSwift.
// ----------------------------------------------------------------------------

int secp256k1_ellswift_decode(const secp256k1_context*,
    secp256k1_pubkey* pubkey, const uint8_t* ell64) NOEXCEPT
{
    local::affine point{};
    local::swift_decode(point, local::to_element(ell64),
        local::to_element(std::next(ell64, array_count<local::bytes>)));

    local::save<zero>(pubkey->data, point);
    return success;
}

// An invalid secret encodes the key of one, with false returned and zeroed
// output. Randomness is H(secret || zeros || auxiliary || counter).
int secp256k1_ellswift_create(const secp256k1_context*, uint8_t* ell64,
    const uint8_t* seckey32, const uint8_t* auxrnd32) NOEXCEPT
{
    constexpr local::bytes zeros{};
    local::scalar secret{};
    const auto valid = local::to_secret(secret, seckey32);
    if (!valid)
        secret = { 1 };

    local::affine point{};
    local::secret_multiply(point, secret, local::blind(secret, {}, 0));

    accumulator<sha256> hasher{ tagged_midstate<"secp256k1_ellswift_create">,
        one };
    hasher.write(local::to_array(seckey32));
    hasher.write(zeros);
    if (!is_null(auxrnd32))
        hasher.write(local::to_array(auxrnd32));

    local::encode(ell64, point, hasher);
    if (!valid)
        std::fill_n(ell64, ec_ellswift_size, uint8_t{});

    return valid ? success : failure;
}

// The shared x is that of secret times their point, whose x is decoded as a
// fraction (y does not affect x of the product). An invalid secret is one.
int secp256k1_ellswift_xdh(const secp256k1_context*, uint8_t* output,
    const uint8_t* ell_a64, const uint8_t* ell_b64, const uint8_t* seckey32,
    int party, secp256k1_ellswift_xdh_hash_function hashfp,
    void* data) NOEXCEPT
{
    const auto theirs = is_zero(party) ? ell_b64 : ell_a64;

    local::field xn{}, xd{}, x{};
    local::swift_fraction(xn, xd, local::to_element(theirs),
        local::to_element(std::next(theirs, array_count<local::bytes>)));

    local::inverse(xd, xd);
    local::multiply(x, xn, xd);
    local::normalize(x);

    local::affine point{}, shared{};
    /* bool */ local::lift(point, x, 0_u64);

    local::scalar secret{};
    const auto valid = local::to_secret(secret, seckey32);
    if (!valid)
        secret = { 1 };

    const auto context = unsafe_array_cast<uint8_t, ec_ellswift_size>(theirs);
    local::secret_multiply(shared, secret, point,
        local::blind(secret, context, 0));

    local::bytes shared_x{};
    local::to_bytes(shared_x.data(), shared.x);
    const auto hashed = hashfp(output, shared_x.data(), ell_a64, ell_b64,
        data);

    return is_nonzero(hashed) && valid ? success : failure;
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

#endif
