#ifndef LIBBITCOIN_MNEMONIC_HPP
#define LIBBITCOIN_MNEMONIC_HPP

#include <string>
#include <vector>

namespace libbitcoin {

typedef std::vector<std::string> string_list;

string_list encode_mnemonic(const std::string& seed);
const std::string decode_mnemonic(const string_list& words);

} // libbitcoin

#endif

