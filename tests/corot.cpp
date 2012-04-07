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
    outpoint.hash = hash_from_pretty<decltype(outpoint.hash)>("f4184fc596403b9d638783cf57adfe4c75c605f6356fbc91338530e9831e9e16");
    outpoint.index = 1;
    reenter(this)
    {
        yield blockchain_->fetch_last_depth(
            [this_ptr, stack](const std::error_code& ec, size_t depth)
            {
                stack->last = depth;
                this_ptr->run(ec, stack);
            });
        log_debug() << stack->last;
        yield blockchain_->fetch_block_header(stack->last,
            [this_ptr, stack](const std::error_code& ec,
                const message::block& blk)
            {
                stack->blk = blk;
                this_ptr->run(ec, stack);
            });
        log_debug() << pretty_hex(hash_block_header(stack->blk));
        yield blockchain_->fetch_spend(outpoint,
            [this_ptr, stack](const std::error_code& ec,
                const message::input_point& inp)
            {
                stack->inp = inp;
                this_ptr->run(ec, stack);
            });
        if (!ec)
            log_debug() << pretty_hex(stack->inp.hash);
        else
            log_error() << ec.message();
    }
}

int main()
{
    async_service service(1);
    blockchain_ptr chain = bdb_blockchain::create(service, "database");
    auto f = std::make_shared<fetch_block_t>(chain);
    f->run();
    std::cin.get();
    return 0;
}

