#include <future>

#include <bitcoin/bitcoin.hpp>
#include <bitcoin/blockchain/bdb_blockchain.hpp>
using namespace bc;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

enum class crap_state
{
    main,
    show_address,
    calculate_balance,
    show_outputs,
    create_transaction,
    dump_raw_hex,
    pretty_print,
    broadcast,
    exit
};

class crap_menu
{
public:
    void begin()
    {
        state_ = crap_state::main;
        show_menu();
    }

    void show_menu()
    {
        std::cout
            << "1. Show address\n"
            << "2. Calculate balance\n"
            << "3. Show outputs\n"
            << "4. Create transaction\n"
            << "5. Dump raw hex\n"
            << "6. Pretty print\n"
            << "7. Broadcast\n"
            << "8. Exit\n"
            << "> ";
        int option = get_option();
        switch (option)
        {
            case 1:
                show_address();
                break;
            case 2:
                show_balance();
                break;
            case 3:
                show_outputs();
                break;
            case 4:
                create_transaction();
                break;
            case 5:
                dump_raw_hex();
                break;
            case 6:
                pretty_print();
                break;
            case 7:
                broadcast();
                break;
            case 8:
                state_ = crap_state::exit;
                break;
            default:
                break;
        }
    }

    void show_address()
    {
        state_ = crap_state::show_address;
        std::cout << bitcoin_address() << "\n";
        begin();
    }

    void show_balance()
    {
        state_ = crap_state::calculate_balance;
        auto outpoints = get_outpoints();
        uint64_t total = 0;
        for (uint64_t v: outpoints.second)
            total += v;
        std::cout << pretty_amount(total) << " BTC" << std::endl;
        begin();
    }

    void show_outputs()
    {
        state_ = crap_state::show_outputs;
        auto outpoints = get_outpoints();
        BITCOIN_ASSERT(outpoints.first.size() == outpoints.second.size());
        for (size_t i = 0; i < outpoints.first.size(); ++i)
        {
            auto outpoint = outpoints.first[i];
            auto value = outpoints.second[i];
            std::cout << encode_hex(outpoint.hash) << ":" << outpoint.index
                << " " << pretty_amount(value) << " BTC" << std::endl;
        }
        begin();
    }

    script build_output_script(const short_hash& public_key_hash)
    {
        script result;
        result.push_operation({opcode::dup, data_chunk()});
        result.push_operation({opcode::hash160, data_chunk()});
        result.push_operation({opcode::special,
            data_chunk(public_key_hash.begin(), public_key_hash.end())});
        result.push_operation({opcode::equalverify, data_chunk()});
        result.push_operation({opcode::checksig, data_chunk()});
        return result;
    }

    void create_transaction()
    {
        state_ = crap_state::create_transaction;
        auto outpoints = get_outpoints();
        bool outs_sel = false;
        std::vector<bool> selected;
        for (size_t i = 0; i < outpoints.first.size(); ++i)
            selected.push_back(false);
        while (!outs_sel)
        {
            for (size_t i = 0; i < outpoints.first.size(); ++i)
            {
                auto outpoint = outpoints.first[i];
                auto value = outpoints.second[i];
                std::cout << (i + 1) << ". " 
                    << (selected[i] ? "[X]" : "[ ]") << " "
                    << encode_hex(outpoint.hash) << ":" << outpoint.index
                    << " " << pretty_amount(value) << " BTC" << std::endl;
            }
            std::cout << selected.size() + 1 << ". Continue" << std::endl;
            std::cout << "> " << std::endl;
            int opt = get_option() - 1;
            if (opt < 0 || opt >= outpoints.first.size())
                outs_sel = true;
            else
                selected[opt] = !selected[opt];
        }
        transaction_type tx;
        tx.version = 1;
        tx.locktime = 0;
        for (size_t i = 0; i < selected.size(); ++i)
        {
            if (!selected[i])
                continue;
            transaction_input_type input;
            input.previous_output = outpoints.first[i];
            input.sequence = 4294967295;
            input.input_script.push_operation(
                {opcode::special, get_public_key()});
            tx.inputs.push_back(input);
        }
        // select outputs
        uint64_t total = 0;
        for (auto v: outpoints.second)
            total += v;
        std::cout << "Total money in = " << pretty_amount(total) << " BTC\n";
        std::cout << "(fee = total money in - total money out)\n";
        outs_sel = false;
        while (!outs_sel)
        {
            std::cout << "Enter address: ";
            std::string address;
            std::cin >> address;
            std::cout << "Enter amount: ";
            std::string amount_str;
            std::cin >> amount_str;
            transaction_output_type output;
            output.value = convert_amount(amount_str);
            short_hash dest_pubkey_hash = address_to_short_hash(address);
            output.output_script = build_output_script(dest_pubkey_hash);
            tx.outputs.push_back(output);
            std::cout << "Continue? 1. Yes, 2. No" << std::endl;
            int opt = get_option();
            if (opt == 2)
            {
                outs_sel = true;
                break;
            }
        }
        sign_and_store(tx);
        begin();
    }

