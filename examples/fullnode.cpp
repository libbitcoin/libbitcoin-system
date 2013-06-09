/*
  Full node implementation. Expects the blockchain to be present in
  "./database/" and initialized using ./initchain
*/
#include <future>
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

    // New connection has been started.
    // Subscribe to new transaction messages from the network.
    void connection_started(channel_ptr node);
    // New transaction message from the network.
    // Attempt to validate it by storing it in the transaction pool.
    void recv_tx(const std::error_code& ec,
        const transaction_type& tx, channel_ptr node);
    // Result of store operation in transaction pool.
    void new_unconfirm_valid_tx(
        const std::error_code& ec, const index_list& unconfirmed,
        const transaction_type& tx);

    // Threadpools
    threadpool net_pool_, disk_pool_, mem_pool_;
    // Services
    hosts hosts_;
    handshake handshake_;
    network network_;
    protocol protocol_;
    leveldb_blockchain chain_;
    poller poller_;
    transaction_pool txpool_;
    // Mac OSX needs the bc:: namespace qualifier to compile.
    // Other systems should be OK.
    bc::session session_;
};

fullnode::fullnode()
    // Threadpools and the number of threads they spawn.
    // 6 threads spawned in total.
  : net_pool_(1), disk_pool_(4), mem_pool_(1),
    // Networking related services.
    hosts_(net_pool_), handshake_(net_pool_), network_(net_pool_),
    protocol_(net_pool_, hosts_, handshake_, network_),
    // Blockchain database service.
    chain_(disk_pool_),
    // Poll new blocks, and transaction memory pool.
    poller_(mem_pool_, chain_), txpool_(mem_pool_, chain_),
    // Session manager service. Convenience wrapper.
    session_(net_pool_, {
        handshake_, protocol_, chain_, poller_, txpool_})
{
}

void fullnode::start()
{
    // Subscribe to new connections.
    protocol_.subscribe_channel(
        std::bind(&fullnode::connection_started, this, _1));
    // Start blockchain. Must finish before any operations
    // are performed on the database (or they will fail).
    std::promise<std::error_code> ec_chain;
    auto blockchain_started =
        [&](const std::error_code& ec)
        {
            ec_chain.set_value(ec);
        };
    chain_.start("database", blockchain_started);
    std::error_code ec = ec_chain.get_future().get();
    if (ec)
    {
        stop();
        return;
    }
    // Start transaction pool
    txpool_.start();
    // Fire off app.
    auto handle_start =
        std::bind(&fullnode::handle_start, this, _1);
    session_.start(handle_start);
}

void fullnode::stop()
{
    session_.stop([](const std::error_code&) {});

    // Stop threadpools.
    net_pool_.stop();
    disk_pool_.stop();
    mem_pool_.stop();
    // Join threadpools. Wait for them to finish.
    net_pool_.join();
    disk_pool_.join();
    mem_pool_.join();

    // Safely close blockchain database.
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

void fullnode::connection_started(channel_ptr node)
{
    // Subscribe to transaction messages from this node.
    node->subscribe_transaction(
        std::bind(&fullnode::recv_tx, this, _1, _2, node));
    // Stay subscribed to new connections.
    protocol_.subscribe_channel(
        std::bind(&fullnode::connection_started, this, _1));
}

void fullnode::recv_tx(const std::error_code& ec,
    const transaction_type& tx, channel_ptr node)
{
    if (ec)
    {
        log_error() << "Receive transaction: " << ec.message();
        return;
    }
    // Called when the transaction becomes confirmed in a block.
    auto handle_confirm = [](const std::error_code& ec)
        {
            if (ec)
                log_error() << "Confirm error: " << ec.message();
        };
    // Validate the transaction from the network.
    // Attempt to store in the transaction pool and check the result.
    txpool_.store(tx, handle_confirm,
        std::bind(&fullnode::new_unconfirm_valid_tx, this, _1, _2, tx));
    // Resubscribe to transaction messages from this node.
    node->subscribe_transaction(
        std::bind(&fullnode::recv_tx, this, _1, _2, node));
}

void fullnode::new_unconfirm_valid_tx(
    const std::error_code& ec, const index_list& unconfirmed,
    const transaction_type& tx)
{
    const hash_digest& tx_hash = hash_transaction(tx);
    if (ec)
    {
        log_error()
            << "Error storing memory pool transaction "
            << tx_hash << ": " << ec.message();
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
        l << tx_hash;
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

