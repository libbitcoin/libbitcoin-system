###############################################################################
#  Copyright (c) 2014-2020 libbitcoin-system developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################

# Automake settings.
#==============================================================================
# Look for macros in the m4 subdirectory.
#------------------------------------------------------------------------------
ACLOCAL_AMFLAGS = -I m4


# Distribute, make and install products.
#==============================================================================
# files => ${pkgconfigdir}
#------------------------------------------------------------------------------
pkgconfig_DATA = \
    libbitcoin-system.pc

# files => ${docdir}
#------------------------------------------------------------------------------
doc_DATA = \
    AUTHORS \
    COPYING \
    ChangeLog \
    INSTALL \
    NEWS \
    README

# src/libbitcoin-system.la => ${libdir}
#------------------------------------------------------------------------------
lib_LTLIBRARIES = src/libbitcoin-system.la
src_libbitcoin_system_la_CPPFLAGS = -I${srcdir}/include ${icu} ${boost_BUILD_CPPFLAGS} ${pthread_BUILD_CPPFLAGS} ${icu_i18n_BUILD_CPPFLAGS} ${secp256k1_BUILD_CPPFLAGS}
src_libbitcoin_system_la_LDFLAGS = ${boost_LDFLAGS}
src_libbitcoin_system_la_LIBADD = ${boost_chrono_LIBS} ${boost_date_time_LIBS} ${boost_filesystem_LIBS} ${boost_iostreams_LIBS} ${boost_locale_LIBS} ${boost_log_LIBS} ${boost_program_options_LIBS} ${boost_regex_LIBS} ${boost_system_LIBS} ${boost_thread_LIBS} ${pthread_LIBS} ${rt_LIBS} ${icu_i18n_LIBS} ${dl_LIBS} ${secp256k1_LIBS}
src_libbitcoin_system_la_SOURCES = \
    src/error.cpp \
    src/settings.cpp \
    src/chain/block.cpp \
    src/chain/block_filter.cpp \
    src/chain/chain_state.cpp \
    src/chain/compact.cpp \
    src/chain/header.cpp \
    src/chain/input.cpp \
    src/chain/output.cpp \
    src/chain/output_point.cpp \
    src/chain/payment_record.cpp \
    src/chain/point.cpp \
    src/chain/point_value.cpp \
    src/chain/points_value.cpp \
    src/chain/script.cpp \
    src/chain/stealth_record.cpp \
    src/chain/transaction.cpp \
    src/chain/witness.cpp \
    src/concurrency/conditional_lock.cpp \
    src/concurrency/deadline.cpp \
    src/concurrency/dispatcher.cpp \
    src/concurrency/file_lock.cpp \
    src/concurrency/flush_lock.cpp \
    src/concurrency/interprocess_lock.cpp \
    src/concurrency/monitor.cpp \
    src/concurrency/prioritized_mutex.cpp \
    src/concurrency/scope_lock.cpp \
    src/concurrency/sequencer.cpp \
    src/concurrency/sequential_lock.cpp \
    src/concurrency/socket.cpp \
    src/concurrency/thread.cpp \
    src/concurrency/threadpool.cpp \
    src/concurrency/work.cpp \
    src/config/authority.cpp \
    src/config/base16.cpp \
    src/config/base2.cpp \
    src/config/base32.cpp \
    src/config/base58.cpp \
    src/config/base64.cpp \
    src/config/base85.cpp \
    src/config/block.cpp \
    src/config/checkpoint.cpp \
    src/config/compact_filter.cpp \
    src/config/directory.cpp \
    src/config/endpoint.cpp \
    src/config/hash160.cpp \
    src/config/hash256.cpp \
    src/config/header.cpp \
    src/config/input.cpp \
    src/config/output.cpp \
    src/config/parameter.cpp \
    src/config/parser.cpp \
    src/config/point.cpp \
    src/config/printer.cpp \
    src/config/script.cpp \
    src/config/transaction.cpp \
    src/data/binary.cpp \
    src/data/data_chunk.cpp \
    src/data/data_slab.cpp \
    src/data/data_slice.cpp \
    src/data/string.cpp \
    src/log/file_collector.cpp \
    src/log/file_collector_repository.cpp \
    src/log/file_counter_formatter.cpp \
    src/log/sink.cpp \
    src/log/statsd_sink.cpp \
    src/log/udp_client_sink.cpp \
    src/machine/interpreter.cpp \
    src/machine/number.cpp \
    src/machine/opcode.cpp \
    src/machine/operation.cpp \
    src/machine/program.cpp \
    src/math/checksum.cpp \
    src/math/crypto.cpp \
    src/math/elliptic_curve.cpp \
    src/math/golomb_coding.cpp \
    src/math/hash.cpp \
    src/math/pseudo_random.cpp \
    src/math/ring_signature.cpp \
    src/math/secp256k1_initializer.cpp \
    src/math/secp256k1_initializer.hpp \
    src/math/siphash.cpp \
    src/math/stealth.cpp \
    src/math/external/aes256.c \
    src/math/external/aes256.h \
    src/math/external/crypto_scrypt.c \
    src/math/external/crypto_scrypt.h \
    src/math/external/hmac_sha256.c \
    src/math/external/hmac_sha256.h \
    src/math/external/hmac_sha512.c \
    src/math/external/hmac_sha512.h \
    src/math/external/lax_der_parsing.c \
    src/math/external/lax_der_parsing.h \
    src/math/external/pbkdf2_sha256.c \
    src/math/external/pbkdf2_sha256.h \
    src/math/external/pkcs5_pbkdf2.c \
    src/math/external/pkcs5_pbkdf2.h \
    src/math/external/ripemd160.c \
    src/math/external/ripemd160.h \
    src/math/external/sha1.c \
    src/math/external/sha1.h \
    src/math/external/sha256.c \
    src/math/external/sha256.h \
    src/math/external/sha512.c \
    src/math/external/sha512.h \
    src/math/external/zeroize.c \
    src/math/external/zeroize.h \
    src/message/address.cpp \
    src/message/alert.cpp \
    src/message/alert_payload.cpp \
    src/message/block.cpp \
    src/message/block_transactions.cpp \
    src/message/compact_block.cpp \
    src/message/compact_filter.cpp \
    src/message/compact_filter_checkpoint.cpp \
    src/message/compact_filter_headers.cpp \
    src/message/fee_filter.cpp \
    src/message/filter_add.cpp \
    src/message/filter_clear.cpp \
    src/message/filter_load.cpp \
    src/message/get_address.cpp \
    src/message/get_block_transactions.cpp \
    src/message/get_blocks.cpp \
    src/message/get_compact_filter_checkpoint.cpp \
    src/message/get_compact_filter_headers.cpp \
    src/message/get_compact_filters.cpp \
    src/message/get_data.cpp \
    src/message/get_headers.cpp \
    src/message/header.cpp \
    src/message/headers.cpp \
    src/message/heading.cpp \
    src/message/inventory.cpp \
    src/message/inventory_vector.cpp \
    src/message/memory_pool.cpp \
    src/message/merkle_block.cpp \
    src/message/messages.cpp \
    src/message/network_address.cpp \
    src/message/not_found.cpp \
    src/message/ping.cpp \
    src/message/pong.cpp \
    src/message/prefilled_transaction.cpp \
    src/message/reject.cpp \
    src/message/send_compact.cpp \
    src/message/send_headers.cpp \
    src/message/transaction.cpp \
    src/message/verack.cpp \
    src/message/version.cpp \
    src/radix/base_10.cpp \
    src/radix/base_16.cpp \
    src/radix/base_2048.cpp \
    src/radix/base_32.cpp \
    src/radix/base_58.cpp \
    src/radix/base_64.cpp \
    src/radix/base_85.cpp \
    src/unicode/ascii.cpp \
    src/unicode/code_points.cpp \
    src/unicode/conversion.cpp \
    src/unicode/normalization.cpp \
    src/unicode/utf8_everywhere/utf8_console_streambuf.cpp \
    src/unicode/utf8_everywhere/utf8_environment.cpp \
    src/unicode/utf8_everywhere/utf8_ifstream.cpp \
    src/unicode/utf8_everywhere/utf8_istream.cpp \
    src/unicode/utf8_everywhere/utf8_ofstream.cpp \
    src/unicode/utf8_everywhere/utf8_ostream.cpp \
    src/unicode/utf8_everywhere/utf8_streambuf.cpp \
    src/wallet/context.cpp \
    src/wallet/message.cpp \
    src/wallet/neutrino_filter.cpp \
    src/wallet/property_tree.cpp \
    src/wallet/addresses/bitcoin_uri.cpp \
    src/wallet/addresses/payment_address.cpp \
    src/wallet/addresses/qr_code.cpp \
    src/wallet/addresses/stealth_address.cpp \
    src/wallet/addresses/stealth_receiver.cpp \
    src/wallet/addresses/stealth_sender.cpp \
    src/wallet/addresses/tiff.cpp \
    src/wallet/addresses/uri.cpp \
    src/wallet/addresses/witness_address.cpp \
    src/wallet/addresses/qrencode/bitstream.c \
    src/wallet/addresses/qrencode/bitstream.h \
    src/wallet/addresses/qrencode/mask.c \
    src/wallet/addresses/qrencode/mask.h \
    src/wallet/addresses/qrencode/mmask.c \
    src/wallet/addresses/qrencode/mmask.h \
    src/wallet/addresses/qrencode/mqrspec.c \
    src/wallet/addresses/qrencode/mqrspec.h \
    src/wallet/addresses/qrencode/qrencode.c \
    src/wallet/addresses/qrencode/qrencode.h \
    src/wallet/addresses/qrencode/qrencode_inner.h \
    src/wallet/addresses/qrencode/qrinput.c \
    src/wallet/addresses/qrencode/qrinput.h \
    src/wallet/addresses/qrencode/qrspec.c \
    src/wallet/addresses/qrencode/qrspec.h \
    src/wallet/addresses/qrencode/rsecc.c \
    src/wallet/addresses/qrencode/rsecc.h \
    src/wallet/addresses/qrencode/split.c \
    src/wallet/addresses/qrencode/split.h \
    src/wallet/keys/ec_point.cpp \
    src/wallet/keys/ec_private.cpp \
    src/wallet/keys/ec_public.cpp \
    src/wallet/keys/ec_scalar.cpp \
    src/wallet/keys/ek_private.cpp \
    src/wallet/keys/ek_public.cpp \
    src/wallet/keys/ek_token.cpp \
    src/wallet/keys/encrypted_keys.cpp \
    src/wallet/keys/hd_private.cpp \
    src/wallet/keys/hd_public.cpp \
    src/wallet/keys/mini_keys.cpp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_key.hpp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_key.ipp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_prefix.hpp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_prefix.ipp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_private.cpp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_private.hpp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_public.cpp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_public.hpp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_token.cpp \
    src/wallet/keys/parse_encrypted_keys/parse_encrypted_token.hpp \
    src/wallet/mnemonics/electrum.cpp \
    src/wallet/mnemonics/electrum_v1.cpp \
    src/wallet/mnemonics/languages.cpp \
    src/wallet/mnemonics/mnemonic.cpp \
    src/wallet/mnemonics/dictionaries/electrum.cpp \
    src/wallet/mnemonics/dictionaries/electrum_v1.cpp \
    src/wallet/mnemonics/dictionaries/mnemonic.cpp

