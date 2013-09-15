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

