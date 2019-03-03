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
#ifndef LIBBITCOIN__UTILITY_TRACK_HPP
#define LIBBITCOIN__UTILITY_TRACK_HPP

#include <atomic>
#include <cstddef>
#include <string>
#include <bitcoin/bitcoin/utility/track.hpp>

// libbitcoin defines the log and tracking but does not use them.
// These are defined in bc so that they can be used in network and blockchain.

//#define CONSTRUCT_TRACK(class_name) \
//    track<class_name>(#class_name)

template <class Shared>
class utility_track
{
//public:
//    static std::atomic<size_t> instances;

protected:
    utility_track(const std::string& class_name);
    virtual ~utility_track();

    track<class Shared> getValue() {
       return value;
   }

   void setValue(track<class Shared> value) {
       this->value = value;
   }
private:
   track<class Shared> value;
//    const std::string class_;
};

//#include <bitcoin/bitcoin/impl/utility/track.ipp>

#endif
