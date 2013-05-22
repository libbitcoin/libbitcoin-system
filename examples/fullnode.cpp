#include <bitcoin/bitcoin.hpp>
using namespace bc;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

void output_to_file(std::ofstream& file, log_level level,
    const std::string& domain, const std::string& body)
{
    if (body.empty())
        return;
    file << level_repr(level);
    if (!domain.empty())
        file << " [" << domain << "]";
    file << ": " << body << std::endl;
}
void output_cerr_and_file(std::ofstream& file, log_level level,
    const std::string& domain, const std::string& body)
{
    if (body.empty())
        return;
    std::ostringstream output;
    output << level_repr(level);
    if (!domain.empty())
        output << " [" << domain << "]";
    output << ": " << body;
    std::cerr << output.str() << std::endl;
}

class fullnode
{
public:
    fullnode();
    void start();
    void stop();

private:
    void handle_start(const std::error_code& ec);

    void monitor_tx(channel_ptr node);
    void recv_tx(const std::error_code& ec,
        const transaction_type& tx, channel_ptr node);

    void new_unconfirm_valid_tx(
        const std::error_code& ec, const index_list& unconfirmed,
        const transaction_type& tx, channel_ptr node);

    threadpool net_pool_, disk_pool_, mem_pool_;
    hosts hosts_;
    handshake handshake_;
    network network_;
    protocol protocol_;
    leveldb_blockchain chain_;
    poller poller_;
    transaction_pool txpool_;
    session session_;
};

fullnode::fullnode()
  : net_pool_(1), disk_pool_(1), mem_pool_(1),
    hosts_(net_pool_), handshake_(net_pool_), network_(net_pool_),
    protocol_(net_pool_, hosts_, handshake_, network_),
    chain_(disk_pool_),
    poller_(mem_pool_, chain_), txpool_(mem_pool_, chain_),
    session_(net_pool_, {
        handshake_, protocol_, chain_, poller_, txpool_})
{
}

void fullnode::start()
{
    protocol_.subscribe_channel(
        std::bind(&fullnode::monitor_tx, this, _1));
    auto handle_start =
        std::bind(&fullnode::handle_start, this, _1);
    // Initialize blockchain
    chain_.start("database", handle_start);
    // Start transaction pool
    txpool_.start();
    // Fire off app.
    session_.start(handle_start);
}

void fullnode::stop()
{
    session_.stop([](const std::error_code&) {});

    net_pool_.stop();
    disk_pool_.stop();
    mem_pool_.stop();
    net_pool_.join();
    disk_pool_.join();
    mem_pool_.join();

    chain_.stop();
}

void fullnode::handle_start(const std::error_code& ec)
{
    if (ec)
    {
        log_error() << "fullnode: " << ec.message();
        stop();
        exit(1);
    }
}

void fullnode::monitor_tx(channel_ptr node)
{
    node->subscribe_transaction(
        std::bind(&fullnode::recv_tx, this, _1, _2, node));
    protocol_.subscribe_channel(
        std::bind(&fullnode::monitor_tx, this, _1));
}
void fullnode::recv_tx(const std::error_code& ec,
    const transaction_type& tx, channel_ptr node)
{
    if (ec)
    {
        log_error() << "Receive transaction: " << ec.message();
        return;
    }
    auto handle_confirm = [](const std::error_code& ec)
        {
            if (ec)
                log_error() << "Confirm error: " << ec.message();
        };
    txpool_.store(tx, handle_confirm,
        std::bind(&fullnode::new_unconfirm_valid_tx, this, _1, _2, tx, node));
    node->subscribe_transaction(
        std::bind(&fullnode::recv_tx, this, _1, _2, node));
}

void fullnode::new_unconfirm_valid_tx(
    const std::error_code& ec, const index_list& unconfirmed,
    const transaction_type& tx, channel_ptr node)
{
    const hash_digest& tx_hash = hash_transaction(tx);
    if (ec)
    {
        log_error()
            << "Error storing memory pool transaction "
            << pretty_hex(tx_hash) << ": " << ec.message();
    }
    else
    {
        auto l = log_info();
        l << "Accepted transaction ";
        if (!unconfirmed.empty())
        {
            l << "(Unconfirmed inputs";
            for (auto idx: unconfirmed)
                l << " " << idx;
            l << ") ";
        }
        l << pretty_hex(tx_hash);
    }
}

int main()
{
    std::ofstream outfile("debug.log"), errfile("error.log");
    log_debug().set_output_function(
        std::bind(output_to_file, std::ref(outfile), _1, _2, _3));
    log_info().set_output_function(
        std::bind(output_to_file, std::ref(outfile), _1, _2, _3));
    log_warning().set_output_function(
        std::bind(output_to_file, std::ref(errfile), _1, _2, _3));
    log_error().set_output_function(
        std::bind(output_cerr_and_file, std::ref(errfile), _1, _2, _3));
    log_fatal().set_output_function(
        std::bind(output_cerr_and_file, std::ref(errfile), _1, _2, _3));

    fullnode app;
    app.start();
    std::cin.get();
    app.stop();

    return 0;
}

