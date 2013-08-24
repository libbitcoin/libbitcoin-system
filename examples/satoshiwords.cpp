/*
  Display the genesis block message by Satoshi.
*/
#include <bitcoin/bitcoin.hpp>
using namespace bc;

int main()
{
    // Create genesis block.
    block_type genblk = genesis_block();
    // Genesis block contains a single coinbase transaction.
    assert(genblk.transactions.size() == 1);
    // Get first transaction in block (coinbase).
    const transaction_type& coinbase_tx = genblk.transactions[0];
    // Coinbase tx has a single input.
    assert(coinbase_tx.inputs.size() == 1);
    const transaction_input_type& coinbase_input = coinbase_tx.inputs[0];
    // Convert the input script to its raw format.
    const data_chunk& raw_block_message = save_script(coinbase_input.script);
    // Convert this to an std::string.
    std::string message;
    message.resize(raw_block_message.size());
    std::copy(raw_block_message.begin(), raw_block_message.end(),
        message.begin());
    // Display the genesis block message.
    std::cout << message << std::endl;
    return 0;
}

