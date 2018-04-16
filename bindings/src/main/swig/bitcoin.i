%module bitcoin
%{
#include <bitcoin/bitcoin.hpp>
%}
#pragma SWIG nowarn=342

//%import <features.h>
//%import <bits/wchar.h>
//%import <bits/wordsize.h>
%import <stdint.i>

%import "../../../../include/bitcoin/bitcoin/compat.h"
%import "../../../../include/bitcoin/bitcoin/compat.hpp"
%import "../../../../include/bitcoin/bitcoin/define.hpp"

%import "bitcoin-config-ext.i"
%import "bitcoin-config-ext-typemap.i"

%include "bitcoin-ext.i"

%include "enums.swg"
%javaconst(1);

%include "../../../../include/bitcoin/bitcoin/version.hpp"
%include "../../../../include/bitcoin/bitcoin/constants.hpp"
%include "../../../../include/bitcoin/bitcoin/define.hpp"
%include "../../../../include/bitcoin/bitcoin/handlers.hpp"
%include "../../../../include/bitcoin/bitcoin/version.hpp"
