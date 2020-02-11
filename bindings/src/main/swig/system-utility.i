%module utility
%{
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/system_common.hpp>
#include <bitcoin/system_utility.hpp>
%}
#pragma SWIG nowarn=317
#pragma SWIG nowarn=341
#pragma SWIG nowarn=342

%ignore getValue();

//%import "system-ext.i"
%import "system-math-ext.i"
//%import "bitcoin-math-ext-typemap.i"
%import "system-chain-ext.i"
//%import "bitcoin-chain-ext-typemap.i"
%import "system-config-ext.i"
//%import "bitcoin-config-ext-typemap.i"

%include "system-utility-ext.i"

//%import "boost/asio.hpp"
//%import "boost/asio/ip/address.hpp"
%include "bitcoin/utility_address.hpp"
%import "boost/asio/ip/tcp.hpp"
%include "bitcoin/utility_endpoint.hpp"
//%import "boost/asio/ip/address_v6.hpp"
%include "bitcoin/utility_ipv6.hpp"
//%import "boost/asio/io_service.hpp"
%ignore setValue(boost::asio::io_service* value);
%include "bitcoin/utility_service.hpp"
%include "bitcoin/utility_array_slice.hpp"
%import <bitcoin/bitcoin/compat.hpp>
%import <boost/asio/detail/config.hpp>
%import <boost/asio/error.hpp>
//%import <boost/thread.hpp>
//%include "bitcoin/utility_asio.hpp"
%include "bitcoin/utility_assert.hpp"
%include "bitcoin/utility_atomic.hpp"

%include "bitcoin/utility_collection.hpp"
%ignore setValue(color* value);
%include "bitcoin/utility_color.hpp"
%import "bitcoin/bitcoin/utility/conditional_lock.hpp"
%ignore setValue(std::shared_ptr<shared_mutex>* value);
%include "bitcoin/utility_mutex_ptr.hpp"
%include "bitcoin/utility_conditional_lock.hpp"
%include "bitcoin/utility_container_sink.hpp"
%include "bitcoin/utility_container_source.hpp"

//%include <std_array.i>

//%ignore build_chunk(loaf slices, size_t extra_reserve=0);
//%include "bitcoin/utility/data.hpp"

//%import "bitcoin/define.hpp"

//%import <boost/config/suffix.hpp>
//%import <boost/smart_ptr/enable_shared_from_this.hpp>
//%include "bitcoin/utility/enable_shared_from_base.hpp"
//%template(enable_shared_from_base_sequencer) enable_shared_from_base<sequencer>;

%include "bitcoin/utility_data_chunk.hpp"
%include "bitcoin/utility_data_stack.hpp"

%import "bitcoin/utility_duration.hpp"
%ignore setValue(std::function<void(const libbitcoin::code&)>* value);
%include "bitcoin/utility_handler.hpp"
%import "bitcoin/bitcoin/utility/thread.hpp"
%include "bitcoin/utility_threadpool.hpp"
%ignore setValue(deadline* value);
%include "bitcoin/utility_deadline.hpp"
//%import <boost/functional.hpp>
%include "bitcoin/utility_decorator.hpp"
%include "bitcoin/utility_delegates.hpp"
%include "bitcoin/utility_deserializer.hpp"
//%include "bitcoin/utility_dispatcher_delay_handler.hpp"
//%include "bitcoin/utility_dispatcher.hpp"
%include "bitcoin/utility_duration.hpp"
%include "bitcoin/utility_enable_shared_from_base.hpp"
%include "bitcoin/utility_endian.hpp"

%import <std_except.i>
%import "bitcoin/bitcoin/utility/exceptions.hpp"
%include "bitcoin/utility_exceptions.hpp"
%import "bitcoin/bitcoin/utility/flush_lock.hpp"
%ignore setValue(boost::filesystem::path* value);
%include "bitcoin/utility_path.hpp"
%include "bitcoin/utility_flush_lock.hpp"
%import "bitcoin/bitcoin/utility/interprocess_lock.hpp"
%ignore setValue(interprocess_lock* value);
%include "bitcoin/utility_interprocess_lock.hpp"

//%rename(toBoolean) libbitcoin::reader::operator bool() const;
//%rename(notOp) libbitcoin::reader::operator!() const;
//%include "bitcoin/utility/reader.hpp"

//%rename(toBoolean) libbitcoin::istream_reader::operator bool() const;
//%rename(notOp) libbitcoin::istream_reader::operator!() const;
//%include "bitcoin/utility/istream_reader.hpp"
%ignore setValue(monitor::count_ptr* value);
%include "bitcoin/utility_monitor_count_ptr.hpp"
%import "bitcoin/p_std_string.hpp"
%import "bitcoin/bitcoin/utility/monitor.hpp"
%include "bitcoin/utility_monitor.hpp"
%import "bitcoin/bitcoin/utility/noncopyable.hpp"
%include "bitcoin/utility_noncopyable.hpp"

//%rename(toBoolean) libbitcoin::writer::operator bool() const;
//%rename(notOp) libbitcoin::writer::operator!() const;
//%include "bitcoin/utility/writer.hpp"

//%rename(toBoolean) libbitcoin::ostream_writer::operator bool() const;
//%rename(notOp) libbitcoin::ostream_writer::operator!() const;
//%include "bitcoin/utility/ostream_writer.hpp"

%include "bitcoin/utility_pending.hpp"
%include "bitcoin/utility_png.hpp"
%import "bitcoin/bitcoin/utility/prioritized_mutex.hpp"
%include "bitcoin/utility_prioritized_mutex.hpp"

//%import <boost/property_tree/ptree.hpp>

//%include "bitcoin/utility_property_tree.hpp"
//%include "bitcoin/utility/random.hpp"
//%include "bitcoin/utility_resubscriber_handler.hpp"
//%include "bitcoin/utility_resubscriber.hpp"
%import "bitcoin/bitcoin/utility/scope_lock.hpp"
%ignore setValue(libbitcoin::shared_mutex* value);
%include "bitcoin/utility_shared_mutex.hpp"
%include "bitcoin/utility_scope_lock.hpp"

%ignore setValue(sequencer::action* value);
%include "bitcoin/utility_action.hpp"
%import "bitcoin/bitcoin/utility/sequencer.hpp"
%include "bitcoin/utility_sequencer.hpp"
%import "bitcoin/bitcoin/utility/sequential_lock.hpp"
%include "bitcoin/utility_sequential_lock.hpp"
%include "bitcoin/utility_serializer_functor.hpp"
%include "bitcoin/utility_serializer.hpp"
%ignore setValue(socket* value);
%include "bitcoin/utility_socket.hpp"
//%include "bitcoin/utility_subscriber_handler.hpp"
%include "bitcoin/utility_subscriber.hpp"
%include "bitcoin/utility_synchronizer.hpp"
%include "bitcoin/utility_thread.hpp"

//%ignore libbitcoin::threadpool::service() const;
%import "bitcoin/bitcoin/utility/threadpool.hpp"
%include "bitcoin/utility_threadpool.hpp"
%import "bitcoin/p_std_time_t.hpp"
%include "bitcoin/utility_timer.hpp"
%include "bitcoin/utility_track.hpp"
%import "bitcoin/bitcoin/utility/work.hpp"
%include "bitcoin/utility_work.hpp"
%import "bitcoin/bitcoin/utility/writer.hpp"
%import "bitcoin/math_short_hash.hpp"
%import "bitcoin/math_mini_hash.hpp"
%import "bitcoin/p_unsigned_char.hpp"
%include "bitcoin/utility_writer.hpp"
