#include <future>
#include <bitcoin/bitcoin.hpp>
using namespace libbitcoin;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

void error_exit(const std::string& message, int status=1)
{
    log_error() << "session: " << message;
    exit(status);
}

void handle_start(const std::error_code& ec)
{
    if (ec)
        error_exit(ec.message());
}

void handle_stop(const std::error_code& ec)
{
    if (ec)
        error_exit(ec.message());
}

void handle_confirm(const std::error_code& ec)
{
    if (ec)
        log_error() << "Confirm error: " << ec.message();
    else
        log_error() << "Confirmed.";
}

session_params* p;

void recv_transaction(const std::error_code& ec,
    const transaction_type& tx, channel_ptr node);
void monitor_tx(channel_ptr node);
void handle_mempool_store(
    const std::error_code& ec, const index_list& unconfirmed,
    const transaction_type& tx, channel_ptr node);

void monitor_tx(channel_ptr node)
{
    node->subscribe_transaction(std::bind(&recv_transaction, _1, _2, node));
    p->protocol_.subscribe_channel(monitor_tx);
}

void recv_transaction(const std::error_code& ec,
    const transaction_type& tx, channel_ptr node)
{
    if (ec)
    {
        log_error() << "transaction: " << ec.message();
        return;
    }
    p->transaction_pool_.store(tx, handle_confirm,
        std::bind(&handle_mempool_store, _1, _2, tx, node));
    node->subscribe_transaction(std::bind(recv_transaction, _1, _2, node));
}

void depends_requested(const std::error_code& ec)
{
    if (ec)
        log_error() << "depends_requested: " << ec.message();
}

void handle_mempool_store(
    const std::error_code& ec, const index_list& unconfirmed,
    const transaction_type& tx, channel_ptr node)
{
    const hash_digest& tx_hash = hash_transaction(tx);
    // Decided against this. Spammers can abuse us more easily.
    /*if (ec == error::input_not_found)
    {
        BITCOIN_ASSERT(unconfirmed.size() == 1);
        BITCOIN_ASSERT(unconfirmed[0] < tx.inputs.size());
        const auto& prevout = tx.inputs[unconfirmed[0]].previous_output;
        log_info() << "Requesting dependency " << pretty_hex(prevout.hash)
            << " for " << pretty_hex(tx_hash);
        message::get_data getdat;
        getdat.inventories.push_back(
            {message::inventory_type::transaction, prevout.hash});
        //getdat.inventories.push_back(
        //    {message::inventory_type::transaction, tx_hash});
        node->send(getdat, depends_requested);
    }
    else if (ec)*/
    if (ec)
    {
        //BITCOIN_ASSERT(unconfirmed.size() == 0);
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

    //bdb_blockchain::setup("database");
    threadpool network_pool(1), disk_pool(1), mempool_pool(1);
    hosts hsts(network_pool);
    handshake hs(network_pool);
    network net(network_pool);
    protocol prot(network_pool, hsts, hs, net);
    prot.subscribe_channel(monitor_tx);

    leveldb_blockchain chain(disk_pool);
    std::promise<std::error_code> ec_promise;
    auto blockchain_started =
        [&ec_promise](const std::error_code& ec)
        {
            ec_promise.set_value(ec);
        };
    chain.start("database", blockchain_started);
    std::error_code ec = ec_promise.get_future().get();
    if (ec)
        error_exit(ec.message());

    poller poll(mempool_pool, chain);

    transaction_pool txpool(mempool_pool, chain);
    txpool.start();

    session_params pp{hs, prot, chain, poll, txpool};
    p = &pp;
    session sesh(network_pool, pp);
    sesh.start(handle_start);

    std::cin.get();

    sesh.stop(handle_stop);

    network_pool.stop();
    disk_pool.stop();
    mempool_pool.stop();
    network_pool.join();
    disk_pool.join();
    mempool_pool.join();

    chain.stop();
    log_debug() << "Exiting...";

    return 0;
}

