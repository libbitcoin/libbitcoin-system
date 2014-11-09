/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#include <memory>
#include <boost/asio.hpp>
using boost::asio::io_service;
#include <bitcoin/utility/logger.hpp>
//#include <bitcoin/utility/subscriber.hpp>
#include "../include/bitcoin/utility/subscriber.hpp"
#include <bitcoin/threadpool.hpp>
using namespace libbitcoin;

namespace libbitcoin {

struct abc
{
    abc(size_t x)
      : x(x)
    {
        log_error() << "abc()";
    }
    ~abc()
    {
        log_error() << "~abc()";
    }
    size_t x;
};

}

void foo(abc_ptr a)
{
    log_debug() << "fii " << (a ? "y" : "n");
    log_debug() << "Weeeee #" << a->x;
}

typedef std::function<void (abc_ptr)> example_handler_type;
typedef std::shared_ptr<subscriber<abc_ptr>> subscribe_ver_ptr;

void defer_sub(subscribe_ver_ptr s, example_handler_type f)
{
    s->subscribe(f);
    s->subscribe(f);
}

void callall(subscribe_ver_ptr s)
{
    abc_ptr a = std::make_shared<abc>(110);
    s->relay(a);
}

int main()
{
    threadpool pool(1);
    io_service::strand str(pool.service());
    auto sub = std::make_shared<subscriber<abc_ptr>>(str);
    str.post(std::bind(defer_sub, sub, foo));
    str.post(std::bind(callall, sub));
    std::cin.get();
    return 0;
}

