#ifndef LIBBITCOIN__UTILITY_UTILITY_MONITOR_COUNT_PTR_HPP
#define LIBBITCOIN__UTILITY_UTILITY_MONITOR_COUNT_PTR_HPP

#include <bitcoin/bitcoin/utility/monitor.hpp>

namespace libbitcoin {
namespace api {

class utility_monitor_count_ptr {
public:
	monitor::count_ptr getValue() {
        return value;
    }

    void setValue(monitor::count_ptr value) {
        this->value = value;
    }
private:
    monitor::count_ptr value;
};

} // namespace api
} // namespace libbitcoin
#endif
