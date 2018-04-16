%module utility
%{
#include <bitcoin/bitcoin.hpp>
%}
#pragma SWIG nowarn=317
#pragma SWIG nowarn=341
#pragma SWIG nowarn=342

%import "bitcoin-math-ext.i"
%import "bitcoin-math-ext-typemap.i"
%import "bitcoin-chain-ext.i"
%import "bitcoin-chain-ext-typemap.i"
%import "bitcoin-config-ext.i"
%import "bitcoin-config-ext-typemap.i"

%include "bitcoin-utility-ext.i"

%include "../include/bitcoin/bitcoin/utility/array_slice.hpp"
%import <bitcoin/bitcoin/compat.hpp>
%import <boost/asio/detail/config.hpp>
%import <boost/asio/error.hpp>
//%import <boost/thread.hpp>
%include "../include/bitcoin/bitcoin/utility/asio.hpp"
%include "../include/bitcoin/bitcoin/utility/assert.hpp"
%include "../include/bitcoin/bitcoin/utility/atomic.hpp"

%include "../include/bitcoin/bitcoin/utility/collection.hpp"
%include "../include/bitcoin/bitcoin/utility/color.hpp"
%include "../include/bitcoin/bitcoin/utility/conditional_lock.hpp"
%include "../include/bitcoin/bitcoin/utility/container_sink.hpp"
%include "../include/bitcoin/bitcoin/utility/container_source.hpp"

//%include <std_array.i>

//%ignore build_chunk(loaf slices, size_t extra_reserve=0);
//%include "../include/bitcoin/bitcoin/utility/data.hpp"

//%import "../include/bitcoin/bitcoin/define.hpp"

//%import <boost/config/suffix.hpp>
//%import <boost/smart_ptr/enable_shared_from_this.hpp>
//%include "../include/bitcoin/bitcoin/utility/enable_shared_from_base.hpp"
//%template(enable_shared_from_base_sequencer) enable_shared_from_base<sequencer>;

%include "../include/bitcoin/bitcoin/utility/deadline.hpp"
//%import <boost/functional.hpp>
//%include "../include/bitcoin/bitcoin/utility/decorator.hpp"
%include "../include/bitcoin/bitcoin/utility/delegates.hpp"
%include "../include/bitcoin/bitcoin/utility/deserializer.hpp"
//%include "../include/bitcoin/bitcoin/utility/dispatcher.hpp"
%include "../include/bitcoin/bitcoin/utility/endian.hpp"

%import <std_except.i>
%include "../include/bitcoin/bitcoin/utility/exceptions.hpp"
%include "../include/bitcoin/bitcoin/utility/flush_lock.hpp"
%include "../include/bitcoin/bitcoin/utility/interprocess_lock.hpp"

//%rename(toBoolean) libbitcoin::reader::operator bool() const;
//%rename(notOp) libbitcoin::reader::operator!() const;
//%include "../include/bitcoin/bitcoin/utility/reader.hpp"

//%rename(toBoolean) libbitcoin::istream_reader::operator bool() const;
//%rename(notOp) libbitcoin::istream_reader::operator!() const;
//%include "../include/bitcoin/bitcoin/utility/istream_reader.hpp"
%include "../include/bitcoin/bitcoin/utility/monitor.hpp"
%include "../include/bitcoin/bitcoin/utility/noncopyable.hpp"

//%rename(toBoolean) libbitcoin::writer::operator bool() const;
//%rename(notOp) libbitcoin::writer::operator!() const;
//%include "../include/bitcoin/bitcoin/utility/writer.hpp"

//%rename(toBoolean) libbitcoin::ostream_writer::operator bool() const;
//%rename(notOp) libbitcoin::ostream_writer::operator!() const;
//%include "../include/bitcoin/bitcoin/utility/ostream_writer.hpp"

%include "../include/bitcoin/bitcoin/utility/pending.hpp"
%include "../include/bitcoin/bitcoin/utility/png.hpp"
%include "../include/bitcoin/bitcoin/utility/prioritized_mutex.hpp"

%import <boost/property_tree/ptree.hpp>

%include "../include/bitcoin/bitcoin/utility/property_tree.hpp"
%include "../include/bitcoin/bitcoin/utility/random.hpp"
//%include "../include/bitcoin/bitcoin/utility/resubscriber.hpp"
%include "../include/bitcoin/bitcoin/utility/scope_lock.hpp"

%include "../include/bitcoin/bitcoin/utility/sequencer.hpp"
%include "../include/bitcoin/bitcoin/utility/sequential_lock.hpp"
%include "../include/bitcoin/bitcoin/utility/serializer.hpp"
%include "../include/bitcoin/bitcoin/utility/socket.hpp"
//%include "../include/bitcoin/bitcoin/utility/subscriber.hpp"
%include "../include/bitcoin/bitcoin/utility/synchronizer.hpp"
%include "../include/bitcoin/bitcoin/utility/thread.hpp"
%ignore libbitcoin::threadpool::service() const;
%include "../include/bitcoin/bitcoin/utility/threadpool.hpp"
%include "../include/bitcoin/bitcoin/utility/timer.hpp"
%include "../include/bitcoin/bitcoin/utility/track.hpp"
%include "../include/bitcoin/bitcoin/utility/work.hpp"
