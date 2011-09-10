CFLAGS= -std=c++0x -Wall -pedantic -pthread -Wextra -fstack-protector -ggdb -Iinclude/ -Iusr/include/ -c
LIBS= usr/lib/libcppdb.a -lcrypto -lboost_thread -lboost_system -ldl -lpq

objs: block transaction logger

block: src/block.cpp
	$(CXX) $(CFLAGS) -o obj/block.o src/block.cpp

obj/ripemd.o: src/util/ripemd.cpp
	$(CXX) $(CFLAGS) -o obj/ripemd.o src/util/ripemd.cpp

sha256: src/util/sha256.cpp
	$(CXX) $(CFLAGS) -o obj/sha256.o src/util/sha256.cpp

obj/logger.o: src/util/logger.cpp include/bitcoin/util/logger.hpp
	$(CXX) $(CFLAGS) -o obj/logger.o src/util/logger.cpp

transaction: src/transaction.cpp include/bitcoin/transaction.hpp
	$(CXX) $(CFLAGS) -o obj/transaction.o src/transaction.cpp

script: src/script.cpp include/bitcoin/script.hpp
	$(CXX) $(CFLAGS) -o obj/script.o src/script.cpp

obj/block.o: src/block.cpp include/bitcoin/block.hpp
	$(CXX) $(CFLAGS) -o obj/block.o src/block.cpp

obj/network.o: src/network/network.cpp include/bitcoin/network/network.hpp
	$(CXX) $(CFLAGS) -o obj/network.o src/network/network.cpp

obj/dialect.o: src/dialect.cpp include/bitcoin/dialect.hpp
	$(CXX) $(CFLAGS) -o obj/dialect.o src/dialect.cpp

obj/channel.o: src/network/channel.cpp src/network/channel.hpp
	$(CXX) $(CFLAGS) -o obj/channel.o src/network/channel.cpp

obj/sha256.o: src/util/sha256.cpp include/bitcoin/util/sha256.hpp
	$(CXX) $(CFLAGS) -o obj/sha256.o src/util/sha256.cpp

obj/kernel.o: src/kernel.cpp include/bitcoin/kernel.hpp
	$(CXX) $(CFLAGS) -o obj/kernel.o src/kernel.cpp

obj/error.o: src/error.cpp include/bitcoin/error.hpp
	$(CXX) $(CFLAGS) -o obj/error.o src/error.cpp

obj/serializer.o: src/util/serializer.cpp include/bitcoin/util/serializer.hpp
	$(CXX) $(CFLAGS) -o obj/serializer.o src/util/serializer.cpp

obj/nettest.o: tests/net.cpp
	$(CXX) $(CFLAGS) -o obj/nettest.o tests/net.cpp

obj/postgresql_storage.o: src/storage/postgresql_storage.cpp include/bitcoin/storage/postgresql_storage.hpp obj/script.o obj/transaction.o
	$(CXX) $(CFLAGS) -o obj/postgresql_storage.o src/storage/postgresql_storage.cpp

obj/elliptic_curve_key.o: src/util/elliptic_curve_key.cpp include/bitcoin/util/elliptic_curve_key.hpp
	$(CXX) $(CFLAGS) -o obj/elliptic_curve_key.o src/util/elliptic_curve_key.cpp

bin/tests/nettest: obj/network.o  obj/dialect.o  obj/channel.o obj/serializer.o obj/logger.o obj/nettest.o obj/kernel.o obj/sha256.o obj/types.o obj/block.o obj/script.o obj/ripemd.o obj/postgresql_storage.o obj/block.o obj/elliptic_curve_key.o obj/transaction.o obj/error.o obj/threaded_service.o obj/postgresql_blockchain.o
	$(CXX) -o bin/tests/nettest obj/network.o obj/dialect.o obj/channel.o obj/serializer.o obj/logger.o obj/nettest.o obj/kernel.o obj/sha256.o obj/types.o obj/script.o obj/ripemd.o obj/postgresql_storage.o obj/block.o obj/elliptic_curve_key.o obj/transaction.o obj/error.o obj/threaded_service.o obj/postgresql_blockchain.o $(LIBS)

net: bin/tests/nettest

obj/threaded_service.o: src/util/threaded_service.cpp include/bitcoin/util/threaded_service.hpp
	$(CXX) $(CFLAGS) -o obj/threaded_service.o src/util/threaded_service.cpp

obj/verify.o: src/verify.cpp include/bitcoin/verify.hpp obj/threaded_service.o
	$(CXX) $(CFLAGS) -o obj/verify.o src/verify.cpp

obj/types.o: src/types.cpp include/bitcoin/types.hpp
	$(CXX) $(CFLAGS) -o obj/types.o src/types.cpp

obj/gengen.o: tests/gengen.cpp
	$(CXX) $(CFLAGS) -o obj/gengen.o tests/gengen.cpp

bin/tests/gengen: obj/gengen.o obj/logger.o obj/sha256.o  obj/types.o obj/serializer.o obj/types.o
	$(CXX) -o bin/tests/gengen obj/gengen.o obj/logger.o obj/sha256.o  obj/types.o obj/serializer.o $(LIBS)

