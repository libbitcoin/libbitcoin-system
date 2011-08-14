#include <bitcoin/storage/postgresql_storage.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/messages.hpp>
#include <iostream>
#include <memory>
using std::shared_ptr;
using libbitcoin::postgresql_storage;
using namespace libbitcoin::message;
using libbitcoin::script;
using libbitcoin::operation;
using libbitcoin::opcode;
using libbitcoin::hash_digest;
using libbitcoin::log_error;
typedef shared_ptr<postgresql_storage> psql_ptr;

using std::placeholders::_1;
using std::placeholders::_2;

void null(std::error_code ec)
{
}

void create_fake_stuff(psql_ptr psql)
{
    // First lets try an inv item
    inv_vect ivv{inv_type::transaction,
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
         1, 2, 3, 4, 5, 6, 7, 8, 9, 0xde, 0xad, 0xbe, 0xef}};
    inv inventory;
    inventory.invs.push_back(ivv);
    inventory.invs.push_back(ivv);
    std::cout << "pushing TX ";
    for (int v: inventory.invs[0].hash)
        std::cout << v << " ";
    std::cout << "\n";
    psql->store(inventory, null);

    // Lets push a transaction
    transaction tx;
    tx.version = 123;
    tx.locktime = 110;

    transaction_input input;
    input.hash = hash_digest
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
         1, 2, 3, 4, 5, 6, 7, 8, 9, 0xde, 0xad, 0xbe, 0xef};
    input.index = 80;

    script scr;
    operation op;
    op.code = opcode::dup;
    scr.push_operation(op);
    op.code = opcode::checksig;
    scr.push_operation(op);
    op.code = opcode::special;
    scr.push_operation(op);
    op.code = opcode::hash160;
    op.data = libbitcoin::data_chunk{0x99, 0x98, 0x97, 0x96, 0x95, 0x94, 0x93, 0x92, 0x91, 0x90};
    scr.push_operation(op);
    input.input_script = scr;
    input.sequence = 123;

    tx.inputs.push_back(input);

    transaction_output output;
    output.value = 500;

    operation op2;
    op2.code = opcode::dup;
    scr.push_operation(op2);
    scr.push_operation(op2);
    scr.push_operation(op2);
    output.output_script = scr;

    tx.outputs.push_back(output);
    psql->store(tx, null);

    // Create a fake block
    block blk;
    blk.version = 31900;
    blk.prev_block = hash_digest
        {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
         1, 2, 3, 4, 5, 6, 7, 8, 9, 0xde, 0xad, 0xbe, 0xef};
    blk.merkle_root = hash_digest
        {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
         1, 2, 2, 2, 2, 2, 2, 2, 2, 0xde, 0xad, 0xbe, 0xef};
    blk.timestamp = 9999;
    blk.bits = 0x1b0404cb;
    blk.nonce = 2;
    blk.transactions.push_back(tx);
    psql->store(blk, null);
}

void recv_block(std::error_code ec, libbitcoin::message::block block, psql_ptr psql, size_t block_depth)
{
    if (ec)
    {
        log_error() << ec.message();
        exit(0);
    }
    if (block_depth == (180 + 1000))
    {
        exit(0);
    }
    block_depth++;
    psql->fetch_block_by_depth(block_depth, std::bind(recv_block, _1, _2, psql, block_depth));
}

int main()
{
    psql_ptr psql(new postgresql_storage("bitcoin", "genjix", ""));
    //psql->organize_block_chain();
    psql->fetch_block_by_depth(180, std::bind(recv_block, _1, _2, psql, 180));
    while (true)
        sleep(10);
    return 0;
}

