objs: sha256 block

sha256:
	$(CXX) -I./include/ -c -o ./obj/sha256.o ./src/sha256.cpp

block:
	$(CXX) -I./include/ -c -o ./obj/block.o ./src/block.cpp
