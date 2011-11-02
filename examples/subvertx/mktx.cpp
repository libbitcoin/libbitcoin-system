#include <bitcoin/address.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/messages.hpp>
#include <bitcoin/kernel.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/transaction.hpp>
#include <bitcoin/network/network.hpp>
#include <bitcoin/util/elliptic_curve_key.hpp>
#include <bitcoin/util/logger.hpp>

#include <functional>
#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace libbitcoin;
using namespace libbitcoin::message;
using std::placeholders::_1;
using std::placeholders::_2;

void display_help()
{
    puts("Usage: mktx [OUTPUT] [ADDRESS] [AMOUNT]");
    puts("");
    puts("OUTPUT consists of a transaction hash and output index");
    puts("  125d49f6b826c564ea99345c56286de4b5126e3da38691caa4ccc68c8c8118d5:1");
}

void error_exit(const std::string& message, int status=1)
{
    log_error() << "balance: " << message;
    exit(status);
}

std::mutex mutex;
std::condition_variable condition;
bool finished = false;

void close_application()
{
    std::unique_lock<std::mutex> lock(mutex);
    finished = true;
    condition.notify_one();
}

void handle_tx_sent(const std::error_code& ec)
{
    if (ec)
        error_exit(ec.message());
    close_application();
}

void handle_connected(const std::error_code& ec, channel_handle chandle,
    network_ptr net, const message::transaction& tx)
{
    if (ec)
        error_exit(ec.message());
    log_info() << "Connected";
    net->send(chandle, tx, handle_tx_sent);
}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        display_help();
        return 0;
    }
    std::vector<std::string> output_parts;
    boost::split(output_parts, argv[1], boost::is_any_of(":"));
    if (output_parts.size() != 2)
        error_exit("output requires transaction hash and index");

    std::string raw_private_key;
    std::getline(std::cin, raw_private_key);
    elliptic_curve_key key;
    if (!key.set_private_key(
            private_data(raw_private_key.begin(), raw_private_key.end())))
        error_exit("bad private key");

    transaction_input input;
    input.previous_output.hash = hash_from_pretty(output_parts[0]);
    if (input.previous_output.hash == null_hash)
        error_exit("malformed previous output transaction hash");
    input.previous_output.index = 
        boost::lexical_cast<uint32_t>(output_parts[1]);
    input.sequence = 4294967295;
    data_chunk public_key = key.get_public_key();
    input.input_script.push_operation({opcode::special, public_key});

    transaction_output output;
    output.value = boost::lexical_cast<uint64_t>(argv[3]);
    short_hash pubkey_hash = address_to_short_hash(argv[2]);
    data_chunk raw_script{0x76, 0xa9, 0x14};
    extend_data(raw_script, pubkey_hash);
    extend_data(raw_script, data_chunk{0x88, 0xac});
    output.output_script = parse_script(raw_script);

    transaction tx;
    tx.inputs.push_back(input);
    tx.outputs.push_back(output);
    tx.version = 1;
    tx.locktime = 0;

    // temporary... should be prevout script
    script script_code = output.output_script;
    hash_digest tx_hash =
        script::generate_signature_hash(tx, 0, script_code, 1);
    if (tx_hash == null_hash)
        error_exit("error generating signature hash");
    data_chunk signature = key.sign(tx_hash);
    signature.push_back(0x01);

    script& input_script = tx.inputs[0].input_script;
    input_script = script();
    input_script.push_operation({opcode::special, signature});
    input_script.push_operation({opcode::special, public_key});

    network_ptr net(new network_impl);
    handshake_connect(net, "localhost", 8333, 
        std::bind(&handle_connected, _1, _2, net, tx));

    std::unique_lock<std::mutex> lock(mutex);
    condition.wait(lock, []{ return finished; });
    // Display hash of sent transaction
    log_info() << pretty_hex(hash_transaction(tx));
    sleep(1);
    return 0;
}