gengen: bin/tests/gengen

obj/script.o: src/script.cpp include/bitcoin/script.hpp
	$(CXX) $(CFLAGS) -o obj/script.o src/script.cpp

obj/script-test.o: tests/script-test.cpp
	$(CXX) $(CFLAGS) -o obj/script-test.o tests/script-test.cpp

bin/tests/script-test: obj/script-test.o obj/script.o obj/logger.o obj/sha256.o obj/ripemd.o obj/types.o obj/postgresql_storage.o obj/transaction.o obj/block.o obj/serializer.o obj/elliptic_curve_key.o obj/error.o obj/postgresql_blockchain.o obj/threaded_service.o
	$(CXX) -o bin/tests/script-test obj/script-test.o obj/script.o obj/logger.o obj/sha256.o obj/ripemd.o obj/types.o obj/postgresql_storage.o obj/transaction.o obj/block.o obj/serializer.o obj/elliptic_curve_key.o obj/error.o obj/postgresql_blockchain.o obj/threaded_service.o $(LIBS)

obj/postbind.o: tests/postbind.cpp
	$(CXX) $(CFLAGS) -o obj/postbind.o tests/postbind.cpp

bin/tests/postbind: obj/postbind.o
	$(CXX) -o bin/tests/postbind obj/postbind.o $(LIBS)

postbind: bin/tests/postbind

script-test: bin/tests/script-test

obj/psql.o: tests/psql.cpp
	$(CXX) $(CFLAGS) -o obj/psql.o tests/psql.cpp

bin/tests/psql: obj/postgresql_storage.o obj/psql.o obj/logger.o obj/script.o obj/ripemd.o obj/block.o obj/serializer.o obj/sha256.o obj/types.o obj/transaction.o obj/error.o obj/elliptic_curve_key.o obj/threaded_service.o
	$(CXX) -o bin/tests/psql obj/psql.o obj/postgresql_storage.o obj/logger.o obj/script.o obj/ripemd.o obj/block.o obj/serializer.o obj/sha256.o obj/types.o obj/transaction.o obj/error.o obj/elliptic_curve_key.o obj/threaded_service.o $(LIBS)

psql: bin/tests/psql

obj/types-test.o: tests/types-test.cpp
	$(CXX) $(CFLAGS) -o obj/types-test.o tests/types-test.cpp

bin/tests/types-test: obj/types.o obj/types-test.o
	$(CXX) -o bin/tests/types-test obj/types.o obj/types-test.o $(LIBS)

types-test: bin/tests/types-test

obj/merkle.o: tests/merkle.cpp
	$(CXX) $(CFLAGS) -o obj/merkle.o tests/merkle.cpp

bin/tests/merkle: obj/merkle.o obj/postgresql_storage.o obj/sha256.o obj/script.o obj/logger.o obj/ripemd.o obj/types.o obj/block.o obj/serializer.o obj/transaction.o obj/elliptic_curve_key.o obj/error.o
	$(CXX) -o bin/tests/merkle obj/merkle.o obj/postgresql_storage.o obj/sha256.o obj/script.o obj/logger.o obj/ripemd.o obj/types.o obj/block.o obj/serializer.o obj/transaction.o obj/elliptic_curve_key.o obj/error.o $(LIBS)

merkle: bin/tests/merkle

obj/transaction.o: src/transaction.cpp
	$(CXX) $(CFLAGS) -o obj/transaction.o src/transaction.cpp

obj/tx-hash.o: tests/tx-hash.cpp
	$(CXX) $(CFLAGS) -o obj/tx-hash.o tests/tx-hash.cpp

bin/tests/tx-hash: obj/tx-hash.o obj/transaction.o obj/sha256.o obj/script.o obj/serializer.o obj/logger.o obj/types.o obj/ripemd.o obj/elliptic_curve_key.o
	$(CXX) -o bin/tests/tx-hash obj/tx-hash.o obj/transaction.o obj/sha256.o obj/script.o obj/serializer.o obj/logger.o obj/types.o obj/ripemd.o obj/elliptic_curve_key.o $(LIBS)

tx-hash: bin/tests/tx-hash

obj/block-hash.o: tests/block-hash.cpp
	$(CXX) $(CFLAGS) -o obj/block-hash.o tests/block-hash.cpp

bin/tests/block-hash: obj/block-hash.o obj/block.o obj/postgresql_storage.o obj/sha256.o obj/script.o obj/logger.o obj/ripemd.o obj/types.o obj/serializer.o obj/transaction.o obj/elliptic_curve_key.o obj/error.o
	$(CXX) -o bin/tests/block-hash obj/block-hash.o obj/block.o obj/postgresql_storage.o obj/sha256.o obj/script.o obj/logger.o obj/ripemd.o obj/types.o obj/serializer.o obj/transaction.o obj/elliptic_curve_key.o obj/error.o $(LIBS)

block-hash: bin/tests/block-hash

obj/ec-key.o: tests/ec-key.cpp
	$(CXX) $(CFLAGS) -o obj/ec-key.o tests/ec-key.cpp

