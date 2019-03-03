#ifndef LIBBITCOIN__UTILITY_DATA_SLICE_VECTOR_HPP
#define LIBBITCOIN__UTILITY_DATA_SLICE_VECTOR_HPP

#include <bitcoin/utility_data_slice.hpp>
#include <vector>

namespace libbitcoin {
namespace api {

using namespace std;

typedef size_t size_type;

class utility_data_slice_vector {

public:
	utility_data_slice_vector() :
			value(new std::vector<utility_data_slice>) {
	}

	/*  utility_data_slice_vector(std::vector<utility_data_slice>value, unsigned int n) {
	 p = value;
	 size = n;
	 }

	 void set(std::vector<utility_data_slice>value, unsigned int n) {
	 if (p) delete [] p;
	 size = n;
	 p = new utility_data_slice[size];
	 for (int i=0; i<size; i++) {
	 p[i] = value[i];
	 }
	 }
	 */
	void set(size_type i, utility_data_slice value) {
		this->value[i] = value;
	}

	const std::vector<utility_data_slice>& cast() const {
		return value;
	}

	utility_data_slice get(size_type i) {
		return value[i];
	}

	size_t getSize() {
		return value.size();
	}

	~utility_data_slice_vector() {
		value.clear();
	}

	std::vector<utility_data_slice> getValue() {
		return value;
	}

	void setValue(std::vector<utility_data_slice> value) {
		this->value = value;
	}
private:
	std::vector<utility_data_slice> value;
};

} // namespace api
} // namespace libbitcoin
#endif
