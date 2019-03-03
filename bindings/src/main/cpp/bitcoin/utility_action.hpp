#ifndef LIBBITCOIN__UTILITY_ACTION_HPP
#define LIBBITCOIN__UTILITY_ACTION_HPP

#include <bitcoin/bitcoin/utility/sequencer.hpp>

namespace libbitcoin {
namespace api {

class utility_action {
public:
	sequencer::action getValue() {
        return value;
    }

    void setValue(sequencer::action value) {
        this->value = value;
    }
private:
    sequencer::action value;
};

} // namespace api
} // namespace libbitcoin
#endif
