#include <bitcoin/bitcoin.hpp>
using namespace libbitcoin;

using std::placeholders::_1;
using std::placeholders::_2;

class pollapp
  : public threaded_service,
    public std::enable_shared_from_this<pollapp>
{
public:
    pollapp();

    void start(std::string hostname, unsigned int port);

private:
    void handle_connect(const std::error_code& ec, channel_ptr node);
    void initial_ask_blocks(const std::error_code& ec,
        const message::block_locator& loc);

    void recv_inv(const std::error_code& ec,
        const message::inventory& packet);
    void recv_blk(const std::error_code& ec,
        const message::block& blk);

    void handle_store(const std::error_code& ec, block_info info,
        const hash_digest& block_hash);
    void ask_blocks(const std::error_code& ec,
        const message::block_locator& loc, const hash_digest& hash_stop);

    network_ptr network_;
    handshake_ptr handshake_;
    blockchain_ptr chain_;
    channel_ptr node_;
};

typedef std::shared_ptr<pollapp> pollapp_ptr;

pollapp::pollapp()
{
    network_ = std::make_shared<network>();
    handshake_ = std::make_shared<handshake>();
    chain_ = std::make_shared<bdb_blockchain>("database/");
}

void pollapp::start(std::string hostname, unsigned int port)
{
    handshake_->connect(network_, "localhost", 8333,
        std::bind(&pollapp::handle_connect, shared_from_this(), _1, _2));
}

void pollapp::handle_connect(const std::error_code& ec, channel_ptr node)
{
    if (ec)
    {
        log_fatal() << ec.message();
        return;
    }
    node_ = node;
    chain_->fetch_block_locator(
        std::bind(&pollapp::initial_ask_blocks, shared_from_this(), _1, _2));
}

void handle_send_packet(const std::error_code& ec)
{
    if (ec)
        log_error() << ec.message();
}

void pollapp::initial_ask_blocks(const std::error_code& ec,
    const message::block_locator& loc)
{
    if (ec)
    {
        log_fatal() << ec.message();
        return;
    }
    node_->subscribe_inventory(
        std::bind(&pollapp::recv_inv, shared_from_this(), _1, _2));
    node_->subscribe_block(
        std::bind(&pollapp::recv_blk, shared_from_this(), _1, _2));
    ask_blocks(ec, loc, null_hash);
}

void pollapp::recv_inv(const std::error_code& ec,
    const message::inventory& packet)
{
    if (ec)
    {
        log_fatal() << ec.message();
        return;
    }
    message::get_data getdata;
    for (const message::inventory_vector& ivv: packet.inventories)
    {
        if (ivv.type != message::inventory_type::block)
            continue;
        getdata.inventories.push_back(ivv);
    }
    node_->send(getdata, handle_send_packet);
    // Re-subscribe
    node_->subscribe_inventory(
        std::bind(&pollapp::recv_inv, shared_from_this(), _1, _2));
}

void pollapp::recv_blk(const std::error_code& ec,
    const message::block& blk)
{
    if (ec)
    {
        log_fatal() << ec.message();
        return;
    }
    chain_->store(blk,
        std::bind(&pollapp::handle_store, shared_from_this(),
            _1, _2, hash_block_header(blk)));
    // Re-subscribe
    node_->subscribe_block(
        std::bind(&pollapp::recv_blk, shared_from_this(), _1, _2));
}

void pollapp::handle_store(const std::error_code& ec, block_info info,
    const hash_digest& block_hash)
{
    if (ec)
    {
        log_fatal() << ec.message();
        return;
    }
    switch (info.status)
    {
        case block_status::orphan:
            chain_->fetch_block_locator(
                std::bind(&pollapp::ask_blocks, shared_from_this(),
                    _1, _2, block_hash));
            break;

        case block_status::rejected:
            log_error() << "Rejected block " << pretty_hex(block_hash);
            break;

        case block_status::confirmed:
            log_debug() << "block #" << info.depth;
            break;
    }
}

void pollapp::ask_blocks(const std::error_code& ec,
    const message::block_locator& loc, const hash_digest& hash_stop)
{
    if (ec)
    {
        log_fatal() << ec.message();
        return;
    }
    message::get_blocks packet;
    packet.locator_start_hashes = loc;
    packet.hash_stop = hash_stop;
    node_->send(packet, std::bind(&handle_send_packet, _1));
}

int main(int argc, const char** argv)
{
    bdb_blockchain::setup("database/");

    pollapp_ptr app = std::make_shared<pollapp>();
    app->start("localhost", 8333);
    // Wait for CTRL-D
    while (true)
    {
        char n;
        std::cin >> n;
        if (std::cin.eof())
            break;
    }
    return 0;
}