# local: examples/libbitcoin-system-examples
#------------------------------------------------------------------------------
if WITH_EXAMPLES

noinst_PROGRAMS = examples/libbitcoin-system-examples
examples_libbitcoin_system_examples_CPPFLAGS = -I${srcdir}/include ${icu} ${boost_BUILD_CPPFLAGS} ${pthread_BUILD_CPPFLAGS} ${icu_i18n_BUILD_CPPFLAGS} ${secp256k1_BUILD_CPPFLAGS}
examples_libbitcoin_system_examples_LDFLAGS = ${boost_LDFLAGS}
examples_libbitcoin_system_examples_LDADD = src/libbitcoin-system.la ${boost_chrono_LIBS} ${boost_date_time_LIBS} ${boost_filesystem_LIBS} ${boost_iostreams_LIBS} ${boost_locale_LIBS} ${boost_log_LIBS} ${boost_program_options_LIBS} ${boost_regex_LIBS} ${boost_system_LIBS} ${boost_thread_LIBS} ${pthread_LIBS} ${rt_LIBS} ${icu_i18n_LIBS} ${dl_LIBS} ${secp256k1_LIBS}
examples_libbitcoin_system_examples_SOURCES = \
    examples/main.cpp

endif WITH_EXAMPLES

# local: test/libbitcoin-system-test
#------------------------------------------------------------------------------
if WITH_TESTS

