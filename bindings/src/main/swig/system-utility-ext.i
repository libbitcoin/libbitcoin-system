%include <stdint.i>

%include <std_string.i>

%import <bitcoin/bitcoin/define.hpp>
%import <bitcoin/bitcoin/compat.hpp>

//%include <bitcoin/utility_array_slice.hpp>

//%rename(get) operator[](size_type index) const;
//%rename(lt) operator<(const binary& other) const;
//%rename(eq) operator==(const binary& other) const;
//%rename(ne) operator!=(const binary& other) const;
//%rename(assign) operator=(const binary& other);
//%ignore operator>>(std::istream& in, binary& to);
//%ignore operator<<(std::ostream& out, const binary& of);
%import "bitcoin/bitcoin/utility/binary.hpp"
//%import <stddef.h>
%ignore cast() const;
%ignore setValue(std::size_t value);
%include "bitcoin/utility_size_type.hpp"
%ignore setValue(libbitcoin::data_slice value);
%include "bitcoin/utility_data_slice.hpp"
%include "bitcoin/utility_binary.hpp"

//%ignore build_chunk(loaf slices, size_t extra_reserve=0);
%include "bitcoin/utility_data_chunk.hpp"
%ignore setValue(std::vector<utility_data_slice> value);
%include "bitcoin/utility_data_slice_vector.hpp"
%ignore setValue(one_byte value);
%include "bitcoin/utility_one_byte.hpp"
%include "bitcoin/utility_data.hpp"

//%include "bitcoin/utility_string.hpp"
//%template(StringVector) std::vector<std::string>;

//%include <std_array.i>

//%template(OneByte) std::array<unsigned char, 1>;
//%template(DataSlice) libbitcoin::array_slice<uint8_t>;
