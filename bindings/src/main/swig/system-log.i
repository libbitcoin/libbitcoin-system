%module log
%{
#include <bitcoin/bitcoin.hpp>
%}

//#define BOOST_LOG_NO_LIB
//#define BOOST_DATE_TIME_NO_LIB
//%import <boost/log/detail/config.hpp>
//%import <boost/log/attributes/clock.hpp>
//%import <boost/config/suffix.hpp>
//%import <boost/log/sources/basic_logger.hpp>
//%import <boost/log/expressions/keyword.hpp>

//#include <boost/log/sources/features.hpp>
//#include <boost/log/sources/global_logger_storage.hpp>
//#include <boost/log/sources/threading_models.hpp>
%import <bitcoin/bitcoin/define.hpp>

%include "../include/bitcoin/bitcoin/log/features/counter.hpp"
%include "../include/bitcoin/bitcoin/log/features/gauge.hpp"
%include "../include/bitcoin/bitcoin/log/features/metric.hpp"
%include "../include/bitcoin/bitcoin/log/features/rate.hpp"
%include "../include/bitcoin/bitcoin/log/features/timer.hpp"
//%include "../include/bitcoin/bitcoin/log/attributes.hpp"
%include "../include/bitcoin/bitcoin/log/file_char_traits.hpp"
%include "../include/bitcoin/bitcoin/log/file_collector.hpp"

//%import <boost/limits.hpp>
//%include <boost_shared_ptr.i>
//%include "../include/bitcoin/bitcoin/log/file_collector_repository.hpp"
%include "../include/bitcoin/bitcoin/log/file_counter_formatter.hpp"
%include "../include/bitcoin/bitcoin/log/rotable_file.hpp"
%include "../include/bitcoin/bitcoin/log/severity.hpp"
%include "../include/bitcoin/bitcoin/log/sink.hpp"
%include "../include/bitcoin/bitcoin/log/source.hpp"
%include "../include/bitcoin/bitcoin/log/statsd_sink.hpp"
%include "../include/bitcoin/bitcoin/log/statsd_source.hpp"
%include "../include/bitcoin/bitcoin/log/udp_client_sink.hpp"