TESTS = libbitcoin-system-test_runner.sh

check_PROGRAMS = test/libbitcoin-system-test
test_libbitcoin_system_test_CPPFLAGS = -I${srcdir}/include ${icu} ${boost_BUILD_CPPFLAGS} ${pthread_BUILD_CPPFLAGS} ${icu_i18n_BUILD_CPPFLAGS} ${secp256k1_BUILD_CPPFLAGS}
test_libbitcoin_system_test_LDFLAGS = ${boost_LDFLAGS}
test_libbitcoin_system_test_LDADD = src/libbitcoin-system.la ${boost_unit_test_framework_LIBS} ${boost_chrono_LIBS} ${boost_date_time_LIBS} ${boost_filesystem_LIBS} ${boost_iostreams_LIBS} ${boost_locale_LIBS} ${boost_log_LIBS} ${boost_program_options_LIBS} ${boost_regex_LIBS} ${boost_system_LIBS} ${boost_thread_LIBS} ${pthread_LIBS} ${rt_LIBS} ${icu_i18n_LIBS} ${dl_LIBS} ${secp256k1_LIBS}
test_libbitcoin_system_test_SOURCES = \
    test/constants.cpp \
    test/main.cpp \
    test/settings.cpp \
    test/test.cpp \
    test/test.hpp \
    test/type_constraints.cpp \
    test/chain/block.cpp \
    test/chain/block_filter.cpp \
    test/chain/chain_state.cpp \
    test/chain/compact.cpp \
    test/chain/header.cpp \
    test/chain/input.cpp \
    test/chain/output.cpp \
    test/chain/output_point.cpp \
    test/chain/payment_record.cpp \
    test/chain/point.cpp \
    test/chain/point_value.cpp \
    test/chain/points_value.cpp \
    test/chain/satoshi_words.cpp \
    test/chain/script.cpp \
    test/chain/script.hpp \
    test/chain/stealth_record.cpp \
    test/chain/transaction.cpp \
    test/concurrency/file_lock.cpp \
    test/concurrency/flush_lock.cpp \
    test/concurrency/interprocess_lock.cpp \
    test/concurrency/thread.cpp \
    test/config/authority.cpp \
    test/config/base16.cpp \
    test/config/base2.cpp \
    test/config/base32.cpp \
    test/config/base58.cpp \
    test/config/base64.cpp \
    test/config/base85.cpp \
    test/config/block.cpp \
    test/config/checkpoint.cpp \
    test/config/compact_filter.cpp \
    test/config/endpoint.cpp \
    test/config/hash256.cpp \
    test/config/parameter.cpp \
    test/config/printer.cpp \
    test/data/binary.cpp \
    test/data/collection.cpp \
    test/data/data_array.cpp \
    test/data/data_chunk.cpp \
    test/data/data_slab.cpp \
    test/data/data_slice.cpp \
    test/data/integer.cpp \
    test/data/string.cpp \
    test/iostream/removed.cpp \
    test/iostream/readers/bit_reader.cpp \
    test/iostream/readers/byte_reader.cpp \
    test/iostream/sinks/copy_sink.cpp \
    test/iostream/sinks/push_sink.cpp \
    test/iostream/sources/copy_source.cpp \
    test/iostream/sources/move_source.cpp \
    test/iostream/writers/bit_writer.cpp \
    test/iostream/writers/byte_writer.cpp \
    test/machine/number.cpp \
    test/machine/number.hpp \
    test/machine/opcode.cpp \
    test/machine/operation.cpp \
    test/math/addition.cpp \
    test/math/checksum.cpp \
    test/math/division.cpp \
    test/math/elliptic_curve.cpp \
    test/math/hash.cpp \
    test/math/hash.hpp \
    test/math/limits.cpp \
    test/math/power.cpp \
    test/math/pseudo_random.cpp \
    test/math/ring_signature.cpp \
    test/math/safe.cpp \
    test/math/sign.cpp \
    test/math/siphash.cpp \
    test/math/siphash.hpp \
    test/math/stealth.cpp \
    test/message/address.cpp \
    test/message/alert.cpp \
    test/message/alert_payload.cpp \
    test/message/block.cpp \
    test/message/block_transactions.cpp \
    test/message/compact_block.cpp \
    test/message/compact_filter.cpp \
    test/message/compact_filter_checkpoint.cpp \
    test/message/compact_filter_headers.cpp \
    test/message/fee_filter.cpp \
    test/message/filter_add.cpp \
    test/message/filter_clear.cpp \
    test/message/filter_load.cpp \
    test/message/get_address.cpp \
    test/message/get_block_transactions.cpp \
    test/message/get_blocks.cpp \
    test/message/get_compact_filter_checkpoint.cpp \
    test/message/get_compact_filter_headers.cpp \
    test/message/get_compact_filters.cpp \
    test/message/get_data.cpp \
    test/message/get_headers.cpp \
    test/message/header.cpp \
    test/message/headers.cpp \
    test/message/heading.cpp \
    test/message/inventory.cpp \
    test/message/inventory_vector.cpp \
    test/message/memory_pool.cpp \
    test/message/merkle_block.cpp \
    test/message/messages.cpp \
    test/message/network_address.cpp \
    test/message/not_found.cpp \
    test/message/ping.cpp \
    test/message/pong.cpp \
    test/message/prefilled_transaction.cpp \
    test/message/reject.cpp \
    test/message/send_compact.cpp \
    test/message/send_headers.cpp \
    test/message/transaction.cpp \
    test/message/verack.cpp \
    test/message/version.cpp \
    test/radix/base_10.cpp \
    test/radix/base_16.cpp \
    test/radix/base_2048.cpp \
    test/radix/base_32.cpp \
    test/radix/base_58.cpp \
    test/radix/base_64.cpp \
    test/radix/base_85.cpp \
    test/serialization/deserialize.cpp \
    test/serialization/endian.cpp \
    test/serialization/serialize.cpp \
    test/unicode/ascii.cpp \
    test/unicode/code_points.cpp \
    test/unicode/conversion.cpp \
    test/unicode/normalization.cpp \
    test/unicode/utf8_everywhere/utf8_environment.cpp \
    test/unicode/utf8_everywhere/utf8_ifstream.cpp \
    test/unicode/utf8_everywhere/utf8_istream.cpp \
    test/unicode/utf8_everywhere/utf8_ofstream.cpp \
    test/unicode/utf8_everywhere/utf8_ostream.cpp \
    test/wallet/context.cpp \
    test/wallet/message.cpp \
    test/wallet/neutrino_filter.cpp \
    test/wallet/property_tree.cpp \
    test/wallet/addresses/bitcoin_uri.cpp \
    test/wallet/addresses/checked.cpp \
    test/wallet/addresses/payment_address.cpp \
    test/wallet/addresses/qr_code.cpp \
    test/wallet/addresses/stealth_address.cpp \
    test/wallet/addresses/stealth_receiver.cpp \
    test/wallet/addresses/stealth_sender.cpp \
    test/wallet/addresses/tiff.cpp \
    test/wallet/addresses/uri.cpp \
    test/wallet/addresses/uri_reader.cpp \
    test/wallet/addresses/witness_address.cpp \
    test/wallet/keys/ec_point.cpp \
    test/wallet/keys/ec_private.cpp \
    test/wallet/keys/ec_public.cpp \
    test/wallet/keys/ec_scalar.cpp \
    test/wallet/keys/encrypted_keys.cpp \
    test/wallet/keys/hd_private.cpp \
    test/wallet/keys/hd_public.cpp \
    test/wallet/mnemonics/dictionaries.cpp \
    test/wallet/mnemonics/dictionaries.hpp \
    test/wallet/mnemonics/dictionary.cpp \
    test/wallet/mnemonics/dictionary.hpp \
    test/wallet/mnemonics/electrum.cpp \
    test/wallet/mnemonics/electrum.hpp \
    test/wallet/mnemonics/electrum_v1.cpp \
    test/wallet/mnemonics/electrum_v1.hpp \
    test/wallet/mnemonics/languages.cpp \
    test/wallet/mnemonics/languages.hpp \
    test/wallet/mnemonics/mnemonic.cpp \
    test/wallet/mnemonics/mnemonic.hpp \
    test/wallet/mnemonics/dictionaries/electrum.cpp \
    test/wallet/mnemonics/dictionaries/electrum.hpp \
    test/wallet/mnemonics/dictionaries/electrum_v1.cpp \
    test/wallet/mnemonics/dictionaries/electrum_v1.hpp \
    test/wallet/mnemonics/dictionaries/mnemonic.cpp \
    test/wallet/mnemonics/dictionaries/mnemonic.hpp

