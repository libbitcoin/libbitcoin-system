/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_TEST_PERFORMANCE_PERFORMANCE_HPP
#define LIBBITCOIN_SYSTEM_TEST_PERFORMANCE_PERFORMANCE_HPP

#include "../test.hpp"
#include <chrono>

namespace perf {

template <size_t Size>
constexpr data_array<Size> get_array(uint64_t seed) noexcept
{
    // specified-size array.
    return to_big_endian_size<Size>(seed);
}

template <size_t Size>
constexpr data_chunk get_chunk(uint64_t seed) noexcept
{
    // minimally-sized chunk.
    const auto chunk = to_big_endian_size(seed);
    chunk.resize(Size);
    return chunk;
}

template <typename Time = std::chrono::milliseconds,
    class Clock = std::chrono::system_clock>
class timer
{
public:
    /// Returns the duration (in chrono's type system) of the elapsed time.
    template <typename Function, typename ...Args>
    static Time duration(const Function& func, Args&&... args) noexcept
    {
        const auto start = Clock::now();
        func(std::forward<Args>(args)...);
        return std::chrono::duration_cast<Time>(Clock::now() - start);
    }

    /// Returns the quantity (count) of the elapsed time as TimeT units.
    template <typename Function, typename ...Args>
    static typename Time::rep execution(const Function& func,
        Args&&... args) noexcept
    {
        return duration(func, std::forward<Args>(args)...).count();
    }
};

constexpr auto block_100k = base16_array(
    "010000007f110631052deeee06f0754a3629ad7663e56359fd5f3aa7b3e30a00"
    "000000005f55996827d9712147a8eb6d7bae44175fe0bcfa967e424a25bfe9f4"
    "dc118244d67fb74c9d8e2f1bea5ee82a03010000000100000000000000000000"
    "00000000000000000000000000000000000000000000ffffffff07049d8e2f1b"
    "0114ffffffff0100f2052a0100000043410437b36a7221bc977dce712728a954"
    "e3b5d88643ed5aef46660ddcfeeec132724cd950c1fdd008ad4a2dfd354d6af0"
    "ff155fc17c1ee9ef802062feb07ef1d065f0ac000000000100000001260fd102"
    "fab456d6b169f6af4595965c03c2296ecf25bfd8790e7aa29b404eff01000000"
    "8c493046022100c56ad717e07229eb93ecef2a32a42ad041832ffe66bd2e1485"
    "dc6758073e40af022100e4ba0559a4cebbc7ccb5d14d1312634664bac46f36dd"
    "d35761edaae20cefb16f01410417e418ba79380f462a60d8dd12dcef8ebfd7ab"
    "1741c5c907525a69a8743465f063c1d9182eea27746aeb9f1f52583040b1bc34"
    "1b31ca0388139f2f323fd59f8effffffff0200ffb2081d0000001976a914fc7b"
    "44566256621affb1541cc9d59f08336d276b88ac80f0fa02000000001976a914"
    "617f0609c9fabb545105f7898f36b84ec583350d88ac00000000010000000122"
    "cd6da26eef232381b1a670aa08f4513e9f91a9fd129d912081a3dd138cb01301"
    "0000008c4930460221009339c11b83f234b6c03ebbc4729c2633cbc8cbd0d157"
    "74594bfedc45c4f99e2f022100ae0135094a7d651801539df110a028d65459d2"
    "4bc752d7512bc8a9f78b4ab368014104a2e06c38dc72c4414564f190478e3b0d"
    "01260f09b8520b196c2f6ec3d06239861e49507f09b7568189efe8d327c3384a"
    "4e488f8c534484835f8020b3669e5aebffffffff0200ac23fc060000001976a9"
    "14b9a2c9700ff9519516b21af338d28d53ddf5349388ac00743ba40b00000019"
    "76a914eb675c349c474bec8dea2d79d12cff6f330ab48788ac00000000");

constexpr auto transaction_2 = base16_array(
    "010000000364e62ad837f29617bafeae951776e7a6b3019b2da37827921548d1"
    "a5efcf9e5c010000006b48304502204df0dc9b7f61fbb2e4c8b0e09f3426d625"
    "a0191e56c48c338df3214555180eaf022100f21ac1f632201154f3c69e1eadb5"
    "9901a34c40f1127e96adc31fac6ae6b11fb4012103893d5a06201d5cf61400e9"
    "6fa4a7514fc12ab45166ace618d68b8066c9c585f9ffffffff54b755c39207d4"
    "43fd96a8d12c94446a1c6f66e39c95e894c23418d7501f681b010000006b4830"
    "4502203267910f55f2297360198fff57a3631be850965344370f732950b47795"
    "737875022100f7da90b82d24e6e957264b17d3e5042bab8946ee5fc676d15d91"
    "5da450151d36012103893d5a06201d5cf61400e96fa4a7514fc12ab45166ace6"
    "18d68b8066c9c585f9ffffffff0aa14d394a1f0eaf0c4496537f8ab9246d9663"
    "e26acb5f308fccc734b748cc9c010000006c493046022100d64ace8ec2d5feeb"
    "3e868e82b894202db8cb683c414d806b343d02b7ac679de7022100a2dcd39940"
    "dd28d4e22cce417a0829c1b516c471a3d64d11f2c5d754108bdc0b012103893d"
    "5a06201d5cf61400e96fa4a7514fc12ab45166ace618d68b8066c9c585f9ffff"
    "ffff02c0e1e400000000001976a914884c09d7e1f6420976c40e040c30b2b622"
    "10c3d488ac20300500000000001976a914905f933de850988603aafeeb2fd7fc"
    "e61e66fe5d88ac00000000");

} // namespace perf

#endif
