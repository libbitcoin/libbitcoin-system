%module libbitcoin
%include <std_string.i>

#if (SWIG_VERSION >= 0x020000)
  /* shared_ptr library only available in swig 2
     but we could backport if needed */
  %include <std_shared_ptr.i>

  %shared_ptr(boost::asio::io_service)
  %shared_ptr(boost::asio::io_service::work)
  %shared_ptr(libbitcoin::net::peer)
  %shared_ptr(libbitcoin::net::dialect)
  %shared_ptr(libbitcoin::net::original_dialect)
#endif

/* Headers and declarations for our output c++ program */
%{

#if defined(SWIGRUBY)
        #undef _ /* nice one ruby.. */
#endif

#include "net/messages.h"
#include "net/delegator.h"
#include "net/peer.h"
#include "net/dialects/original.h"

using std::shared_ptr;
using boost::asio::io_service;
using boost::asio::ip::tcp;
using namespace libbitcoin::net;
%}

/* Found this on internets, not sure it will be useful */
%ignore boost::noncopyable;
namespace boost {
    class noncopyable {};
}

/* Header files we want to wrap */
%include "net/dialect.h"
%include "net/messages.h"
%include "net/peer.h"
%include "net/dialects/original.h"
%include "net/delegator.h"

