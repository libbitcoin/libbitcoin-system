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
        BITCOIN_ASSERT(next_block.header.nonce !=
            std::numeric_limits<uint32_t>::max());
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

// Maybe should also be in libbitcoin too?
script_type build_pubkey_hash_script(const short_hash& pubkey_hash)
{
    script_type result;
    result.push_operation({opcode::dup, data_chunk()});
    result.push_operation({opcode::hash160, data_chunk()});
    result.push_operation({opcode::special,
        data_chunk(pubkey_hash.begin(), pubkey_hash.end())});
    result.push_operation({opcode::equalverify, data_chunk()});
    result.push_operation({opcode::checksig, data_chunk()});
    return result;
}

script_type build_script_hash_script(const short_hash& script_hash)
{
    script_type result;
    result.push_operation({opcode::hash160, data_chunk()});
    result.push_operation({opcode::special,
        data_chunk(script_hash.begin(), script_hash.end())});
    result.push_operation({opcode::equal, data_chunk()});
    return result;
}

bool build_output_script(
    script_type& out_script, const payment_address& payaddr)
{
    switch (payaddr.version())
    {
        case payment_address::pubkey_version:
            out_script = build_pubkey_hash_script(payaddr.hash());
            return true;

        case payment_address::script_version:
            out_script = build_script_hash_script(payaddr.hash());
            return true;
    }
    return false;
}

bool make_signature(transaction_type& tx, size_t input_index,
    const elliptic_curve_key& key, const script_type& script_code)
{
    transaction_input_type& input = tx.inputs[input_index];

    const data_chunk public_key = key.public_key();
    if (public_key.empty())
        return false;
    hash_digest tx_hash =
        script_type::generate_signature_hash(tx, input_index, script_code, 1);
    if (tx_hash == null_hash)
        return false;
    data_chunk signature = key.sign(tx_hash);
    signature.push_back(0x01);
    //std::cout << signature << std::endl;
    script_type& script = tx.inputs[input_index].script;
    // signature
    script.push_operation({opcode::special, signature});
    // public key
    script.push_operation({opcode::special, public_key});
    return true;
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
    payment_address addr;
    set_public_key(addr, public_key);
    bool build_output_success = build_output_script(output.script, addr);
    BITCOIN_ASSERT(build_output_success);
    coinbase_tx.outputs.push_back(output);
    return coinbase_tx;
}

struct block_point
{
    typedef std::vector<block_point> block_point_list;
    typedef std::vector<block_type> block_list;

    // Only used in root node.
    block_list prefix_chain;

    block_point* parent = nullptr;
    block_type blk;
    block_point_list children;
    std::error_code ec;
};

void display_full_chain(const block_point& point,
    size_t offset=0, size_t indent=0)
{
    for (size_t i = 0; i < point.prefix_chain.size(); ++i)
        std::cout << i << ": "
            << hash_block_header(point.prefix_chain[i].header) << std::endl;
    offset += point.prefix_chain.size();
    for (size_t i = 0; i < indent; ++i)
        std::cout << "  ";
    std::cout << (offset + indent) << ": "
        << hash_block_header(point.blk.header);
    if (point.ec)
        std::cout << " [" << point.ec.message() << "]";
    std::cout << std::endl;
    for (const auto& child: point.children)
        display_full_chain(child, offset, indent + 1);
}

typedef std::vector<size_t> point_coord;

const block_type* lookup(const block_point& root, const point_coord& coord)
{
    const block_point* result = &root;
    for (size_t idx: coord)
        result = &result->children[idx];
    return &result->blk;
}

#include "step.hpp"

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
    block_type current_block = genesis_block();
    for (size_t i = 0; i < 100; ++i)
    {
        root_block.prefix_chain.push_back(current_block);
        transaction_type coinbase_tx = create_coinbase(public_key());
        current_block = mine_next(current_block, {coinbase_tx});
        std::error_code ec = store(chain, current_block);
        BITCOIN_ASSERT(!ec);
    }
    // Steal the last block as our new root
    root_block.blk = current_block;
    for (size_t i = 0; i < 20; ++i)
    {
        current_block = step(root_block, *head_block, i);
        std::error_code ec = store(chain, current_block);
        if (ec)
            log_error() << ec.message();
        // Add to index
        block_point new_point;
        new_point.parent = head_block;
        new_point.blk = current_block;
        new_point.ec = ec;
        block_point* parent = find_parent(root_block, current_block);
        BITCOIN_ASSERT(parent != nullptr);
        parent->children.push_back(new_point);
        if (!ec)
            head_block = &parent->children.back();
    }
    display_full_chain(root_block);
    pool.stop();
    pool.join();
    chain.stop();
    return 0;
}

