CXXFLAGS=$(shell pkg-config --cflags libbitcoin) -ggdb
LIBS=$(shell pkg-config --libs libbitcoin)

default: all

bootstrap.o: bootstrap.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

bootstrap: bootstrap.o
	$(CXX) -o $@ $< $(LIBS)

all: bootstrap
