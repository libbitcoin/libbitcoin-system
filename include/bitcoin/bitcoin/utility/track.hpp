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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_TRACK_HPP
#define LIBBITCOIN_TRACK_HPP

#define CONSTRUCT_TRACK(class_name) \
    track<class_name>(#class_name)

#ifdef NDEBUG
    #define INITIALIZE_TRACK(class_name)

    template <class Shared>
    class track
    {
    protected:
        track(const std::string&)
        {
        }
    };
#else
    #include <atomic>
    #include <cstddef>
    #include <string>
    #include <bitcoin/bitcoin/define.hpp>
    #include <bitcoin/bitcoin/utility/log.hpp>

    #define INITIALIZE_TRACK(class_name) \
        template <> \
        std::atomic<size_t> track<class_name>::instances(0);

    template <class Shared>
    class track
    {
    public:
        static std::atomic<size_t> instances;

    protected:
        track(const std::string& class_name)
            : class_(class_name)
        {
            bc::log::debug(LOG_TRACK)
                << class_ << "(" << ++instances << ")";
        }

        ~track()
        {
            bc::log::debug(LOG_TRACK)
                << "~" << class_ << "(" << --instances << ")";
        }

    private:
        const std::string class_;
    };
#endif

#endif
