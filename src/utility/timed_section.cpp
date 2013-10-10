#include <bitcoin/utility/timed_section.hpp>

#include <bitcoin/utility/logger.hpp>

namespace libbitcoin {

timed_section::timed_section(
    const std::string& context, const std::string& section)
  : context_(context), section_(section)
{
    clock_gettime(CLOCK_REALTIME, &start_);
}

timed_section::~timed_section()
{
    timespec end;
    clock_gettime(CLOCK_REALTIME, &end);
    double duration =
        1e3 * (end.tv_sec - start_.tv_sec) +
        1e-6 * (end.tv_nsec - start_.tv_nsec);
    log_debug(context_) << section_ << " " << std::fixed << duration;
}

} // namespace libbitcoin

