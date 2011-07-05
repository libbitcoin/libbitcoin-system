#include <bitcoin/types.hpp>

namespace libbitcoin {

void extend_data(data_chunk& chunk, const data_chunk& other)
{
    chunk.insert(chunk.end(), other.cbegin(), other.cend());
}

} // libbitcoin

