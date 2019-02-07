#ifndef LIBBITCOIN__MESSAGE_IP_ADDRESS_HPP
#define LIBBITCOIN__MESSAGE_IP_ADDRESS_HPP

#include <cstddef>
#include <iostream>
#include <bitcoin/bitcoin/message/network_address.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class message_ip_address {

public:
  message_ip_address() : ip_address(new message::ip_address) {
  }
    
/*  message_ip_address(unsigned char *value, unsigned int n) {
    p = value;
    size = n;
  }

  void set(unsigned char *value, unsigned int n) {
    if (p) delete [] p;
    size = n;
    p = new unsigned char[size];
    for (int i=0; i<size; i++) {
      p[i] = value[i];
    }
  }
*/
  void set(size_t i, uint8_t value) {
    if (i<16) {
      ip_address[i] = value;
    } else {
    	std::cerr << "SEVERE: index " << i << " is out of range [0;16)\n";
    }
  }

  message::ip_address cast() {
    return ip_address;
  }

  uint8_t get(size_t i) {
    return ip_address[i];
  }

  size_t getSize() {
    return 16;
  }

  ~message_ip_address() {
	  delete ip_address;
  }

private:
  message::ip_address ip_address;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin
#endif
