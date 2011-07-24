CFLAGS= -std=c++0x -Wall -pedantic -Wextra -fstack-protector -g -Iinclude/ -Iusr/include/ -c
LIBS= usr/lib/libcppdb.a -lcrypto -lboost_thread -lboost_system -ldl -lpq

objs: block transaction logger

block: ./src/block.cpp
	$(CXX) $(CFLAGS) -o ./obj/block.o ./src/block.cpp

obj/ripemd.o: ./src/util/ripemd.cpp
	$(CXX) $(CFLAGS) -o ./obj/ripemd.o ./src/util/ripemd.cpp

sha256: ./src/util/sha256.cpp
	$(CXX) $(CFLAGS) -o ./obj/sha256.o ./src/util/sha256.cpp

obj/logger.o: ./src/util/logger.cpp ./include/bitcoin/util/logger.hpp
	$(CXX) $(CFLAGS) -o ./obj/logger.o ./src/util/logger.cpp

transaction: ./src/transaction.cpp ./include/bitcoin/transaction.hpp
	$(CXX) $(CFLAGS) -o ./obj/transaction.o ./src/transaction.cpp

script: ./src/script.cpp ./include/bitcoin/script.hpp
	$(CXX) $(CFLAGS) -o ./obj/script.o ./src/script.cpp

tests: block_hashing transaction_hashing script_parsing

block_hashing: block obj/logger.o sha256
	$(CXX) $(CFLAGS) ./bin/tests/block_hashing ./tests/block_hashing.cpp ./obj/block.o ./obj/logger.o ./obj/sha256.o

script_parsing: script
	$(CXX) $(CFLAGS) ./bin/tests/script_parsing ./tests/script_parsing.cpp ./obj/script.o

obj/block.o: ./src/block.cpp ./include/bitcoin/block.hpp
	$(CXX) $(CFLAGS) -o ./obj/block.o ./src/block.cpp

obj/network.o: ./src/net/network.cpp ./include/bitcoin/net/network.hpp
	$(CXX) $(CFLAGS) -o ./obj/network.o ./src/net/network.cpp

obj/dialect.o: ./src/net/dialect.cpp ./src/net/dialect.hpp
	$(CXX) $(CFLAGS) -o ./obj/dialect.o ./src/net/dialect.cpp

obj/channel.o: ./src/net/channel.cpp ./src/net/channel.hpp
	$(CXX) $(CFLAGS) -o ./obj/channel.o ./src/net/channel.cpp

obj/sha256.o: ./src/util/sha256.cpp ./include/bitcoin/util/sha256.hpp
	$(CXX) $(CFLAGS) -o ./obj/sha256.o ./src/util/sha256.cpp

obj/kernel.o: ./src/kernel.cpp ./include/bitcoin/kernel.hpp
	$(CXX) $(CFLAGS) -o ./obj/kernel.o ./src/kernel.cpp

obj/serializer.o: ./src/util/serializer.cpp ./include/bitcoin/util/serializer.hpp
	$(CXX) $(CFLAGS) -o ./obj/serializer.o ./src/util/serializer.cpp

obj/nettest.o: ./tests/net.cpp
	$(CXX) $(CFLAGS) -o ./obj/nettest.o ./tests/net.cpp

obj/postgresql_storage.o: ./src/storage/postgresql_storage.cpp ./include/bitcoin/storage/postgresql_storage.hpp
	$(CXX) $(CFLAGS) -o ./obj/postgresql_storage.o ./src/storage/postgresql_storage.cpp

obj/elliptic_curve_key.o: ./src/util/elliptic_curve_key.cpp ./include/bitcoin/util/elliptic_curve_key.hpp
	$(CXX) $(CFLAGS) -o ./obj/elliptic_curve_key.o ./src/util/elliptic_curve_key.cpp

obj/memory_storage.o: ./src/storage/memory_storage.cpp ./include/bitcoin/storage/memory_storage.hpp
	$(CXX) $(CFLAGS) -o ./obj/memory_storage.o ./src/storage/memory_storage.cpp