endif WITH_TESTS

# files => ${includedir}/bitcoin
#------------------------------------------------------------------------------
include_bitcoindir = ${includedir}/bitcoin
include_bitcoin_HEADERS = \
    include/bitcoin/system.hpp

include_bitcoin_systemdir = ${includedir}/bitcoin/system
include_bitcoin_system_HEADERS = \
    include/bitcoin/system/assert.hpp \
    include/bitcoin/system/constants.hpp \
    include/bitcoin/system/define.hpp \
    include/bitcoin/system/error.hpp \
    include/bitcoin/system/exceptions.hpp \
    include/bitcoin/system/settings.hpp \
    include/bitcoin/system/type_constraints.hpp \
    include/bitcoin/system/version.hpp

include_bitcoin_system_chaindir = ${includedir}/bitcoin/system/chain
include_bitcoin_system_chain_HEADERS = \
    include/bitcoin/system/chain/block.hpp \
    include/bitcoin/system/chain/block_filter.hpp \
    include/bitcoin/system/chain/chain.hpp \
    include/bitcoin/system/chain/chain_state.hpp \
    include/bitcoin/system/chain/compact.hpp \
    include/bitcoin/system/chain/header.hpp \
    include/bitcoin/system/chain/input.hpp \
    include/bitcoin/system/chain/input_point.hpp \
    include/bitcoin/system/chain/output.hpp \
    include/bitcoin/system/chain/output_point.hpp \
    include/bitcoin/system/chain/payment_record.hpp \
    include/bitcoin/system/chain/point.hpp \
    include/bitcoin/system/chain/point_value.hpp \
    include/bitcoin/system/chain/points_value.hpp \
    include/bitcoin/system/chain/script.hpp \
    include/bitcoin/system/chain/stealth_record.hpp \
    include/bitcoin/system/chain/transaction.hpp \
    include/bitcoin/system/chain/witness.hpp

