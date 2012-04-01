#include <bitcoin/bitcoin.hpp>
using namespace bc;

extern data_chunk raw_block44987;
extern data_chunk raw_block44989;

int main()
{
    satoshi_exporter ex;
    const message::block& block44987 = ex.load_block(raw_block44987);
    const message::block& block44989 = ex.load_block(raw_block44989);

    const auto& tx_a = block44987.transactions[1];
    const auto& tx_b = block44989.transactions[16];

    auto output_script = tx_a.outputs[1].output_script;
    auto input_script = tx_b.inputs[0].input_script;

    log_debug() << output_script.pretty();
    log_debug();
    log_debug() << input_script.pretty();

    if (output_script.type() == payment_type::script_hash)
        log_debug() << "scrhash";

    if (output_script.run(input_script, tx_b, 0))
        log_debug() << "pass";
    return 0;
}

