%module consensus
//%include "carrays.i"
//%array_class(unsigned char, charArray);
%include "enums.swg"
%{
#include <bitcoin/consensus.hpp>
#include <bitcoin/consensus_api.hpp>
//#include <stdexcept>
%}
//%exception verify_script {
//  try {
//    $action
//  } catch (std::invalid_argument &e) {
//    jclass clazz = jenv->FindClass("java/lang/IllegalArgumentException");
//    jenv->ThrowNew(clazz, e.what());
//    return $null;
//  }
//}
%ignore libbitcoin::consensus::verify_script(const unsigned char* transaction,
    size_t transaction_size, const unsigned char* prevout_script,
    size_t prevout_script_size, unsigned long long prevout_value,
    unsigned int tx_input_index, unsigned int flags);
%import "bitcoin/consensus/define.hpp"
%import "bitcoin/consensus/version.hpp"
%include "bitcoin/consensus/export.hpp"
%import "bitcoin/p_unsigned_char.hpp"
%include "bitcoin/consensus_api.hpp"