include_bitcoin_system_concurrencydir = ${includedir}/bitcoin/system/concurrency
include_bitcoin_system_concurrency_HEADERS = \
    include/bitcoin/system/concurrency/asio.hpp \
    include/bitcoin/system/concurrency/atomic.hpp \
    include/bitcoin/system/concurrency/concurrency.hpp \
    include/bitcoin/system/concurrency/conditional_lock.hpp \
    include/bitcoin/system/concurrency/deadline.hpp \
    include/bitcoin/system/concurrency/decorator.hpp \
    include/bitcoin/system/concurrency/delegates.hpp \
    include/bitcoin/system/concurrency/dispatcher.hpp \
    include/bitcoin/system/concurrency/enable_shared_from_base.hpp \
    include/bitcoin/system/concurrency/file_lock.hpp \
    include/bitcoin/system/concurrency/flush_lock.hpp \
    include/bitcoin/system/concurrency/handlers.hpp \
    include/bitcoin/system/concurrency/interprocess_lock.hpp \
    include/bitcoin/system/concurrency/monitor.hpp \
    include/bitcoin/system/concurrency/pending.hpp \
    include/bitcoin/system/concurrency/prioritized_mutex.hpp \
    include/bitcoin/system/concurrency/resubscriber.hpp \
    include/bitcoin/system/concurrency/scope_lock.hpp \
    include/bitcoin/system/concurrency/sequencer.hpp \
    include/bitcoin/system/concurrency/sequential_lock.hpp \
    include/bitcoin/system/concurrency/socket.hpp \
    include/bitcoin/system/concurrency/subscriber.hpp \
    include/bitcoin/system/concurrency/synchronizer.hpp \
    include/bitcoin/system/concurrency/thread.hpp \
    include/bitcoin/system/concurrency/threadpool.hpp \
    include/bitcoin/system/concurrency/timer.hpp \
    include/bitcoin/system/concurrency/track.hpp \
    include/bitcoin/system/concurrency/work.hpp

include_bitcoin_system_configdir = ${includedir}/bitcoin/system/config
include_bitcoin_system_config_HEADERS = \
    include/bitcoin/system/config/authority.hpp \
    include/bitcoin/system/config/base16.hpp \
    include/bitcoin/system/config/base2.hpp \
    include/bitcoin/system/config/base32.hpp \
    include/bitcoin/system/config/base58.hpp \
    include/bitcoin/system/config/base64.hpp \
    include/bitcoin/system/config/base85.hpp \
    include/bitcoin/system/config/block.hpp \
    include/bitcoin/system/config/checkpoint.hpp \
    include/bitcoin/system/config/compact_filter.hpp \
    include/bitcoin/system/config/config.hpp \
    include/bitcoin/system/config/directory.hpp \
    include/bitcoin/system/config/endpoint.hpp \
    include/bitcoin/system/config/hash160.hpp \
    include/bitcoin/system/config/hash256.hpp \
    include/bitcoin/system/config/header.hpp \
    include/bitcoin/system/config/input.hpp \
    include/bitcoin/system/config/output.hpp \
    include/bitcoin/system/config/parameter.hpp \
    include/bitcoin/system/config/parser.hpp \
    include/bitcoin/system/config/point.hpp \
    include/bitcoin/system/config/printer.hpp \
    include/bitcoin/system/config/script.hpp \
    include/bitcoin/system/config/settings.hpp \
    include/bitcoin/system/config/transaction.hpp

include_bitcoin_system_datadir = ${includedir}/bitcoin/system/data
include_bitcoin_system_data_HEADERS = \
    include/bitcoin/system/data/binary.hpp \
    include/bitcoin/system/data/collection.hpp \
    include/bitcoin/system/data/data.hpp \
    include/bitcoin/system/data/data_array.hpp \
    include/bitcoin/system/data/data_chunk.hpp \
    include/bitcoin/system/data/data_slab.hpp \
    include/bitcoin/system/data/data_slice.hpp \
    include/bitcoin/system/data/string.hpp \
    include/bitcoin/system/data/uintx.hpp

include_bitcoin_system_impl_concurrencydir = ${includedir}/bitcoin/system/impl/concurrency
include_bitcoin_system_impl_concurrency_HEADERS = \
    include/bitcoin/system/impl/concurrency/pending.ipp \
    include/bitcoin/system/impl/concurrency/resubscriber.ipp \
    include/bitcoin/system/impl/concurrency/subscriber.ipp \
    include/bitcoin/system/impl/concurrency/track.ipp

include_bitcoin_system_impl_datadir = ${includedir}/bitcoin/system/impl/data
include_bitcoin_system_impl_data_HEADERS = \
    include/bitcoin/system/impl/data/collection.ipp \
    include/bitcoin/system/impl/data/data_array.ipp \
    include/bitcoin/system/impl/data/data_slab.ipp \
    include/bitcoin/system/impl/data/data_slice.ipp \
    include/bitcoin/system/impl/data/string.ipp

include_bitcoin_system_impl_iostream_readersdir = ${includedir}/bitcoin/system/impl/iostream/readers
include_bitcoin_system_impl_iostream_readers_HEADERS = \
    include/bitcoin/system/impl/iostream/readers/bit_reader.ipp \
    include/bitcoin/system/impl/iostream/readers/byte_reader.ipp

include_bitcoin_system_impl_iostream_sinksdir = ${includedir}/bitcoin/system/impl/iostream/sinks
include_bitcoin_system_impl_iostream_sinks_HEADERS = \
    include/bitcoin/system/impl/iostream/sinks/copy_sink.ipp \
    include/bitcoin/system/impl/iostream/sinks/push_sink.ipp \
    include/bitcoin/system/impl/iostream/sinks/sink.ipp

