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
#ifndef LIBBBITCOIN__MESSAGE_REJECT_REASON_CODE_HPP
#define LIBBBITCOIN__MESSAGE_REJECT_REASON_CODE_HPP

//#include <bitcoin/message/reject.hpp>
#include <cstdint>

namespace libbitcoin {
namespace api {

//enum message_reject_reason_code : message::reject::reason_code {
//public:
//	message::reject::reason_code getValue() {
//        return value;
//    }
//
//    void setValue(message::reject::reason_code value) {
//        this->value = value;
//    }
//private:
//    message::reject::reason_code value;

//};

enum class message_reject_reason_code: uint8_t
{
    /// The reason code is not defined.
    undefined = 0x00,

    /// The message was malformed.
    malformed = 0x01,

    /// In response to block or tx message: invalid (data is hash).
    invalid = 0x10,

    /// In response to version message: version.
    /// In respose to block message: version (data is hash).
    obsolete = 0x11,

    /// In respose to tx message: double spend (data is hash).
    /// In respose to version message: more than one received.
    duplicate = 0x12,

    /// In respose to tx message: nonstandard (data is hash).
    nonstandard = 0x40,

    /// In respose to tx message: dust output(s) (data is hash).
    dust = 0x41,

    /// In respose to tx message: insufficient fee (data is hash).
    insufficient_fee = 0x42,

    /// In response to block message: failed checkpoint (data is hash).
    checkpoint = 0x43
};


} // namespace api
} // namespace libbitcoin

#endif
