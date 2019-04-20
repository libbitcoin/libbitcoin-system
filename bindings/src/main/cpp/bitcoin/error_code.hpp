#ifndef LIBBITCOIN__ERROR_CODE_HPP
#define LIBBITCOIN__ERROR_CODE_HPP

#include <cstddef>
#include <iostream>
#include <bitcoin/bitcoin/error.hpp>

namespace libbitcoin {
namespace api {

class error_code {

public:
	error_code(code* code) {
		this->value = code;
	}

	code* getValue() {
		return value;
	}

	void setValue(code* value) {
		this->value = value;
	}

	~error_code() {
		delete value;
	}

private:
	code *value;
};

} // namespace api
} // namespace libbitcoin
#endif