include_bitcoin_system_impl_iostream_sourcesdir = ${includedir}/bitcoin/system/impl/iostream/sources
include_bitcoin_system_impl_iostream_sources_HEADERS = \
    include/bitcoin/system/impl/iostream/sources/copy_source.ipp \
    include/bitcoin/system/impl/iostream/sources/move_source.ipp \
    include/bitcoin/system/impl/iostream/sources/source.ipp

include_bitcoin_system_impl_iostream_writersdir = ${includedir}/bitcoin/system/impl/iostream/writers
include_bitcoin_system_impl_iostream_writers_HEADERS = \
    include/bitcoin/system/impl/iostream/writers/bit_writer.ipp \
    include/bitcoin/system/impl/iostream/writers/byte_writer.ipp

include_bitcoin_system_impl_log_featuresdir = ${includedir}/bitcoin/system/impl/log/features
include_bitcoin_system_impl_log_features_HEADERS = \
    include/bitcoin/system/impl/log/features/counter.ipp \
    include/bitcoin/system/impl/log/features/gauge.ipp \
    include/bitcoin/system/impl/log/features/metric.ipp \
    include/bitcoin/system/impl/log/features/rate.ipp \
    include/bitcoin/system/impl/log/features/timer.ipp

include_bitcoin_system_impl_machinedir = ${includedir}/bitcoin/system/impl/machine
include_bitcoin_system_impl_machine_HEADERS = \
    include/bitcoin/system/impl/machine/interpreter.ipp \
    include/bitcoin/system/impl/machine/number.ipp \
    include/bitcoin/system/impl/machine/operation.ipp \
    include/bitcoin/system/impl/machine/program.ipp

include_bitcoin_system_impl_mathdir = ${includedir}/bitcoin/system/impl/math
include_bitcoin_system_impl_math_HEADERS = \
    include/bitcoin/system/impl/math/addition.ipp \
    include/bitcoin/system/impl/math/checksum.ipp \
    include/bitcoin/system/impl/math/division.ipp \
    include/bitcoin/system/impl/math/hash.ipp \
    include/bitcoin/system/impl/math/limits.ipp \
    include/bitcoin/system/impl/math/power.ipp \
    include/bitcoin/system/impl/math/safe.ipp \
    include/bitcoin/system/impl/math/sign.ipp

include_bitcoin_system_impl_radixdir = ${includedir}/bitcoin/system/impl/radix
include_bitcoin_system_impl_radix_HEADERS = \
    include/bitcoin/system/impl/radix/base_16.ipp \
    include/bitcoin/system/impl/radix/base_58.ipp

include_bitcoin_system_impl_serializationdir = ${includedir}/bitcoin/system/impl/serialization
include_bitcoin_system_impl_serialization_HEADERS = \
    include/bitcoin/system/impl/serialization/deserialize.ipp \
    include/bitcoin/system/impl/serialization/endian.ipp \
    include/bitcoin/system/impl/serialization/serialize.ipp

include_bitcoin_system_impl_walletdir = ${includedir}/bitcoin/system/impl/wallet
include_bitcoin_system_impl_wallet_HEADERS = \
    include/bitcoin/system/impl/wallet/property_tree.ipp

include_bitcoin_system_impl_wallet_addressesdir = ${includedir}/bitcoin/system/impl/wallet/addresses
include_bitcoin_system_impl_wallet_addresses_HEADERS = \
    include/bitcoin/system/impl/wallet/addresses/checked.ipp

include_bitcoin_system_impl_wallet_mnemonicsdir = ${includedir}/bitcoin/system/impl/wallet/mnemonics
include_bitcoin_system_impl_wallet_mnemonics_HEADERS = \
    include/bitcoin/system/impl/wallet/mnemonics/dictionaries.ipp \
    include/bitcoin/system/impl/wallet/mnemonics/dictionary.ipp

include_bitcoin_system_iostreamdir = ${includedir}/bitcoin/system/iostream
include_bitcoin_system_iostream_HEADERS = \
    include/bitcoin/system/iostream/iostream.hpp

include_bitcoin_system_iostream_interfacesdir = ${includedir}/bitcoin/system/iostream/interfaces
include_bitcoin_system_iostream_interfaces_HEADERS = \
    include/bitcoin/system/iostream/interfaces/bitreader.hpp \
    include/bitcoin/system/iostream/interfaces/bitwriter.hpp \
    include/bitcoin/system/iostream/interfaces/reader.hpp \
    include/bitcoin/system/iostream/interfaces/writer.hpp

include_bitcoin_system_iostream_readersdir = ${includedir}/bitcoin/system/iostream/readers
include_bitcoin_system_iostream_readers_HEADERS = \
    include/bitcoin/system/iostream/readers/bit_reader.hpp \
    include/bitcoin/system/iostream/readers/byte_reader.hpp \
    include/bitcoin/system/iostream/readers/readers.hpp

include_bitcoin_system_iostream_sinksdir = ${includedir}/bitcoin/system/iostream/sinks
include_bitcoin_system_iostream_sinks_HEADERS = \
    include/bitcoin/system/iostream/sinks/copy_sink.hpp \
    include/bitcoin/system/iostream/sinks/push_sink.hpp \
    include/bitcoin/system/iostream/sinks/sink.hpp

include_bitcoin_system_iostream_sourcesdir = ${includedir}/bitcoin/system/iostream/sources
include_bitcoin_system_iostream_sources_HEADERS = \
    include/bitcoin/system/iostream/sources/copy_source.hpp \
    include/bitcoin/system/iostream/sources/move_source.hpp \
    include/bitcoin/system/iostream/sources/source.hpp

