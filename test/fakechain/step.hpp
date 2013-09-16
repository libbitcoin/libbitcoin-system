const std::string private_key =
    "5KL628YQwEi1Czo9e2CFTcSMmaZXpqLgmEofyhpFqTvkgzv4MbG";

const data_chunk public_key()
{
    elliptic_curve_key key;
    const secret_parameter secret = wif_to_secret(private_key);
    BITCOIN_ASSERT(secret != null_hash);
    bool set_secret_success = key.set_secret(secret);
    BITCOIN_ASSERT(set_secret_success);
    return key.public_key();
}

transaction_type construct_transaction(const output_point& prevout)
{
    transaction_type tx;
    tx.version = 1;
    tx.locktime = 0;
    // Outputs
    transaction_output_type output;
    output.value = coin_price(50);
    payment_address addr;
    set_public_key(addr, public_key());
    bool build_output_success = build_output_script(output.script, addr);
    BITCOIN_ASSERT(build_output_success);
    tx.outputs.push_back(output);
    // Inputs
    transaction_input_type input;
    input.previous_output = prevout;
    input.sequence = 4294967295;
    tx.inputs.push_back(input);
    // Sign inputs
    for (size_t i = 0; i < tx.inputs.size(); ++i)
    {
        bc::transaction_input_type& input = tx.inputs[i];
        const secret_parameter secret = wif_to_secret(private_key);
        elliptic_curve_key key;
        bool set_secret_success = key.set_secret(secret);
        BITCOIN_ASSERT(set_secret_success);
        payment_address address;
        set_public_key(address, key.public_key());
        script_type prevout_script_code;
        bool prevout_script_code_success =
            build_output_script(prevout_script_code, address);
        BITCOIN_ASSERT(prevout_script_code_success);
        bool sign_success = make_signature(tx, i, key, prevout_script_code);
    }
    return tx;
}

block_type step(const block_point& root, const block_point& head, size_t n)
{
    transaction_type coinbase_tx = create_coinbase(public_key());
    const block_type* prev_blk = &head.blk;
    if (n == 4)
    {
        prev_blk = lookup(root, {0, 0, 0});
        output_point prevout{
            hash_transaction(root.prefix_chain[1].transactions[0]), 0};
        transaction_type tx = construct_transaction(prevout);
        return mine_next(*prev_blk, {coinbase_tx, tx});
    }
    else if (n == 5)
    {
        prev_blk = lookup(root, {0, 0, 0, 0});
    }
    else if (n == 11)
    {
        prev_blk = lookup(root, {0, 0, 0, 1});
    }
    return mine_next(*prev_blk, {coinbase_tx});
}

