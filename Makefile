objs: block transaction

block:
	$(CXX) -I./include/ -c -o ./obj/block.o ./src/block.cpp -std=c++0x

transaction:
	$(CXX) -I./include/ -c -o ./obj/transaction.o ./src/transaction.cpp -std=c++0x