include_bitcoin_system_iostream_writersdir = ${includedir}/bitcoin/system/iostream/writers
include_bitcoin_system_iostream_writers_HEADERS = \
    include/bitcoin/system/iostream/writers/bit_writer.hpp \
    include/bitcoin/system/iostream/writers/byte_writer.hpp \
    include/bitcoin/system/iostream/writers/writers.hpp

include_bitcoin_system_logdir = ${includedir}/bitcoin/system/log
include_bitcoin_system_log_HEADERS = \
    include/bitcoin/system/log/attributes.hpp \
    include/bitcoin/system/log/file_char_traits.hpp \
    include/bitcoin/system/log/file_collector.hpp \
    include/bitcoin/system/log/file_collector_repository.hpp \
    include/bitcoin/system/log/file_counter_formatter.hpp \
    include/bitcoin/system/log/log.hpp \
    include/bitcoin/system/log/rotable_file.hpp \
    include/bitcoin/system/log/severity.hpp \
    include/bitcoin/system/log/sink.hpp \
    include/bitcoin/system/log/source.hpp \
    include/bitcoin/system/log/statsd_sink.hpp \
    include/bitcoin/system/log/statsd_source.hpp \
    include/bitcoin/system/log/udp_client_sink.hpp

include_bitcoin_system_log_featuresdir = ${includedir}/bitcoin/system/log/features
include_bitcoin_system_log_features_HEADERS = \
    include/bitcoin/system/log/features/counter.hpp \
    include/bitcoin/system/log/features/gauge.hpp \
    include/bitcoin/system/log/features/metric.hpp \
    include/bitcoin/system/log/features/rate.hpp \
    include/bitcoin/system/log/features/timer.hpp

include_bitcoin_system_machinedir = ${includedir}/bitcoin/system/machine
include_bitcoin_system_machine_HEADERS = \
    include/bitcoin/system/machine/interpreter.hpp \
    include/bitcoin/system/machine/machine.hpp \
    include/bitcoin/system/machine/number.hpp \
    include/bitcoin/system/machine/opcode.hpp \
    include/bitcoin/system/machine/operation.hpp \
    include/bitcoin/system/machine/program.hpp \
    include/bitcoin/system/machine/rule_fork.hpp \
    include/bitcoin/system/machine/script_pattern.hpp \
    include/bitcoin/system/machine/script_version.hpp \
    include/bitcoin/system/machine/sighash_algorithm.hpp

include_bitcoin_system_mathdir = ${includedir}/bitcoin/system/math
include_bitcoin_system_math_HEADERS = \
    include/bitcoin/system/math/addition.hpp \
    include/bitcoin/system/math/checksum.hpp \
    include/bitcoin/system/math/crypto.hpp \
    include/bitcoin/system/math/division.hpp \
    include/bitcoin/system/math/elliptic_curve.hpp \
    include/bitcoin/system/math/golomb_coding.hpp \
    include/bitcoin/system/math/hash.hpp \
    include/bitcoin/system/math/limits.hpp \
    include/bitcoin/system/math/math.hpp \
    include/bitcoin/system/math/power.hpp \
    include/bitcoin/system/math/pseudo_random.hpp \
    include/bitcoin/system/math/ring_signature.hpp \
    include/bitcoin/system/math/safe.hpp \
    include/bitcoin/system/math/sign.hpp \
    include/bitcoin/system/math/siphash.hpp \
    include/bitcoin/system/math/stealth.hpp

include_bitcoin_system_messagedir = ${includedir}/bitcoin/system/message
include_bitcoin_system_message_HEADERS = \
    include/bitcoin/system/message/address.hpp \
    include/bitcoin/system/message/alert.hpp \
    include/bitcoin/system/message/alert_payload.hpp \
    include/bitcoin/system/message/block.hpp \
    include/bitcoin/system/message/block_transactions.hpp \
    include/bitcoin/system/message/compact_block.hpp \
    include/bitcoin/system/message/compact_filter.hpp \
    include/bitcoin/system/message/compact_filter_checkpoint.hpp \
    include/bitcoin/system/message/compact_filter_headers.hpp \
    include/bitcoin/system/message/fee_filter.hpp \
    include/bitcoin/system/message/filter_add.hpp \
    include/bitcoin/system/message/filter_clear.hpp \
    include/bitcoin/system/message/filter_load.hpp \
    include/bitcoin/system/message/get_address.hpp \
    include/bitcoin/system/message/get_block_transactions.hpp \
    include/bitcoin/system/message/get_blocks.hpp \
    include/bitcoin/system/message/get_compact_filter_checkpoint.hpp \
    include/bitcoin/system/message/get_compact_filter_headers.hpp \
    include/bitcoin/system/message/get_compact_filters.hpp \
    include/bitcoin/system/message/get_data.hpp \
    include/bitcoin/system/message/get_headers.hpp \
    include/bitcoin/system/message/header.hpp \
    include/bitcoin/system/message/headers.hpp \
    include/bitcoin/system/message/heading.hpp \
    include/bitcoin/system/message/inventory.hpp \
    include/bitcoin/system/message/inventory_vector.hpp \
    include/bitcoin/system/message/memory_pool.hpp \
    include/bitcoin/system/message/merkle_block.hpp \
    include/bitcoin/system/message/message.hpp \
    include/bitcoin/system/message/network_address.hpp \
    include/bitcoin/system/message/not_found.hpp \
    include/bitcoin/system/message/ping.hpp \
    include/bitcoin/system/message/pong.hpp \
    include/bitcoin/system/message/prefilled_transaction.hpp \
    include/bitcoin/system/message/reject.hpp \
    include/bitcoin/system/message/send_compact.hpp \
    include/bitcoin/system/message/send_headers.hpp \
    include/bitcoin/system/message/transaction.hpp \
    include/bitcoin/system/message/verack.hpp \
    include/bitcoin/system/message/version.hpp

