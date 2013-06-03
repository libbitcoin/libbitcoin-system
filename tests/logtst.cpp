#include <bitcoin/bitcoin.hpp>
using namespace bc;

void output(log_level level, const std::string& domain, const std::string& body)
{
    constexpr size_t lvl_spaces = 7;
    constexpr size_t domain_spaces = 10;
    if (body.empty())
        return;
    std::cout << "[";
    std::string lvlrep = level_repr(level);
    std::cout << lvlrep;
    BITCOIN_ASSERT(lvlrep.size() <= lvl_spaces);
    for (size_t i = 0; i < (lvl_spaces - lvlrep.size()); ++i)
        std::cout << " ";
    std::cout << "] ";
    if (!domain.empty())
    {
        BITCOIN_ASSERT(domain.size() <= domain_spaces);
        std::cout << "[" << domain;
        for (size_t i = 0; i < (domain_spaces - domain.size()); ++i)
            std::cout << " ";
        std::cout << "] ";
    }
    std::cout << body << std::endl;
}

int main()
{
    log_debug().set_output_function(output);
    log_info().set_output_function(output);
    log_warning().set_output_function(output);
    log_error().set_output_function(output);
    log_fatal().set_output_function(output);

    log_debug(LOG_PROTOCOL) << "Connected to 500 nodes.";
    log_warning(LOG_BLOCKCHAIN) << "Couldn't start blockchain.";
    return 0;
}

