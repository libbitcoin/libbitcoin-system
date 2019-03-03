#ifndef LIBBITCOIN__UTILITY_SERIALIZER_FUNCTOR_HPP
#define LIBBITCOIN__UTILITY_SERIALIZER_FUNCTOR_HPP

#include <functional>

namespace libbitcoin {
namespace api {

template <typename Iterator>
class utility_serializer_functor {
	std::function<void(serializer<Iterator>&)> getValue() {
       return value;
   }

   void setValue(std::function<void(serializer<Iterator>&)> value) {
       this->value = value;
   }
private:
   std::function<void(serializer<Iterator>&)> value;
};

} // namespace api
} // namespace libbitcoin
#endif
