#ifndef LIBBITCOIN_UTILITY_TIMED_SECTION_HPP
#define LIBBITCOIN_UTILITY_TIMED_SECTION_HPP

#include <ctime>
#include <string>

namespace libbitcoin {

class timed_section
{
public:
    timed_section(const std::string& context, const std::string& section);
    ~timed_section();
private:
    const std::string context_, section_;
    timespec start_;
};

} // namespace libbitcoin

#endif