bin/tests/nettest: obj/network.o  obj/dialect.o  obj/channel.o obj/serializer.o obj/logger.o obj/nettest.o obj/kernel.o obj/memory_storage.o obj/sha256.o obj/types.o obj/block.o obj/script.o obj/ripemd.o obj/postgresql_storage.o obj/block.o obj/elliptic_curve_key.o obj/transaction.o
	$(CXX) -o bin/tests/nettest ./obj/network.o ./obj/dialect.o ./obj/channel.o obj/serializer.o obj/logger.o obj/nettest.o obj/kernel.o obj/memory_storage.o obj/sha256.o obj/types.o obj/script.o obj/ripemd.o obj/postgresql_storage.o obj/block.o obj/elliptic_curve_key.o obj/transaction.o $(LIBS)

net: bin/tests/nettest

obj/types.o: ./src/types.cpp ./include/bitcoin/types.hpp
	$(CXX) $(CFLAGS) -o ./obj/types.o ./src/types.cpp

obj/gengen.o: ./tests/gengen.cpp
	$(CXX) $(CFLAGS) -o ./obj/gengen.o ./tests/gengen.cpp

bin/tests/gengen: obj/gengen.o obj/logger.o obj/sha256.o  obj/types.o obj/serializer.o obj/types.o
	$(CXX) -o bin/tests/gengen obj/gengen.o obj/logger.o obj/sha256.o  obj/types.o obj/serializer.o $(LIBS)

gengen: bin/tests/gengen

obj/script.o: ./src/script.cpp ./include/bitcoin/script.hpp
	$(CXX) $(CFLAGS) -o ./obj/script.o ./src/script.cpp

obj/script-test.o: ./tests/script-test.cpp
	$(CXX) $(CFLAGS) -o ./obj/script-test.o ./tests/script-test.cpp

bin/tests/script-test: obj/script-test.o obj/script.o obj/logger.o obj/sha256.o obj/ripemd.o obj/types.o
	$(CXX) -o bin/tests/script-test obj/script-test.o obj/script.o obj/logger.o obj/sha256.o obj/ripemd.o obj/types.o $(LIBS)

script-test: bin/tests/script-test

obj/psql.o: ./tests/psql.cpp
	$(CXX) $(CFLAGS) -o ./obj/psql.o ./tests/psql.cpp

bin/tests/psql: obj/postgresql_storage.o obj/psql.o obj/logger.o obj/script.o obj/ripemd.o obj/block.o obj/serializer.o obj/sha256.o obj/types.o
	$(CXX) -o bin/tests/psql obj/psql.o obj/postgresql_storage.o obj/logger.o obj/script.o obj/ripemd.o obj/block.o obj/serializer.o obj/sha256.o obj/types.o $(LIBS)

psql: bin/tests/psql

obj/types-test.o: ./tests/types-test.cpp
	$(CXX) $(CFLAGS) -o ./obj/types-test.o ./tests/types-test.cpp

bin/tests/types-test: obj/types.o obj/types-test.o
	$(CXX) -o bin/tests/types-test obj/types.o obj/types-test.o $(LIBS)

types-test: bin/tests/types-test

obj/merkle.o: ./tests/merkle.cpp
	$(CXX) $(CFLAGS) -o ./obj/merkle.o ./tests/merkle.cpp

bin/tests/merkle: obj/merkle.o
	$(CXX) -o bin/tests/merkle obj/merkle.o $(LIBS)

merkle: bin/tests/merkle

obj/transaction.o: src/transaction.cpp
	$(CXX) $(CFLAGS) -o obj/transaction.o src/transaction.cpp

obj/tx-hash.o: tests/tx-hash.cpp
	$(CXX) $(CFLAGS) -o obj/tx-hash.o tests/tx-hash.cpp

bin/tests/tx-hash: obj/tx-hash.o obj/transaction.o obj/sha256.o obj/script.o obj/serializer.o obj/logger.o obj/types.o obj/ripemd.o
	$(CXX) -o bin/tests/tx-hash obj/tx-hash.o obj/transaction.o obj/sha256.o obj/script.o obj/serializer.o obj/logger.o obj/types.o obj/ripemd.o $(LIBS)

tx-hash: bin/tests/tx-hash

