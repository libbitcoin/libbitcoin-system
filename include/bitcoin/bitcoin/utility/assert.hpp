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
#ifndef LIBBITCOIN_ASSERT_HPP
#define LIBBITCOIN_ASSERT_HPP

#ifdef NDEBUG
    #define BITCOIN_ASSERT(expr)
    #define BITCOIN_ASSERT_MSG(expr, msg)
    #define DEBUG_ONLY(expression)
#else
    #include <cassert>
    #define BITCOIN_ASSERT(expr) assert(expr)
    #define BITCOIN_ASSERT_MSG(expr, msg) assert((expr)&&(msg))
    #define DEBUG_ONLY(expression) expression
#endif

#define CONSTRUCT_TRACK(class_name, log_name) \
    track<class_name>(#class_name, log_name)

#ifdef NDEBUG
    #define INITIALIZE_TRACK(class_name)

    template <class Shared>
    class track
    {
    protected:
        track(const std::string&, const std::string&)
        {
        }
    };
#else
    #include <atomic>
    #include <cstddef>
    #include <string>
    #include <bitcoin/bitcoin/utility/log.hpp>

    #define INITIALIZE_TRACK(class_name) \
        template <> \
        track<class_name>::counter track<class_name>::instances(0);

    template <class Shared>
    class track
    {
    public:
        typedef std::atomic<size_t> counter;
        static counter instances;

    protected:
        track(const std::string& class_name, const std::string& log_name)
          : class_(class_name), log_(log_name)
        {
            bc::log::debug(log_)
                << class_ << "(" << ++instances << ")";
        }

        ~track()
        {
            bc::log::debug(log_)
                << "~" << class_ << "(" << --instances << ")";
        }

    private:
        const std::string class_;
        const std::string log_;
    };
#endif

#endif
