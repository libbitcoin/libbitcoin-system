/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/chain/script/opcode.hpp>

#include <sstream>
#include <bitcoin/bitcoin/constants.hpp>

namespace libbitcoin {
namespace chain {

std::string opcode_to_string(opcode value, uint32_t flags)
{
    switch (value)
    {
        case opcode::zero:
            return "zero";
        case opcode::special:
            return "special";
        case opcode::pushdata1:
            return "pushdata1";
        case opcode::pushdata2:
            return "pushdata2";
        case opcode::pushdata4:
            return "pushdata4";
        case opcode::negative_1:
            return "-1";
        case opcode::reserved:
            return "reserved";
        case opcode::op_1:
            return "1";
        case opcode::op_2:
            return "2";
        case opcode::op_3:
            return "3";
        case opcode::op_4:
            return "4";
        case opcode::op_5:
            return "5";
        case opcode::op_6:
            return "6";
        case opcode::op_7:
            return "7";
        case opcode::op_8:
            return "8";
        case opcode::op_9:
            return "9";
        case opcode::op_10:
            return "10";
        case opcode::op_11:
            return "11";
        case opcode::op_12:
            return "12";
        case opcode::op_13:
            return "13";
        case opcode::op_14:
            return "14";
        case opcode::op_15:
            return "15";
        case opcode::op_16:
            return "16";
        case opcode::nop:
            return "nop";
        case opcode::ver:
            return "ver";
        case opcode::if_:
            return "if";
        case opcode::notif:
            return "notif";
        case opcode::verif:
            return "verif";
        case opcode::vernotif:
            return "vernotif";
        case opcode::else_:
            return "else";
        case opcode::endif:
            return "endif";
        case opcode::verify:
            return "verify";
        case opcode::return_:
            return "return";
        case opcode::toaltstack:
            return "toaltstack";
        case opcode::fromaltstack:
            return "fromaltstack";
        case opcode::op_2drop:
            return "2drop";
        case opcode::op_2dup:
            return "2dup";
        case opcode::op_3dup:
            return "3dup";
        case opcode::op_2over:
            return "2over";
        case opcode::op_2rot:
            return "2rot";
        case opcode::op_2swap:
            return "2swap";
        case opcode::ifdup:
            return "ifdup";
        case opcode::depth:
            return "depth";
        case opcode::drop:
            return "drop";
        case opcode::dup:
            return "dup";
        case opcode::nip:
            return "nip";
        case opcode::over:
            return "over";
        case opcode::pick:
            return "pick";
        case opcode::roll:
            return "roll";
        case opcode::rot:
            return "rot";
        case opcode::swap:
            return "swap";
        case opcode::tuck:
            return "tuck";
        case opcode::cat:
            return "cat";
        case opcode::substr:
            return "substr";
        case opcode::left:
            return "left";
        case opcode::right:
            return "right";
        case opcode::size:
            return "size";
        case opcode::invert:
            return "invert";
        case opcode::and_:
            return "and";
        case opcode::or_:
            return "or";
        case opcode::xor_:
            return "xor";
        case opcode::equal:
            return "equal";
        case opcode::equalverify:
            return "equalverify";
        case opcode::reserved1:
            return "reserved1";
        case opcode::reserved2:
            return "reserved2";
        case opcode::op_1add:
            return "1add";
        case opcode::op_1sub:
            return "1sub";
        case opcode::op_2mul:
            return "2mul";
        case opcode::op_2div:
            return "2div";
        case opcode::negate:
            return "negate";
        case opcode::abs:
            return "abs";
        case opcode::not_:
            return "not";
        case opcode::op_0notequal:
            return "0notequal";
        case opcode::add:
            return "add";
        case opcode::sub:
            return "sub";
        case opcode::mul:
            return "mul";
        case opcode::div:
            return "div";
        case opcode::mod:
            return "mod";
        case opcode::lshift:
            return "lshift";
        case opcode::rshift:
            return "rshift";
        case opcode::booland:
            return "booland";
        case opcode::boolor:
            return "boolor";
        case opcode::numequal:
            return "numequal";
        case opcode::numequalverify:
            return "numequalverify";
        case opcode::numnotequal:
            return "numnotequal";
        case opcode::lessthan:
            return "lessthan";
        case opcode::greaterthan:
            return "greaterthan";
        case opcode::lessthanorequal:
            return "lessthanorequal";
        case opcode::greaterthanorequal:
            return "greaterthanorequal";
        case opcode::min:
            return "min";
        case opcode::max:
            return "max";
        case opcode::within:
            return "within";
        case opcode::ripemd160:
            return "ripemd160";
        case opcode::sha1:
            return "sha1";
        case opcode::sha256:
            return "sha256";
        case opcode::hash160:
            return "hash160";
        case opcode::hash256:
            return "hash256";
        case opcode::codeseparator:
            return "codeseparator";
        case opcode::checksig:
            return "checksig";
        case opcode::checksigverify:
            return "checksigverify";
        case opcode::checkmultisig:
            return "checkmultisig";
        case opcode::checkmultisigverify:
            return "checkmultisigverify";
        case opcode::op_nop1:
            return "nop1";
        case opcode::op_nop2:
            return "nop2";
        case opcode::op_nop3:
            return "nop3";
        case opcode::op_nop4:
            return "nop4";
        case opcode::op_nop5:
            return "nop5";
        case opcode::op_nop6:
            return "nop6";
        case opcode::op_nop7:
            return "nop7";
        case opcode::op_nop8:
            return "nop8";
        case opcode::op_nop9:
            return "nop9";
        case opcode::op_nop10:
            return "nop10";
        case opcode::raw_data:
            return "raw_data";
        default:
        {
            std::ostringstream ss;
            ss << "<none " << static_cast<int>(value) << ">";
            return ss.str();
        }
    }
}

opcode string_to_opcode(const std::string& value)
{
    if (value == "zero")
        return opcode::zero;
    else if (value == "special")
        return opcode::special;
    else if (value == "pushdata1")
        return opcode::pushdata1;
    else if (value == "pushdata2")
        return opcode::pushdata2;
    else if (value == "pushdata4")
        return opcode::pushdata4;
    else if (value == "-1")
        return opcode::negative_1;
    else if (value == "reserved")
        return opcode::reserved;
    else if (value == "1")
        return opcode::op_1;
    else if (value == "2")
        return opcode::op_2;
    else if (value == "3")
        return opcode::op_3;
    else if (value == "4")
        return opcode::op_4;
    else if (value == "5")
        return opcode::op_5;
    else if (value == "6")
        return opcode::op_6;
    else if (value == "7")
        return opcode::op_7;
    else if (value == "8")
        return opcode::op_8;
    else if (value == "9")
        return opcode::op_9;
    else if (value == "10")
        return opcode::op_10;
    else if (value == "11")
        return opcode::op_11;
    else if (value == "12")
        return opcode::op_12;
    else if (value == "13")
        return opcode::op_13;
    else if (value == "14")
        return opcode::op_14;
    else if (value == "15")
        return opcode::op_15;
    else if (value == "16")
        return opcode::op_16;
    else if (value == "nop")
        return opcode::nop;
    else if (value == "ver")
        return opcode::ver;
    else if (value == "if")
        return opcode::if_;
    else if (value == "notif")
        return opcode::notif;
    else if (value == "verif")
        return opcode::verif;
    else if (value == "vernotif")
        return opcode::vernotif;
    else if (value == "else")
        return opcode::else_;
    else if (value == "endif")
        return opcode::endif;
    else if (value == "verify")
        return opcode::verify;
    else if (value == "return")
        return opcode::return_;
    else if (value == "toaltstack")
        return opcode::toaltstack;
    else if (value == "fromaltstack")
        return opcode::fromaltstack;
    else if (value == "2drop")
        return opcode::op_2drop;
    else if (value == "2dup")
        return opcode::op_2dup;
    else if (value == "3dup")
        return opcode::op_3dup;
    else if (value == "2over")
        return opcode::op_2over;
    else if (value == "2rot")
        return opcode::op_2rot;
    else if (value == "2swap")
        return opcode::op_2swap;
    else if (value == "ifdup")
        return opcode::ifdup;
    else if (value == "depth")
        return opcode::depth;
    else if (value == "drop")
        return opcode::drop;
    else if (value == "dup")
        return opcode::dup;
    else if (value == "nip")
        return opcode::nip;
    else if (value == "over")
        return opcode::over;
    else if (value == "pick")
        return opcode::pick;
    else if (value == "roll")
        return opcode::roll;
    else if (value == "rot")
        return opcode::rot;
    else if (value == "swap")
        return opcode::swap;
    else if (value == "tuck")
        return opcode::tuck;
    else if (value == "cat")
        return opcode::cat;
    else if (value == "substr")
        return opcode::substr;
    else if (value == "left")
        return opcode::left;
    else if (value == "right")
        return opcode::right;
    else if (value == "size")
        return opcode::size;
    else if (value == "invert")
        return opcode::invert;
    else if (value == "and")
        return opcode::and_;
    else if (value == "or")
        return opcode::or_;
    else if (value == "xor")
        return opcode::xor_;
    else if (value == "equal")
        return opcode::equal;
    else if (value == "equalverify")
        return opcode::equalverify;
    else if (value == "reserved1")
        return opcode::reserved1;
    else if (value == "reserved2")
        return opcode::reserved2;
    else if (value == "1add")
        return opcode::op_1add;
    else if (value == "1sub")
        return opcode::op_1sub;
    else if (value == "2mul")
        return opcode::op_2mul;
    else if (value == "2div")
        return opcode::op_2div;
    else if (value == "negate")
        return opcode::negate;
    else if (value == "abs")
        return opcode::abs;
    else if (value == "not")
        return opcode::not_;
    else if (value == "0notequal")
        return opcode::op_0notequal;
    else if (value == "add")
        return opcode::add;
    else if (value == "sub")
        return opcode::sub;
    else if (value == "mul")
        return opcode::mul;
    else if (value == "div")
        return opcode::div;
    else if (value == "mod")
        return opcode::mod;
    else if (value == "lshift")
        return opcode::lshift;
    else if (value == "rshift")
        return opcode::rshift;
    else if (value == "booland")
        return opcode::booland;
    else if (value == "boolor")
        return opcode::boolor;
    else if (value == "numequal")
        return opcode::numequal;
    else if (value == "numequalverify")
        return opcode::numequalverify;
    else if (value == "numnotequal")
        return opcode::numnotequal;
    else if (value == "lessthan")
        return opcode::lessthan;
    else if (value == "greaterthan")
        return opcode::greaterthan;
    else if (value == "lessthanorequal")
        return opcode::lessthanorequal;
    else if (value == "greaterthanorequal")
        return opcode::greaterthanorequal;
    else if (value == "min")
        return opcode::min;
    else if (value == "max")
        return opcode::max;
    else if (value == "within")
        return opcode::within;
    else if (value == "ripemd160")
        return opcode::ripemd160;
    else if (value == "sha1")
        return opcode::sha1;
    else if (value == "sha256")
        return opcode::sha256;
    else if (value == "hash160")
        return opcode::hash160;
    else if (value == "hash256")
        return opcode::hash256;
    else if (value == "codeseparator")
        return opcode::codeseparator;
    else if (value == "checksig")
        return opcode::checksig;
    else if (value == "checksigverify")
        return opcode::checksigverify;
    else if (value == "checkmultisig")
        return opcode::checkmultisig;
    else if (value == "checkmultisigverify")
        return opcode::checkmultisigverify;
    // Replaces nop2 with BIP65 activation.
    else if (value == "checklocktimeverify")
        return opcode::checklocktimeverify;
    else if (value == "nop1")
        return opcode::op_nop1;
    else if (value == "nop2")
        return opcode::op_nop2;
    else if (value == "nop3")
        return opcode::op_nop3;
    else if (value == "nop4")
        return opcode::op_nop4;
    else if (value == "nop5")
        return opcode::op_nop5;
    else if (value == "nop6")
        return opcode::op_nop6;
    else if (value == "nop7")
        return opcode::op_nop7;
    else if (value == "nop8")
        return opcode::op_nop8;
    else if (value == "nop9")
        return opcode::op_nop9;
    else if (value == "nop10")
        return opcode::op_nop10;
    else if (value == "raw_data")
        return opcode::raw_data;
    // ERROR: unknown...
    return opcode::bad_operation;
}

opcode data_to_opcode(const data_chunk& value)
{
    static constexpr size_t limit = 76;
    opcode code;
    if (value.size() < limit)
        code = opcode::special;
    else if (value.size() < max_uint8)
        code = opcode::pushdata1;
    else if (value.size() < max_uint16)
        code = opcode::pushdata2;
    else if (value.size() < max_uint32)
        code = opcode::pushdata4;
    else
        code = opcode::bad_operation;
    return code;
}

} // namspace chain
} // namspace libbitcoin
