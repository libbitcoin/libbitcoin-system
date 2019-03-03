%module consensus
%include "carrays.i"
%array_class(unsigned char, charArray);
%include "enums.swg"
%{
#include <bitcoin/consensus.hpp>
#include <stdexcept>
%}
%exception verify_script {
  try {
    $action
  } catch (std::invalid_argument &e) {
    jclass clazz = jenv->FindClass("java/lang/IllegalArgumentException");
    jenv->ThrowNew(clazz, e.what());
    return $null;
  }
}
%include "../../../../include/bitcoin/consensus/define.hpp"
%include "../../../../include/bitcoin/consensus/version.hpp"
%include "../../../../include/bitcoin/consensus/export.hpp"
