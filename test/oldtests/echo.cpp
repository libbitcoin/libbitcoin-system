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
#include <bitcoin/bitcoin.hpp>
using namespace libbitcoin;

using std::placeholders::_1;
using std::placeholders::_2;

void error_exit(const std::error_code& ec)
{
    log_fatal() << "error: " << ec.message();
    exit(-1);
}

class echo_app
{
public:
    echo_app()
      : pool_(1)
    {
        net_ = std::make_shared<network>(pool_);
    }
    ~echo_app()
    {
        pool_.stop();
        pool_.join();
    }

    void start()
    {
        net_->listen(9292,
            std::bind(&echo_app::handle_listen, this, _1, _2));
    }
private:
    void handle_listen(const std::error_code& ec, acceptor_ptr accept)
    {
        if (ec)
            error_exit(ec);
        accept_ = accept;
        accept_->accept(std::bind(&echo_app::handle_accept, this, _1, _2));
        net_->connect("localhost", 9292,
            std::bind(&echo_app::handle_connect, this, _1, _2));
    }
    void handle_accept(const std::error_code& ec, channel_ptr node)
    {
        if (ec)
            error_exit(ec);
        log_info() << "Connected";
        node->subscribe_transaction(
            std::bind(&echo_app::handle_receive_tx, this, _1, _2, node));
    }

    void handle_connect(const std::error_code& ec, channel_ptr node)
    {
        // if this node dies then the connection is closed duh
        // and the accepted node (recver) also closes... duh
        sender = node;
        node->send(message::ping(), [](const std::error_code&) {});
    }

    void handle_receive_tx(const std::error_code& ec,
        const message::transaction& tx, channel_ptr node)
    {
        if (ec)
            error_exit(ec);
        log_info() << "Got tx " << encode_hex(hash_transaction(tx));
    }

    threadpool pool_;
    network_ptr net_;
    acceptor_ptr accept_;
    channel_ptr sender, recver;
};

int main()
{
    echo_app app;
    app.start();
    std::cin.get();
    return 0;
}

