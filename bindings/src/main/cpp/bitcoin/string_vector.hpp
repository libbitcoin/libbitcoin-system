#ifndef LIBBITCOIN__STRING_VECTOR_HPP
#define LIBBITCOIN__STRING_VECTOR_HPP

#include <vector>
#include <string>

namespace libbitcoin {
namespace api {

class string_vector {
public:
	string_vector() :
			value_(new std::vector<std::string>) {
	}

	void set(size_t i, std::string value) {
		(*value_)[i] = value;
	}

	const std::vector<std::string> *cast() const {
		return value_;
	}

	std::string get(size_t i) const {
		return (*value_)[i];
	}

	size_t getSize() const {
		return value_->size();
	}

	std::vector<std::string>* getValue() const {
		return value_;
	}

	void setValue(std::vector<std::string>* value) {
		this->value_ = value;
	}

	~string_vector() {
		value_->clear();
	}

private:
	std::vector<std::string> *value_;
};

} // namespace api
} // namespace libbitcoin
#endif
