%include "carrays.i"
%array_class(uint8_t, uint8Array);
%array_class(uint16_t, uint16Array);
%array_class(uint32_t, uint32Array);
%array_class(uint64_t, uint64Array);
%array_class(int32_t, int32Array);
%array_class(int64_t, int64Array);
%array_class(unsigned char, ucharArray);
%array_class(unsigned long long, ulonglongArray);

%include "p_u_long_long.i"
%include "p_bool.i"
//%include <p_int32.i>

%include <vector.i>
%template(ucharVector) std::vector<unsigned char>;
%template(ucharVectorVector) std::vector<std::vector<unsigned char>>;

//%ignore <bc::error::error_code_t>;
//%ignore is_error_condition_enum<bc::error::error_condition_t>;
%include "../../../../include/bitcoin/bitcoin/error.hpp"
