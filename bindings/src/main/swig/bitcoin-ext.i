//%include "carrays.i"
//%array_class(bool, boolArray);
%ignore libbitcoin::api::p_bool::cast();
%include "bitcoin/p_bool.hpp"
//%array_class(int32_t, int32Array);
%ignore libbitcoin::api::p_int32_t::cast();
%include "bitcoin/p_int32_t.hpp"
//%array_class(int64_t, int64Array);
%ignore libbitcoin::api::p_int64_t::cast();
%include "bitcoin/p_int64_t.hpp"
//%array_class(uint8_t, uint8Array);
%ignore libbitcoin::api::p_uint8_t::cast();
%include "bitcoin/p_uint8_t.hpp"
//%array_class(uint16_t, uint16Array);
%ignore libbitcoin::api::p_uint16_t::cast();
%include "bitcoin/p_uint16_t.hpp"
//%array_class(uint32_t, uint32Array);
%ignore libbitcoin::api::p_uint256_t::cast();
%include "bitcoin/p_uint256_t.hpp"
//%array_class(uint32_t, uint32Array);
%ignore libbitcoin::api::p_uint32_t::cast();
%include "bitcoin/p_uint32_t.hpp"
//%array_class(uint64_t, uint64Array);
%ignore libbitcoin::api::p_uint64_t::cast();
%include "bitcoin/p_uint64_t.hpp"
//%array_class(unsigned char, ucharArray);
%ignore libbitcoin::api::p_unsigned_char::cast();
%include "bitcoin/p_unsigned_char.hpp"
//%array_class(unsigned long long, ulonglongArray);
%ignore libbitcoin::api::p_unsigned_long_long::cast();
%include "bitcoin/uint8_t_vector.hpp"
%include "bitcoin/unsigned_char_vector.hpp"
%include "bitcoin/p_unsigned_long_long.hpp"

//%include "p_u_long_long.i"
//%include "p_bool.i"
//%include <p_int32.i>

//%include <vector.i>
//%template(ucharVector) std::vector<unsigned char>;
//%template(ucharVectorVector) std::vector<std::vector<unsigned char>>;

//%ignore <bc::error::error_code_t>;
//%ignore is_error_condition_enum<bc::error::error_condition_t>;
%rename(maturity_coinbase, fullname=1) libbitcoin::error::coinbase_maturity;
%ignore make_error_code(error_code_t e);
%ignore make_error_condition(error_condition_t e);
%ignore boost_to_error_code(const boost_code& ec);
%include "bitcoin/bitcoin/error.hpp"
