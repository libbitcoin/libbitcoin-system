objs: block transaction

block: ./src/block.cpp ./include/block.h ./include/transaction.h
	$(CXX) -I./include/ -c -o ./obj/block.o ./src/block.cpp -std=c++0x

transaction: ./src/transaction.cpp ./include/transaction.h
	$(CXX) -I./include/ -c -o ./obj/transaction.o ./src/transaction.cpp -std=c++0x

tests: block_hashing transaction_hashing

block_hashing: block
	$(CXX) -I./include/ -o ./bin/tests/block_hashing ./tests/block_hashing.cpp ./obj/block.o -std=c++0x -lssl

transaction_hashing: transaction
	$(CXX) -I./include/ -o ./bin/tests/transaction_hashing ./tests/transaction_hashing.cpp ./obj/transaction.o -std=c++0x -lssl
