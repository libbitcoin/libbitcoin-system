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
    {
        net_ = std::make_shared<network>();
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
    }
    void handle_accept(const std::error_code& ec, channel_ptr node)
    {
        if (ec)
            error_exit(ec);
        log_info() << "Connected";
        node->subscribe_transaction(
            std::bind(&echo_app::handle_receive_tx, this, _1, _2, node));
    }

    void handle_receive_tx(const std::error_code& ec,
        const message::transaction& tx, channel_ptr node)
    {
        if (ec)
            error_exit(ec);
        log_info() << "Got tx " << pretty_hex(hash_transaction(tx));
    }

    network_ptr net_;
    acceptor_ptr accept_;
};

int main()
{
    echo_app app;
    app.start();
    std::cin.get();
    return 0;
}