    void dump_raw_hex()
    {
        state_ = crap_state::dump_raw_hex;
        std::cout << raw_tx_hex() << std::endl;
        begin();
    }
    void pretty_print()
    {
        state_ = crap_state::pretty_print;
        std::cout << pretty_tx() << std::endl;
        begin();
    }

    void broadcast()
    {
        state_ = crap_state::broadcast;
        do_broadcast();
        begin();
    }

protected:
    typedef std::vector<uint64_t> amounts_list;
    typedef std::pair<message::output_point_list, amounts_list> outpoints_info;
    virtual const std::string bitcoin_address() = 0;
    virtual outpoints_info get_outpoints() = 0;
    virtual std::string raw_tx_hex() = 0;
    virtual std::string pretty_tx() = 0;
    virtual void do_broadcast() = 0;
    virtual data_chunk get_public_key() = 0;
    virtual void sign_and_store(const transaction_type& new_tx) = 0;
private:
    int get_option()
    {
        int option;
        std::cin >> option;
        return option;
    }
    uint64_t pretty_amount(uint64_t value)
    {
        return value;
    }
    uint64_t convert_amount(const std::string& amount_str)
    {
        return boost::lexical_cast<uint64_t>(amount_str);
    }
    crap_state state_;
};

class crap_app
  : public crap_menu
{
public:
    crap_app()
      : tx_(nullptr), session_(nullptr)
    {
    }
    ~crap_app()
    {
        delete session_;
    }

    bool initialize(const std::string& raw_private_key)
    {
        if (!ec_.set_private_key(
            private_data(raw_private_key.begin(), raw_private_key.end())))
        {
            std::cerr << "Error: bad private key provided\n";
            return false;
        }
        network_pool_.spawn();
        p_.hosts = std::make_shared<hosts>(network_pool_);
        p_.handshake = std::make_shared<handshake>(network_pool_);
        p_.network = std::make_shared<network>(network_pool_);
        p_.protocol = std::make_shared<protocol>(
            network_pool_, p_.hosts, p_.handshake, p_.network);
        //p_.protocol->subscribe_channel(monitor_tx);

        disk_pool_.spawn();
        p_.blockchain = 
            std::make_shared<bdb_blockchain>(disk_pool_, "database");
        p_.poller = std::make_shared<poller>(p_.blockchain);

        mempool_pool_.spawn();
        p_.transaction_pool =
            transaction_pool::create(mempool_pool_, p_.blockchain);

        session_ = new session(p_);
        session_->start(&crap_app::handle_start);
        return true;
    }

    static void handle_start(const std::error_code&)
    {
    }

    int run()
    {
        begin();
        return 0;
    }
protected:
    const std::string bitcoin_address()
    {
        return public_key_to_address(ec_.public_key());
    }
    outpoints_info get_outpoints()
    {
        std::promise<outpoints_info> promise_outpoints;
        p_.blockchain->fetch_outputs(
            generate_ripemd_hash(ec_.public_key()),
            std::bind(&crap_app::handle_outputs, this,
                _1, _2, &promise_outpoints));
        outpoints_info result = promise_outpoints.get_future().get();
        for (auto outpoint: result.first)
        {
            std::promise<uint64_t> promise_value;
            p_.blockchain->fetch_transaction(outpoint.hash,
                std::bind(&crap_app::extract_value, this,
                    _1, _2, &promise_value, outpoint.index));
            result.second.push_back(promise_value.get_future().get());
        }
        return result;
    }
    std::string raw_tx_hex()
    {
        if (!tx_)
            return "No transaction created";
        satoshi_exporter ex;
        auto raw = ex.save(*tx_);
        return encode_hex(raw);
    }
    std::string pretty_tx()
    {
        if (!tx_)
            return "No transaction created";
        return pretty(*tx_);
    }

    void do_broadcast()
    {
        if (!tx_)
            std::cerr << "No transaction created" << std::endl;
        else
        {
            satoshi_exporter convert_tx;
            data_chunk raw_tx = convert_tx.save(*tx_);
            p_.protocol->broadcast(*tx_);
            std::cout << encode_hex(hash_transaction(*tx_)) << std::endl;
        }
    }

    data_chunk get_public_key()
    {
        return ec_.public_key();
    }

    void sign_and_store(const transaction_type& new_tx)
    {
        if (tx_)
            delete tx_;
        tx_ = new message::transaction(new_tx);

        for (size_t i = 0; i < tx_->inputs.size(); ++i)
        {
            transaction_input_type& input = tx_->inputs[i];
            // Rebuild previous output script
            data_chunk public_key = ec_.public_key();
            script script_code = 
                build_output_script(generate_ripemd_hash(public_key));

            hash_digest tx_hash =
                script::generate_signature_hash(*tx_, i, script_code, 1);
            if (tx_hash == null_hash)
            {
                std::cerr << "error generating signature hash" << std::endl;
                return;
            }
            data_chunk signature = ec_.sign(tx_hash);
            signature.push_back(0x01);

            script& input_script = input.input_script;
            input_script = script();
            input_script.push_operation({opcode::special, signature});
            input_script.push_operation({opcode::special, public_key});
        }
    }

private:
    void handle_outputs(const std::error_code& ec,
        const output_point_list& points,
        std::promise<outpoints_info>* promise_outpoints)
    {
        if (ec)
            std::cerr << "Error: " << ec.message() << std::endl;
        outpoints_info info;
        info.first = points;
        promise_outpoints->set_value(info);
    }
    void extract_value(const std::error_code& ec,
        const transaction_type& tx,
        std::promise<uint64_t>* promise_value, uint32_t index)
    {
        if (ec)
            std::cerr << "Error: " << ec.message() << std::endl;
        BITCOIN_ASSERT(index < tx.outputs.size());
        promise_value->set_value(tx.outputs[index].value);
    }

    elliptic_curve_key ec_;

    threadpool network_pool_, disk_pool_, mempool_pool_;
    session_params p_;
    session* session_;

    transaction_type* tx_;
};

std::string read_private_key(std::ifstream& file)
{
    std::istreambuf_iterator<char> it(file);
    std::istreambuf_iterator<char> end;
    return std::string(it, end);
}

int main(int argc, char** argv)
{
    log_debug().alias(log_level::debug, log_level::null);
    log_info().filter(log_domain::network);
    log_info().filter(log_domain::storage);
    log_info().filter(log_domain::validation);
    log_info().filter(log_domain::protocol);
    log_info().filter(log_domain::poller);
    log_info().filter(log_domain::session);
    if (argc != 2)
    {
        std::cerr << "Need private key filename\n";
        return -1;
    }
    crap_app app;
    std::ifstream file(argv[1]);
    if (!app.initialize(read_private_key(file)))
        return -1;
    return app.run();
}