bin/tests/ec-key: obj/ec-key.o obj/serializer.o obj/elliptic_curve_key.o obj/types.o obj/sha256.o obj/logger.o
	$(CXX) -o bin/tests/ec-key obj/ec-key.o obj/serializer.o obj/elliptic_curve_key.o obj/types.o obj/sha256.o obj/logger.o $(LIBS)

ec-key: bin/tests/ec-key

obj/clock.o: src/util/clock.cpp include/bitcoin/util/clock.hpp
	$(CXX) $(CFLAGS) -o obj/clock.o src/util/clock.cpp

obj/constants.o: src/constants.cpp include/bitcoin/constants.hpp
	$(CXX) $(CFLAGS) -o obj/constants.o src/constants.cpp

obj/verify-block.o: tests/verify-block.cpp
	$(CXX) $(CFLAGS) -o obj/verify-block.o tests/verify-block.cpp

bin/tests/verify-block: obj/verify-block.o obj/postgresql_storage.o obj/logger.o obj/serializer.o obj/elliptic_curve_key.o obj/sha256.o obj/ripemd.o obj/types.o obj/block.o obj/error.o obj/verify.o obj/dialect.o obj/constants.o obj/big_number.o obj/clock.o
	$(CXX) -o bin/tests/verify-block obj/verify-block.o obj/postgresql_storage.o obj/transaction.o obj/script.o obj/logger.o obj/serializer.o obj/elliptic_curve_key.o obj/sha256.o obj/ripemd.o obj/types.o obj/block.o obj/error.o obj/verify.o obj/threaded_service.o obj/dialect.o obj/constants.o obj/big_number.o obj/clock.o $(LIBS)

verify-block: bin/tests/verify-block

obj/big_number.o: src/util/big_number.cpp include/bitcoin/util/big_number.hpp
	$(CXX) $(CFLAGS) -o obj/big_number.o src/util/big_number.cpp

obj/big-number-test.o: tests/big-number-test.cpp
	$(CXX) $(CFLAGS) -o obj/big-number-test.o tests/big-number-test.cpp

bin/tests/big-number-test: obj/big-number-test.o obj/big_number.o obj/logger.o obj/constants.o
	$(CXX) -o bin/tests/big-number-test obj/big-number-test.o obj/big_number.o obj/logger.o obj/constants.o $(LIBS)

big-number-test: bin/tests/big-number-test

obj/poller.o: examples/poller.cpp
	$(CXX) $(CFLAGS) -o obj/poller.o examples/poller.cpp

bin/examples/poller: obj/poller.o obj/network.o  obj/dialect.o  obj/channel.o obj/serializer.o obj/logger.o obj/kernel.o obj/sha256.o obj/types.o obj/block.o obj/script.o obj/ripemd.o obj/postgresql_storage.o obj/block.o obj/elliptic_curve_key.o obj/transaction.o obj/error.o obj/threaded_service.o obj/postgresql_blockchain.o obj/verify.o obj/big_number.o obj/clock.o obj/constants.o
	$(CXX) -o bin/examples/poller obj/poller.o obj/network.o obj/dialect.o obj/channel.o obj/serializer.o obj/logger.o obj/kernel.o obj/sha256.o obj/types.o obj/script.o obj/ripemd.o obj/postgresql_storage.o obj/block.o obj/elliptic_curve_key.o obj/transaction.o obj/error.o obj/threaded_service.o obj/postgresql_blockchain.o obj/verify.o obj/big_number.o obj/clock.o obj/constants.o $(LIBS)

poller: bin/examples/poller

obj/postgresql_blockchain.o: src/storage/postgresql_blockchain.cpp src/storage/postgresql_blockchain.hpp
	$(CXX) $(CFLAGS) -o obj/postgresql_blockchain.o src/storage/postgresql_blockchain.cpp

obj/blockchain.o: tests/blockchain.cpp
	$(CXX) $(CFLAGS) -o obj/blockchain.o tests/blockchain.cpp

bin/tests/blockchain: obj/blockchain.o obj/network.o  obj/dialect.o  obj/channel.o obj/serializer.o obj/logger.o obj/kernel.o obj/sha256.o obj/types.o obj/script.o obj/ripemd.o obj/postgresql_storage.o obj/block.o obj/elliptic_curve_key.o obj/transaction.o obj/error.o obj/threaded_service.o obj/postgresql_blockchain.o obj/verify.o obj/big_number.o obj/clock.o obj/constants.o
	$(CXX) -o bin/tests/blockchain obj/blockchain.o obj/network.o  obj/dialect.o  obj/channel.o obj/serializer.o obj/logger.o obj/kernel.o obj/sha256.o obj/types.o obj/script.o obj/ripemd.o obj/postgresql_storage.o obj/block.o obj/elliptic_curve_key.o obj/transaction.o obj/error.o obj/threaded_service.o obj/postgresql_blockchain.o obj/verify.o obj/big_number.o obj/clock.o obj/constants.o $(LIBS)

blockchain: bin/tests/blockchain

