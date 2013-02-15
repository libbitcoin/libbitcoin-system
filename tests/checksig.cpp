#include <bitcoin/bitcoin.hpp>
using namespace bc;

void error_exit(const std::string& message, int status=1)
{
    log_error() << "session: " << message;
    exit(status);
}

void blockchain_started(const std::error_code& ec)
{
    if (ec)
        error_exit(ec.message());
    else
        log_info() << "Blockchain initialized!";
}

void test_checksig(const std::error_code& ec,
    const message::transaction& tx)
{
    if (ec)
        error_exit(ec.message());
    script output_script;
    output_script.push_operation({opcode::dup, data_chunk()});
    output_script.push_operation({opcode::hash160, data_chunk()});
    output_script.push_operation({opcode::special,
        data_chunk{0xcb, 0xf0, 0x3d, 0x35, 0x8d, 0xbb, 0xea, 0x55,
                   0x97, 0x82, 0xbe, 0x77, 0xb5, 0xf2, 0xb4, 0x3e,
                   0xd3, 0xdf, 0xda, 0x24}});
    output_script.push_operation({opcode::equalverify, data_chunk()});
    output_script.push_operation({opcode::checksig, data_chunk()});

    if (output_script.run(tx.inputs[0].input_script, tx, 0))
        log_info() << "SUCCESS";
    else
        log_info() << "FAILURE";
}

int main()
{
    async_service service(1);
    bdb_blockchain chain(service);
    chain.start("database", blockchain_started);
    chain.fetch_transaction(
        hash_digest{0xd2, 0xb5, 0x46, 0x8c, 0xbd, 0x19, 0x4d, 0x7e,
                    0x0d, 0xbe, 0xd4, 0x00, 0xdc, 0x75, 0x0a, 0x79,
                    0x76, 0x0a, 0x85, 0x59, 0x2b, 0x7b, 0x9b, 0xf7,
                    0xe4, 0x11, 0xa8, 0x32, 0x68, 0x9b, 0xe7, 0x36},
        test_checksig);
    std::cin.get();
    service.stop();
    service.join();
    chain.stop();
    return 0;
}

