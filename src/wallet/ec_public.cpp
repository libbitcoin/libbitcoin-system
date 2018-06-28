/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/wallet/ec_public.hpp>

#include <algorithm>
#include <iostream>
#include <boost/program_options.hpp>
#include <bitcoin/bitcoin/formats/base_16.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/wallet/ec_private.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>

namespace libbitcoin {
namespace wallet {

const uint8_t ec_public::compressed_even = 0x02;
const uint8_t ec_public::compressed_odd = 0x03;
const uint8_t ec_public::uncompressed = 0x04;
const uint8_t ec_public::mainnet_p2kh = 0x00;

ec_public::ec_public()
 : valid_(false), compress_(true), point_(null_compressed_point)
{
}

ec_public::ec_public(const ec_public& other)
  : valid_(other.valid_), compress_(other.compress_), point_(other.point_)
{
}

ec_public::ec_public(const ec_private& secret)
  : ec_public(from_private(secret))
{
}

ec_public::ec_public(const data_chunk& decoded)
  : ec_public(from_data(decoded))
{
}

ec_public::ec_public(const std::string& base16)
  : ec_public(from_string(base16))
{
}

ec_public::ec_public(const ec_uncompressed& point, bool compress)
  : ec_public(from_point(point, compress))
{
}

ec_public::ec_public(const ec_compressed& point, bool compress)
  : valid_(true), compress_(compress), point_(point)
{
}

// Validators.
// ----------------------------------------------------------------------------

bool ec_public::is_point(data_slice decoded)
{
    return bc::is_public_key(decoded);
}

// Factories.
// ----------------------------------------------------------------------------

ec_public ec_public::from_private(const ec_private& secret)
{
    if (!secret)
        return ec_public();

    return ec_public(secret.to_public());
}

ec_public ec_public::from_string(const std::string& base16)
{
    data_chunk decoded;
    if (!decode_base16(decoded, base16))
        return ec_public();

    return ec_public(decoded);
}

ec_public ec_public::from_data(const data_chunk& decoded)
{
    if (!is_point(decoded))
        return ec_public();

    if (is_compressed_key(decoded))
        return ec_public(to_array<ec_compressed_size>(decoded), true);

    ec_compressed compressed;
    return bc::compress(compressed, to_array<ec_uncompressed_size>(decoded)) ?
        ec_public(compressed, false) : ec_public();
}

ec_public ec_public::from_point(const ec_uncompressed& point, bool compress)
{
    if (!is_point(point))
        return ec_public();

    ec_compressed compressed;
    return bc::compress(compressed, point) ? ec_public(compressed, compress) :
        ec_public();
}

// Cast operators.
// ----------------------------------------------------------------------------

ec_public::operator bool() const
{
    return valid_;
}

ec_public::operator const ec_compressed&() const
{
    return point_;
}

// Serializer.
// ----------------------------------------------------------------------------

std::string ec_public::encoded() const
{
    if (compressed())
        return encode_base16(point_);

    // If the point is valid it should always decompress, but if not, is null.
    ec_uncompressed uncompressed(null_uncompressed_point);
    to_uncompressed(uncompressed);
    return encode_base16(uncompressed);
}

// Accessors.
// ----------------------------------------------------------------------------

const ec_compressed& ec_public::point() const
{
    return point_;
}

bool ec_public::compressed() const
{
    return compress_;
}

// Methods.
// ----------------------------------------------------------------------------

bool ec_public::to_data(data_chunk& out) const
{
    if (!valid_)
        return false;

    if (compressed())
    {
        out.resize(ec_compressed_size);
        std::copy_n(point_.begin(), ec_compressed_size, out.begin());
        return true;
    }

    ec_uncompressed uncompressed;
    if (to_uncompressed(uncompressed))
    {
        out.resize(ec_uncompressed_size);
        std::copy_n(uncompressed.begin(), ec_uncompressed_size, out.begin());
        return true;
    }

    return false;
}

bool ec_public::to_uncompressed(ec_uncompressed& out) const
{
    if (!valid_)
        return false;

    return bc::decompress(out, to_array<ec_compressed_size>(point_));
}

payment_address ec_public::to_payment_address(uint8_t version) const
{
    return payment_address(*this, version);
}

// Operators.
// ----------------------------------------------------------------------------

ec_public& ec_public::operator=(const ec_public& other)
{
    valid_ = other.valid_;
    compress_ = other.compress_;
    point_ = other.point_;
    return *this;
}

bool ec_public::operator<(const ec_public& other) const
{
    return encoded() < other.encoded();
}

bool ec_public::operator==(const ec_public& other) const
{
    return valid_ == other.valid_ && compress_ == other.compress_ &&
        point_ == other.point_;
}

bool ec_public::operator!=(const ec_public& other) const
{
    return !(*this == other);
}

std::istream& operator>>(std::istream& in, ec_public& to)
{
    std::string value;
    in >> value;
    to = ec_public(value);

    if (!to)
    {
        using namespace boost::program_options;
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const ec_public& of)
{
    out << of.encoded();
    return out;
}

} // namespace wallet
} // namespace libbitcoin