include_bitcoin_system_radixdir = ${includedir}/bitcoin/system/radix
include_bitcoin_system_radix_HEADERS = \
    include/bitcoin/system/radix/base_10.hpp \
    include/bitcoin/system/radix/base_16.hpp \
    include/bitcoin/system/radix/base_2048.hpp \
    include/bitcoin/system/radix/base_32.hpp \
    include/bitcoin/system/radix/base_58.hpp \
    include/bitcoin/system/radix/base_64.hpp \
    include/bitcoin/system/radix/base_85.hpp \
    include/bitcoin/system/radix/radix.hpp

include_bitcoin_system_serializationdir = ${includedir}/bitcoin/system/serialization
include_bitcoin_system_serialization_HEADERS = \
    include/bitcoin/system/serialization/deserialize.hpp \
    include/bitcoin/system/serialization/endian.hpp \
    include/bitcoin/system/serialization/serialization.hpp \
    include/bitcoin/system/serialization/serialize.hpp

include_bitcoin_system_unicodedir = ${includedir}/bitcoin/system/unicode
include_bitcoin_system_unicode_HEADERS = \
    include/bitcoin/system/unicode/ascii.hpp \
    include/bitcoin/system/unicode/code_points.hpp \
    include/bitcoin/system/unicode/conversion.hpp \
    include/bitcoin/system/unicode/normalization.hpp \
    include/bitcoin/system/unicode/unicode.hpp

include_bitcoin_system_unicode_utf8_everywheredir = ${includedir}/bitcoin/system/unicode/utf8_everywhere
include_bitcoin_system_unicode_utf8_everywhere_HEADERS = \
    include/bitcoin/system/unicode/utf8_everywhere/utf8_about.hpp \
    include/bitcoin/system/unicode/utf8_everywhere/utf8_console_streambuf.hpp \
    include/bitcoin/system/unicode/utf8_everywhere/utf8_environment.hpp \
    include/bitcoin/system/unicode/utf8_everywhere/utf8_everywhere.hpp \
    include/bitcoin/system/unicode/utf8_everywhere/utf8_ifstream.hpp \
    include/bitcoin/system/unicode/utf8_everywhere/utf8_istream.hpp \
    include/bitcoin/system/unicode/utf8_everywhere/utf8_ofstream.hpp \
    include/bitcoin/system/unicode/utf8_everywhere/utf8_ostream.hpp \
    include/bitcoin/system/unicode/utf8_everywhere/utf8_streambuf.hpp

include_bitcoin_system_walletdir = ${includedir}/bitcoin/system/wallet
include_bitcoin_system_wallet_HEADERS = \
    include/bitcoin/system/wallet/context.hpp \
    include/bitcoin/system/wallet/message.hpp \
    include/bitcoin/system/wallet/neutrino_filter.hpp \
    include/bitcoin/system/wallet/property_tree.hpp \
    include/bitcoin/system/wallet/wallet.hpp

include_bitcoin_system_wallet_addressesdir = ${includedir}/bitcoin/system/wallet/addresses
include_bitcoin_system_wallet_addresses_HEADERS = \
    include/bitcoin/system/wallet/addresses/addresses.hpp \
    include/bitcoin/system/wallet/addresses/bitcoin_uri.hpp \
    include/bitcoin/system/wallet/addresses/checked.hpp \
    include/bitcoin/system/wallet/addresses/payment_address.hpp \
    include/bitcoin/system/wallet/addresses/qr_code.hpp \
    include/bitcoin/system/wallet/addresses/stealth_address.hpp \
    include/bitcoin/system/wallet/addresses/stealth_receiver.hpp \
    include/bitcoin/system/wallet/addresses/stealth_sender.hpp \
    include/bitcoin/system/wallet/addresses/tiff.hpp \
    include/bitcoin/system/wallet/addresses/uri.hpp \
    include/bitcoin/system/wallet/addresses/uri_reader.hpp \
    include/bitcoin/system/wallet/addresses/witness_address.hpp

include_bitcoin_system_wallet_keysdir = ${includedir}/bitcoin/system/wallet/keys
include_bitcoin_system_wallet_keys_HEADERS = \
    include/bitcoin/system/wallet/keys/ec_point.hpp \
    include/bitcoin/system/wallet/keys/ec_private.hpp \
    include/bitcoin/system/wallet/keys/ec_public.hpp \
    include/bitcoin/system/wallet/keys/ec_scalar.hpp \
    include/bitcoin/system/wallet/keys/ek_private.hpp \
    include/bitcoin/system/wallet/keys/ek_public.hpp \
    include/bitcoin/system/wallet/keys/ek_token.hpp \
    include/bitcoin/system/wallet/keys/encrypted_keys.hpp \
    include/bitcoin/system/wallet/keys/hd_private.hpp \
    include/bitcoin/system/wallet/keys/hd_public.hpp \
    include/bitcoin/system/wallet/keys/keys.hpp \
    include/bitcoin/system/wallet/keys/mini_keys.hpp

include_bitcoin_system_wallet_mnemonicsdir = ${includedir}/bitcoin/system/wallet/mnemonics
include_bitcoin_system_wallet_mnemonics_HEADERS = \
    include/bitcoin/system/wallet/mnemonics/dictionaries.hpp \
    include/bitcoin/system/wallet/mnemonics/dictionary.hpp \
    include/bitcoin/system/wallet/mnemonics/electrum.hpp \
    include/bitcoin/system/wallet/mnemonics/electrum_v1.hpp \
    include/bitcoin/system/wallet/mnemonics/language.hpp \
    include/bitcoin/system/wallet/mnemonics/languages.hpp \
    include/bitcoin/system/wallet/mnemonics/mnemonic.hpp \
    include/bitcoin/system/wallet/mnemonics/mnemonics.hpp


# Custom make targets.
#==============================================================================
# make target: examples
#------------------------------------------------------------------------------
target_examples = \
    examples/libbitcoin-system-examples

examples: ${target_examples}

