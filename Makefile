CFLAGS= -Wall -pedantic -Wextra

objs: block transaction logger

block: ./src/block.cpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/block.o ./src/block.cpp -std=c++0x

sha256: ./src/util/sha256.cpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/sha256.o ./src/util/sha256.cpp -std=c++0x
	
obj/logger.o: ./src/util/logger.cpp ./include/bitcoin/util/logger.hpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/logger.o ./src/util/logger.cpp -std=c++0x
	
transaction: ./src/transaction.cpp ./include/bitcoin/transaction.hpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/transaction.o ./src/transaction.cpp -std=c++0x
	
script: ./src/script.cpp ./include/bitcoin/script.hpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/script.o ./src/script.cpp -std=c++0x

tests: block_hashing transaction_hashing script_parsing

block_hashing: block obj/logger.o sha256
	$(CXX) $(CFLAGS) -I./include/ -o ./bin/tests/block_hashing ./tests/block_hashing.cpp ./obj/block.o ./obj/logger.o ./obj/sha256.o -std=c++0x -lssl -pedantic -Wall -Wextra -lboost_system

transaction_hashing: transaction obj/logger.o sha256
	$(CXX) $(CFLAGS) -I./include/ -o ./bin/tests/transaction_hashing ./tests/transaction_hashing.cpp ./obj/transaction.o ./obj/logger.o ./obj/sha256.o -std=c++0x -lssl -pedantic -Wall -Wextra -lboost_system
	
script_parsing: script
	$(CXX) $(CFLAGS) -I./include/ -o ./bin/tests/script_parsing ./tests/script_parsing.cpp ./obj/script.o -std=c++0x -pedantic -Wall -Wextra

obj/network.o: ./src/net/network.cpp ./include/bitcoin/net/network.hpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/network.o ./src/net/network.cpp -std=c++0x -g

obj/dialect.o: ./src/net/dialect.cpp ./src/net/dialect.hpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/dialect.o ./src/net/dialect.cpp -std=c++0x -g

obj/channel.o: ./src/net/channel.cpp ./src/net/channel.hpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/channel.o ./src/net/channel.cpp -std=c++0x -g

obj/sha256.o: ./src/util/sha256.cpp ./include/bitcoin/util/sha256.hpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/sha256.o ./src/util/sha256.cpp -std=c++0x -g

obj/kernel.o: ./src/kernel.cpp ./include/bitcoin/kernel.hpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/kernel.o ./src/kernel.cpp -std=c++0x -g

obj/serializer.o: ./src/net/serializer.cpp ./src/net/serializer.hpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/serializer.o ./src/net/serializer.cpp -std=c++0x -g

obj/nettest.o: ./tests/net.cpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/nettest.o ./tests/net.cpp -std=c++0x -g

obj/storage.o: ./src/storage/storage.cpp ./include/bitcoin/storage/storage.hpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/storage.o ./src/storage/storage.cpp -std=c++0x -g

bin/tests/nettest: obj/network.o  obj/dialect.o  obj/channel.o obj/serializer.o obj/logger.o obj/nettest.o obj/kernel.o obj/storage.o obj/sha256.o
	$(CXX) $(CFLAGS) -Iinclude/ -o bin/tests/nettest ./obj/network.o ./obj/dialect.o ./obj/channel.o obj/serializer.o -pedantic -std=c++0x -lboost_system -lboost_thread obj/logger.o obj/nettest.o obj/kernel.o obj/storage.o obj/sha256.o -lssl

net: bin/tests/nettest

obj/gengen.o: ./tests/gengen.cpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/gengen.o ./tests/gengen.cpp -std=c++0x -g

bin/tests/gengen: obj/gengen.o obj/logger.o obj/sha256.o
	$(CXX) $(CFLAGS) -Iinclude/ -o bin/tests/gengen -pedantic -std=c++0x -lboost_system -lboost_thread obj/gengen.o -lssl obj/logger.o obj/sha256.o

gengen: bin/tests/gengen

