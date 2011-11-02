#include <bitcoin/util/elliptic_curve_key.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/address.hpp>

#include <iostream>
#include <sstream>

using libbitcoin::elliptic_curve_key;
using libbitcoin::hash_digest;
using libbitcoin::data_chunk;
using libbitcoin::hash_from_pretty;
using libbitcoin::bytes_from_pretty;
using libbitcoin::pubkey_to_address;
using libbitcoin::pretty_hex;
using libbitcoin::private_data;
using libbitcoin::log_info;

void display_help()
{
    puts("Usage: priv [COMMAND] [ARGS]...");
    puts("");
    puts("The priv commands are:");
    puts("  new\t\tGenerate a new private key and output to STDOUT");
    puts("  sign\t\tSign the next argument using the private key in STDIN");
    puts("  verify\tVerify the next argument using the private key in STDIN");
    puts("  address\tshow the associated bitcoin address");
}

void error_exit(const char* message, int status=1)
{
    std::cerr << "priv: " << message << std::endl;
    exit(status);
}

int new_keypair()
{
    elliptic_curve_key ec;
    ec.new_key_pair();
    private_data raw_private_key = ec.get_private_key();
    std::cout << std::string(raw_private_key.begin(), raw_private_key.end());
    return 0;
}

int sign(const std::string input_data, const std::string raw_private_key)
{
    hash_digest digest = hash_from_pretty(input_data);
    elliptic_curve_key ec;
    if (!ec.set_private_key(
            private_data(raw_private_key.begin(), raw_private_key.end())))
        error_exit("bad private key");
    log_info() << pretty_hex(ec.sign(digest));
    return 0;
}

int verify(const std::string input_data, const std::string& signature_data,
    const std::string raw_private_key)
{
    hash_digest digest = hash_from_pretty(input_data);
    data_chunk signature = bytes_from_pretty(signature_data);
    elliptic_curve_key ec;
    if (!ec.set_private_key(
            private_data(raw_private_key.begin(), raw_private_key.end())))
        error_exit("bad private key");
    log_info() << (ec.verify(digest, signature) ? '1' : '0');
    return 0;
}

int address(const std::string raw_private_key)
{
    elliptic_curve_key ec;
    if (!ec.set_private_key(
            private_data(raw_private_key.begin(), raw_private_key.end())))
        error_exit("bad private key");
    log_info() << pubkey_to_address(ec.get_public_key());
    return 0;
}

std::string read_private_key()
{
    std::string raw_private_key;
    std::getline(std::cin, raw_private_key);
    return raw_private_key;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        display_help();
        return 0;
    }
    std::string command = argv[1];
    size_t number_args = argc - 2, arg_index = 2;
    if (command == "new")
        return new_keypair();
    else if (command == "sign")
    {
        if (number_args != 1)
            error_exit("sign requires argument data");
        std::string input_data = argv[arg_index];
        return sign(input_data, read_private_key());
    }
    else if (command == "verify")
    {
        if (number_args != 2)
            error_exit("verify requires argument data and signature");
        std::string input_data = argv[arg_index], 
            signature = argv[arg_index + 1];
        return verify(input_data, signature, read_private_key());
    }
    else if (command == "address")
        return address(read_private_key());
    else
        error_exit("not a valid command. See priv help text.");
    // Should never happen!
    return 1;
}

