#include <bitcoin/bitcoin.hpp>

using namespace bc;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

// This should be a public libbitcoin method.
// And a few others from <bitcoin/validate.hpp> too, but definitely this one.
bool check_proof_of_work(hash_digest block_hash, uint32_t bits)
{
    big_number target;
    target.set_compact(bits);

    if (target <= 0 || target > max_target())
        return false;

    big_number our_value;
    our_value.set_hash(block_hash);
    if (our_value > target)
        return false;

    return true;
}

block_type mine_next(const block_type& current_block,
    const transaction_list& transactions)
{
    block_type next_block;
    next_block.header.version = 1;
    next_block.header.previous_block_hash =
        hash_block_header(current_block.header);
    next_block.header.merkle = generate_merkle_root(transactions);
    //next_block.header.timestamp = time(nullptr);
    next_block.header.timestamp = current_block.header.timestamp + (10 * 60);
    next_block.header.bits = current_block.header.bits;
    next_block.header.nonce = 0;
    next_block.transactions = transactions;
    while (!check_proof_of_work(
        hash_block_header(next_block.header), next_block.header.bits))
    {
        ++next_block.header.nonce;
    }
    return next_block;
}

std::error_code store(blockchain& chain, const block_type& blk)
{
    std::error_code ec;
    block_info info;
    bool finished = false;

    auto block_stored = [&ec, &info, &finished](
        const std::error_code& cec, const block_info blk_info)
    {
        info = blk_info;
        ec = cec;
        finished = true;
    };

    chain.store(blk, block_stored);

    while (!finished)
        usleep(1000);

    hash_digest block_hash = hash_block_header(blk.header);
    switch (info.status)
    {
        // There should be no orphans
        case block_status::orphan:
            log_error() << "Orphan block " << block_hash;
        break;

        case block_status::rejected:
            log_error() << "Rejected block " << block_hash;
        break;

        case block_status::confirmed:
            log_info() << "Block #" << info.height << " " << block_hash;
        break;
    }

    return ec;
}

transaction_type create_coinbase(const data_chunk& public_key)
{
    transaction_type coinbase_tx;
    transaction_input_type null_input;
    null_input.previous_output.hash = null_hash;
    null_input.previous_output.index = std::numeric_limits<uint32_t>::max();
    static uint8_t coinbase_nonce = 0;
    ++coinbase_nonce;
    null_input.script = coinbase_script(data_chunk{0x00, coinbase_nonce});
    null_input.sequence = std::numeric_limits<uint32_t>::max();
    coinbase_tx.inputs.push_back(null_input);
    transaction_output_type output;
    // Ahh the good old days!
    output.value = coin_price(50);
    output.script.push_operation({opcode::special, public_key});
    output.script.push_operation({opcode::checksig, data_chunk()});
    coinbase_tx.outputs.push_back(output);
    return coinbase_tx;
}

struct block_point
{
    typedef std::vector<block_point> block_point_list;

    block_point* parent = nullptr;
    block_type blk;
    block_point_list children;
};

void display_full_chain(const block_point& point, size_t indent=0)
{
    for (size_t i = 0; i < indent; ++i)
        std::cout << "  ";
    std::cout << indent << ": "
        << hash_block_header(point.blk.header) << std::endl;
    for (const auto& child: point.children)
        display_full_chain(child, indent + 1);
}

typedef std::vector<size_t> point_coord;

const block_type* lookup(const block_point& root, const point_coord& coord)
{
    const block_point* result = &root;
    for (size_t idx: coord)
        result = &result->children[idx];
    return &result->blk;
}

block_type step(const block_point& root, const block_point& head, size_t n)
{
    const block_type* prev_blk = &head.blk;
    if (n == 4)
    {
        prev_blk = lookup(root, {0, 0, 0});
    }
    else if (n == 5)
    {
        prev_blk = lookup(root, {0, 0, 0, 0});
    }
    else if (n == 11)
    {
        prev_blk = lookup(root, {0, 0, 0, 1});
    }
    const data_chunk& pubkey = decode_hex(
        "040f7f7e4b13fabda77fedd1e8b5978347f94662da855bfb1a37f3269caea9de0"
        "4df6e075bed569b06aab04dcedd81a93b75bf54675ce805d0e36b28d9d195ea7d");
    transaction_type coinbase_tx = create_coinbase(pubkey);
    return mine_next(*prev_blk, {coinbase_tx});
}

block_point* find_parent(block_point& root, const block_type child)
{
    if (child.header.previous_block_hash == hash_block_header(root.blk.header))
        return &root;
    for (auto& root_child: root.children)
    {
        block_point* maybe_parent = find_parent(root_child, child);
        if (maybe_parent)
            return maybe_parent;
    }
    return nullptr;
}

void reorganize(
    const std::error_code& ec, size_t fork_point,
    const bc::blockchain::block_list& new_blocks,
    const bc::blockchain::block_list& replaced_blocks,
    blockchain& chain)
{
    log_info() << "fork_point: " << fork_point;
    big_number new_work = 0, old_work = 0;
    for (size_t i = 0; i < new_blocks.size(); ++i)
    {
        size_t height = fork_point + i + 1;
        const block_type& blk = *new_blocks[i];
        log_info() << "-> " << hash_block_header(blk.header)
            << " (prev = " << blk.header.previous_block_hash << ")";
        new_work += block_work(blk.header.bits);
    }
    for (size_t i = 0; i < replaced_blocks.size(); ++i)
    {
        size_t height = fork_point + i + 1;
        const block_type& blk = *replaced_blocks[i];
        log_info() << "<- " << hash_block_header(blk.header);
        old_work += block_work(blk.header.bits);
    }
    BITCOIN_ASSERT(old_work < new_work);
    chain.subscribe_reorganize(
        std::bind(reorganize, _1, _2, _3, _4, std::ref(chain)));
}

int main()
{
    block_point root_block, *head_block = &root_block;
    // Init blockchain.
    threadpool pool(1);
    leveldb_blockchain chain(pool);
    auto blockchain_start = [](const std::error_code& ec) {};
    chain.start("blockchain", blockchain_start);
    chain.subscribe_reorganize(
        std::bind(reorganize, _1, _2, _3, _4, std::ref(chain)));
    // Now create blocks and store them in the database.
    root_block.blk = genesis_block();
    block_type current_block = root_block.blk;
    for (size_t i = 0; i < 20; ++i)
    {
        current_block = step(root_block, *head_block, i);
        std::error_code ec = store(chain, current_block);
        if (ec)
        {
            log_error() << ec.message();
            break;
        }
        block_point new_point;
        new_point.parent = head_block;
        new_point.blk = current_block;
        block_point* parent = find_parent(root_block, current_block);
        BITCOIN_ASSERT(parent != nullptr);
        parent->children.push_back(new_point);
        head_block = &parent->children.back();
    }
    display_full_chain(root_block);
    pool.stop();
    pool.join();
    chain.stop();
    return 0;
}

