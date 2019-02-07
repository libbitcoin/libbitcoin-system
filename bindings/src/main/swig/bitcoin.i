%module bitcoin
%{
#include <bitcoin/bitcoin.hpp>
%}
#pragma SWIG nowarn=342

//%import <features.h>
//%import <bits/wchar.h>
//%import <bits/wordsize.h>
%import <stdint.i>

%import "bitcoin/bitcoin/compat.h"
%import "bitcoin/bitcoin/compat.hpp"
%import "bitcoin/bitcoin/define.hpp"

%import "bitcoin-config-ext.i"
//%import "bitcoin-config-ext-typemap.i"

%include "bitcoin-ext.i"

%include "enums.swg"
%javaconst(1);

%include "bitcoin/bitcoin/version.hpp"
%include "bitcoin/bitcoin/constants.hpp"
%include "bitcoin/bitcoin/define.hpp"
%include "bitcoin/bitcoin/handlers.hpp"
%include "bitcoin/bitcoin/version.hpp"
