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
#include <bitcoin/blockchain/bdb_blockchain.hpp>
using namespace bc;

class fetch_block_t
  : private coroutine,
    public std::enable_shared_from_this<fetch_block_t>
{
public:
    fetch_block_t(blockchain_ptr chain)
      : blockchain_(chain) {}

    struct stack_t
    {
        size_t last;
        message::block blk;
        message::input_point inp;
    };
    typedef std::shared_ptr<stack_t> stack_ptr;

    void run(const std::error_code& ec = std::error_code(),
        stack_ptr stack = std::make_shared<stack_t>());
private:
    blockchain_ptr blockchain_;
};

void fetch_block_t::run(const std::error_code& ec, stack_ptr stack)
{
    auto this_ptr = shared_from_this();
    message::output_point outpoint;
    outpoint.hash = decode_hex_digest<hash_digest>("f4184fc596403b9d638783cf57adfe4c75c605f6356fbc91338530e9831e9e16");
    outpoint.index = 1;
    reenter(this)
    {
        yield
        {
            auto count = std::make_shared<atomic_counter>(0);
            for (size_t i = 0; i < 4; ++i)
                blockchain_->fetch_last_height(
                    [this_ptr, stack, count](
                        const std::error_code& ec, size_t height)
                    {
                        stack->last = height;
                        ++(*count);
                        if (*count == 4)
                            this_ptr->run(ec, stack);
                    });
        }
        log_debug() << stack->last;
        yield blockchain_->fetch_block_header(stack->last,
            [this_ptr, stack](const std::error_code& ec,
                const message::block& blk)
            {
                stack->blk = blk;
                this_ptr->run(ec, stack);
            });
        log_debug() << encode_hex(hash_block_header(stack->blk));
        yield blockchain_->fetch_spend(outpoint,
            [this_ptr, stack](const std::error_code& ec,
                const message::input_point& inp)
            {
                stack->inp = inp;
                this_ptr->run(ec, stack);
            });
        if (!ec)
            log_debug() << encode_hex(stack->inp.hash);
        else
            log_error() << ec.message();
    }
}

void blockchain_started(const std::error_code& ec, blockchain_ptr)
{
    if (ec)
        log_error() << "Error: " << ec.message();
    else
        log_info() << "Blockchain initialized!";
}


int main()
{
    threadpool pool(1);
    blockchain_ptr chain =
        bdb_blockchain::create(pool, "database", blockchain_started);
    auto f = std::make_shared<fetch_block_t>(chain);
    f->run();
    std::cin.get();
    return 0;
}

