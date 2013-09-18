block_type step(const block_point& root, const block_point& head, size_t n)
{
    std::cout << "Stepping " << n << std::endl;
    transaction_type coinbase_tx = create_coinbase(public_key());
    const block_type* prev_blk = &head.blk;
    static hash_digest txh6 = null_hash;
    if (n == 4)
    {
        prev_blk = lookup(root, {0, 0, 0});
        output_point prevout{
            hash_transaction(root.prefix_chain[2].transactions[0]), 0};
        transaction_type tx = construct_transaction(prevout);
        txh6 = hash_transaction(tx);
        return mine_next(*prev_blk, {coinbase_tx, tx});
    }
    else if (n == 5)
    {
        prev_blk = lookup(root, {0, 0, 0, 0});
    }
    else if (n == 6)
    {
        output_point prevout{
            hash_transaction(root.prefix_chain[1].transactions[0]), 0};
        transaction_type tx = construct_transaction(prevout);
        txh6 = hash_transaction(tx);
        return mine_next(*prev_blk, {coinbase_tx, tx});
    }
    else if (n == 7)
    {
        BITCOIN_ASSERT(txh6 != null_hash);
        output_point prevout{txh6, 0};
        transaction_type tx = construct_transaction(prevout);
        return mine_next(*prev_blk, {coinbase_tx, tx});
    }
    else if (n == 8)
    {
        prev_blk = lookup(root, {0, 0, 0});
    }
    else if (n == 9)
    {
        BITCOIN_ASSERT(txh6 != null_hash);
        output_point prevout{txh6, 0};
        transaction_type tx = construct_transaction(prevout);
        return mine_next(*prev_blk, {coinbase_tx, tx});
    }
    else if (n == 11)
    {
        prev_blk = lookup(root, {0, 0, 0, 1});
    }
    else if (n == 15)
    {
        // Attempt double spend.
        output_point prevout{
            hash_transaction(root.prefix_chain[2].transactions[0]), 0};
        transaction_type tx = construct_transaction(prevout);
        txh6 = hash_transaction(tx);
        return mine_next(*prev_blk, {coinbase_tx, tx});
    }
    else if (n == 16)
    {
        prev_blk = lookup(root, {0, 0, 0, 2, 0});
    }
    else if (n == 20)
    {
        BITCOIN_ASSERT(txh6 != null_hash);
        output_point prevout{txh6, 0};
        transaction_type tx = construct_transaction(prevout);
        return mine_next(*prev_blk, {coinbase_tx, tx});
    }
    return mine_next(*prev_blk, {coinbase_tx});
}

