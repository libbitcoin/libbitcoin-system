CFLAGS= -Wall -pedantic

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
	$(CXX) $(CFLAGS) -I./include/ -o ./bin/tests/block_hashing ./tests/block_hashing.cpp ./obj/block.o ./obj/logger.o ./obj/sha256.o -std=c++0x -lssl -pedantic -Wall -Wextra

transaction_hashing: transaction logger sha256
	$(CXX) $(CFLAGS) -I./include/ -o ./bin/tests/transaction_hashing ./tests/transaction_hashing.cpp ./obj/transaction.o ./obj/logger.o ./obj/sha256.o -std=c++0x -lssl -pedantic -Wall -Wextra
	
script_parsing: script
	$(CXX) $(CFLAGS) -I./include/ -o ./bin/tests/script_parsing ./tests/script_parsing.cpp ./obj/script.o -std=c++0x -pedantic -Wall -Wextra

obj/connection_manager.o: ./src/net/connection_manager.cpp ./include/bitcoin/net/connection_manager.hpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/connection_manager.o ./src/net/connection_manager.cpp -std=c++0x -g

obj/dialect.o: ./src/net/dialect.cpp ./include/bitcoin/net/dialect.hpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/dialect.o ./src/net/dialect.cpp -std=c++0x -g

obj/channel.o: ./src/net/channel.cpp ./include/bitcoin/net/channel.hpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/channel.o ./src/net/channel.cpp -std=c++0x -g

obj/serializer.o: ./src/net/serializer.cpp ./include/bitcoin/net/serializer.hpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/serializer.o ./src/net/serializer.cpp -std=c++0x -g

obj/nettest.o: ./tests/net.cpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/nettest.o ./tests/net.cpp -std=c++0x -g

bin/tests/nettest: obj/connection_manager.o  obj/dialect.o  obj/channel.o  obj/serializer.o obj/logger.o obj/nettest.o
	$(CXX) $(CFLAGS) -Iinclude/ -o bin/tests/nettest ./obj/connection_manager.o ./obj/dialect.o ./obj/channel.o obj/serializer.o -pedantic -std=c++0x -lboost_system -lboost_thread obj/logger.o obj/nettest.o

net: bin/tests/nettest

