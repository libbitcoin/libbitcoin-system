CFLAGS= -Wall -pedantic

objs: block transaction logger

block: ./src/block.cpp ./include/block.hpp ./include/transaction.hpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/block.o ./src/block.cpp -std=c++0x

logger: ./src/util/logger.cpp ./include/util/logger.hpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/logger.o ./src/util/logger.cpp -std=c++0x
	
transaction: ./src/transaction.cpp ./include/transaction.hpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/transaction.o ./src/transaction.cpp -std=c++0x
	
script: ./src/script.cpp ./include/script.hpp
	$(CXX) $(CFLAGS) -I./include/ -c -o ./obj/script.o ./src/script.cpp -std=c++0x

tests: block_hashing transaction_hashing script_parsing

block_hashing: block
	$(CXX) $(CFLAGS) -I./include/ -o ./bin/tests/block_hashing ./tests/block_hashing.cpp ./obj/block.o -std=c++0x -lssl

transaction_hashing: transaction logger
	$(CXX) $(CFLAGS) -I./include/ -o ./bin/tests/transaction_hashing ./tests/transaction_hashing.cpp ./obj/transaction.o ./obj/logger.o -std=c++0x -lssl
	
script_parsing: script
	$(CXX) $(CFLAGS) -I./include/ -o ./bin/tests/script_parsing ./tests/script_parsing.cpp ./obj/script.o -std=c++0x
