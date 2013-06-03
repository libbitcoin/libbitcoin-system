#include <bitcoin/bitcoin.hpp>
using namespace bc;

using std::placeholders::_1;
using std::placeholders::_2;

// Watches transactions. Keeps a view count per seen tx hash, and
// cleans up old expired tx hashes.
class tx_watch
  : public async_strand
{
public:
    tx_watch(threadpool& pool, time_t timeout=200);

    // Push a seen tx hash. If this entry exists then the count
    // will be incremented.
    // Else create a new entry in our list.
    void push(const hash_digest& tx_hash);
    // Cleans up expired items. We could make this implicitly called
    // by push() or display(), but single use methods with no side-effects
    // is better code design.
    void cleanup();
    // Display transactions and their count. A better design would be to
    // separate the view from the model and instead provide a method which
    // fetches a copy of our list, but we keep it simple here.
    void display();

private:
    struct entry_count
    {
        hash_digest tx_hash;
        size_t count;
        // Timestamp of when transaction hash was first seen.
        time_t timest;
    };

    typedef std::vector<entry_count> entry_list;

    // The public methods push these methods to the threadpool to be
    // executed and then return immediately.
    // async_strand::queue() is a helper method which posts the work
    // to the threadpool and serializes access.
    // No 2 operations posted through the same async_strand using queue()
    // will execute at the same time.
    void do_push(const hash_digest& tx_hash);
    void do_cleanup();
    void do_display();

    entry_list entries_;
    // Time until an entry is ready to be removed.
    time_t timeout_;
};

tx_watch::tx_watch(threadpool& pool, time_t timeout)
  : async_strand(pool), timeout_(timeout)
{
}

void tx_watch::push(const hash_digest& tx_hash)
{
    queue(std::bind(&tx_watch::do_push, this, tx_hash));
    // Returns immediately.
}
void tx_watch::do_push(const hash_digest& tx_hash)
{
    // If tx_hash is found then increment count...
    bool is_found = false;
    for (entry_count& entry: entries_)
        if (entry.tx_hash == tx_hash)
        {
            ++entry.count;
            is_found = true;
        }
    // Else create a new entry with a count of 1.
    if (!is_found)
        entries_.push_back({tx_hash, 1, time(nullptr)});
}

void tx_watch::cleanup()
{
    queue(std::bind(&tx_watch::do_cleanup, this));
}
void tx_watch::do_cleanup()
{
    // Erase entries where timest is older than (now - timeout_) seconds.
    time_t current_time = time(nullptr);
    auto erase_pred =
        [&](const entry_count& entry)
        {
            return (current_time - entry.timest) > timeout_;
        };
    auto erase_begin =
        std::remove_if(entries_.begin(), entries_.end(), erase_pred);
    // If we have old entries to delete then erase them.
    if (erase_begin != entries_.end())
        entries_.erase(erase_begin);
}

void tx_watch::display()
{
    queue(std::bind(&tx_watch::do_display, this));
}
void tx_watch::do_display()
{
    // Sort entries by count. Highest numbers at the top.
    std::sort(entries_.begin(), entries_.end(),
        [](const entry_count& entry_a, const entry_count& entry_b)
        {
            return entry_a.count > entry_b.count;
        });
    // Display the first 20 entries.
    for (size_t i = 0; i < 20 && i < entries_.size(); ++i)
    {
        const entry_count& entry = entries_[i];
        log_info() << entry.tx_hash << " " << entry.count;
    }
}

// We don't have a database open, and aren't doing any critical file
// operations so we aren't worried about exiting suddenly.
void check_error(const std::error_code& ec)
{
    if (!ec)
        return;
    log_fatal() << ec.message();
    exit(-1);
}

// Needed for the C callback capturing the signals.
bool stopped = false;
void signal_handler(int sig)
{
    log_info() << "Caught signal: " << sig;
    stopped = true;
}

// Started protocol. Node discovery complete.
void handle_start(const std::error_code& ec)
{
    check_error(ec);
    log_debug() << "Started.";
}

void connection_started(channel_ptr node, protocol& prot, tx_watch& watch);
void inventory_received(const std::error_code& ec, const inventory_type& inv,
    channel_ptr node, tx_watch& watch);

void connection_started(channel_ptr node, protocol& prot, tx_watch& watch)
{
    log_info() << "Connection established.";
    node->subscribe_inventory(
        std::bind(inventory_received, _1, _2, node, std::ref(watch)));
    // Resubscribe to new nodes.
    prot.subscribe_channel(
        std::bind(connection_started, _1, std::ref(prot), std::ref(watch)));
}

void inventory_received(const std::error_code& ec, const inventory_type& inv,
    channel_ptr node, tx_watch& watch)
{
    check_error(ec);
    for (const inventory_vector_type& ivec: inv.inventories)
    {
        if (ivec.type != inventory_type_id::transaction)
            continue;
        watch.push(ivec.hash);
    }
    node->subscribe_inventory(
        std::bind(inventory_received, _1, _2, node, std::ref(watch)));
}

int main()
{
    threadpool pool(4);
    // Create dependencies for our protocol object.
    hosts hst(pool);
    handshake hs(pool);
    network net(pool);
    // protocol service.
    protocol prot(pool, hst, hs, net);
    // Perform node discovery if needed and then creating connections.
    prot.start(handle_start);
    tx_watch watch(pool, 200);
    // Notify us of new connections.
    prot.subscribe_channel(
        std::bind(connection_started, _1, std::ref(prot), std::ref(watch)));
    // Catch C signals for stopping the program.
    signal(SIGABRT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    while (!stopped)
    {
        watch.cleanup();
        watch.display();
        sleep(10);
    }
    // Safely close down.
    pool.stop();
    pool.join();
    return 0;
}

