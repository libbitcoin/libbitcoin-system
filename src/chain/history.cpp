/////**
//// * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
//// *
//// * This file is part of libbitcoin.
//// *
//// * This program is free software: you can redistribute it and/or modify
//// * it under the terms of the GNU Affero General Public License as published by
//// * the Free Software Foundation, either version 3 of the License, or
//// * (at your option) any later version.
//// *
//// * This program is distributed in the hope that it will be useful,
//// * but WITHOUT ANY WARRANTY; without even the implied warranty of
//// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// * GNU Affero General Public License for more details.
//// *
//// * You should have received a copy of the GNU Affero General Public License
//// * along with this program.  If not, see <http://www.gnu.org/licenses/>.
//// */
////#include <bitcoin/bitcoin/chain/output.hpp>
////
////#include <cstdint>
////#include <sstream>
////#include <bitcoin/bitcoin/constants.hpp>
////#include <bitcoin/bitcoin/utility/container_sink.hpp>
////#include <bitcoin/bitcoin/utility/container_source.hpp>
////#include <bitcoin/bitcoin/utility/istream_reader.hpp>
////#include <bitcoin/bitcoin/utility/ostream_writer.hpp>
////#include <bitcoin/bitcoin/wallet/payment_address.hpp>
////
////namespace libbitcoin {
////namespace chain {
////
////// Constructors.
//////-----------------------------------------------------------------------------
////
////history::history()
////  : value_(not_found),
////    script_{},
////    validation{}
////{
////}
////
////history::history(output&& other)
////  : addresses_(other.addresses_cache()),
////    value_(other.value_),
////    script_(std::move(other.script_)),
////    validation(other.validation)
////{
////}
////
////history::history(const output& other)
////  : addresses_(other.addresses_cache()),
////    value_(other.value_),
////    script_(other.script_),
////    validation(other.validation)
////{
////}
////
////history::history(uint64_t value, chain::script&& script)
////  : value_(value),
////    script_(std::move(script)),
////    validation{}
////{
////}
////
////history::history(uint64_t value, const chain::script& script)
////  : value_(value),
////    script_(script),
////    validation{}
////{
////}
////
////// Operators.
//////-----------------------------------------------------------------------------
////
////history& history::operator=(output&& other)
////{
////    addresses_ = other.addresses_cache();
////    value_ = other.value_;
////    script_ = std::move(other.script_);
////    validation = std::move(other.validation);
////    return *this;
////}
////
////history& history::operator=(const output& other)
////{
////    addresses_ = other.addresses_cache();
////    value_ = other.value_;
////    script_ = other.script_;
////    validation = other.validation;
////    return *this;
////}
////
////bool history::operator==(const history& other) const
////{
////    return (value_ == other.value_) && (script_ == other.script_);
////}
////
////bool history::operator!=(const history& other) const
////{
////    return !(*this == other);
////}
////
////// Deserialization.
//////-----------------------------------------------------------------------------
////
////history history::factory(const data_chunk& data, bool wire)
////{
////    output instance;
////    instance.from_data(data, wire);
////    return instance;
////}
////
////history history::factory(std::istream& stream, bool wire)
////{
////    output instance;
////    instance.from_data(stream, wire);
////    return instance;
////}
////
////history history::factory(reader& source, bool wire)
////{
////    output instance;
////    instance.from_data(source, wire);
////    return instance;
////}
////
////bool history::from_data(const data_chunk& data, bool wire)
////{
////    data_source istream(data);
////    return from_data(istream, wire);
////}
////
////bool history::from_data(std::istream& stream, bool wire)
////{
////    istream_reader source(stream);
////    return from_data(source, wire);
////}
////
////bool history::from_data(reader& source, bool wire)
////{
////    reset();
////
////    if (!wire)
////        validation.spender_height = source.read_4_bytes_little_endian();
////
////    value_ = source.read_8_bytes_little_endian();
////    script_.from_data(source, true);
////
////    if (!source)
////        reset();
////
////    return source;
////}
////
////// protected
////void history::reset()
////{
////    value_ = history::not_found;
////    script_.reset();
////}
////
////// Empty scripts are valid, validation relies on not_found only.
////bool history::is_valid() const
////{
////    return value_ != history::not_found;
////}
////
////// Serialization.
//////-----------------------------------------------------------------------------
////
////data_chunk history::to_data(bool wire) const
////{
////    data_chunk data;
////    const auto size = serialized_size(wire);
////    data.reserve(size);
////    data_sink ostream(data);
////    to_data(ostream, wire);
////    ostream.flush();
////    BITCOIN_ASSERT(data.size() == size);
////    return data;
////}
////
////void history::to_data(std::ostream& stream, bool wire) const
////{
////    ostream_writer sink(stream);
////    to_data(sink, wire);
////}
////
////void history::to_data(writer& sink, bool wire) const
////{
////    if (!wire)
////    {
////        auto height32 = safe_unsigned<uint32_t>(validation.spender_height);
////        sink.write_4_bytes_little_endian(height32);
////    }
////
////    sink.write_8_bytes_little_endian(value_);
////    script_.to_data(sink, true);
////}
////
////// Size.
//////-----------------------------------------------------------------------------
////
////size_t history::serialized_size(bool wire) const
////{
////    // validation.spender_height is size_t stored as uint32_t.
////    return (wire ? 0 : sizeof(uint32_t)) + sizeof(value_) +
////        script_.serialized_size(true);
////}
////
////// Accessors.
//////-----------------------------------------------------------------------------
////
////uint64_t history::value() const
////{
////    return value_;
////}
////
////void history::set_value(uint64_t value)
////{
////    value_ = value;
////}
////
////const chain::script& history::script() const
////{
////    return script_;
////}
////
////void history::set_script(const chain::script& value)
////{
////    script_ = value;
////    invalidate_cache();
////}
////
////void history::set_script(chain::script&& value)
////{
////    script_ = std::move(value);
////    invalidate_cache();
////}
////
////} // namespace chain
////} // namespace libbitcoin
