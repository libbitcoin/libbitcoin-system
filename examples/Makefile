CXXFLAGS=$(shell pkg-config --cflags libbitcoin)
LIBS=$(shell pkg-config --libs libbitcoin)

default: all

txrad.o: txrad.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

txrad: txrad.o
	$(CXX) -o $@ $< $(LIBS)

connect.o: connect.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

connect: connect.o
	$(CXX) -o $@ $< $(LIBS)

accept.o: accept.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

accept: accept.o
	$(CXX) -o $@ $< $(LIBS)

satoshiwords.o: satoshiwords.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

satoshiwords: satoshiwords.o
	$(CXX) -o $@ $< $(LIBS)

initchain.o: initchain.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

initchain: initchain.o
	$(CXX) -o $@ $< $(LIBS)

determ.o: determ.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

determ: determ.o
	$(CXX) -o $@ $< $(LIBS)

priv.o: priv.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

priv: priv.o
	$(CXX) -o $@ $< $(LIBS)

fullnode.o: fullnode.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

fullnode: fullnode.o
	$(CXX) -o $@ $< $(LIBS)

display-last.o: display-last.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

display-last: display-last.o
	$(CXX) -o $@ $< $(LIBS)

proto.o: proto.cpp
	$(CXX) -o $@ -c $< $(CXXFLAGS)

proto: proto.o
	$(CXX) -o $@ $< $(LIBS)

all: priv determ initchain fullnode display-last satoshiwords connect accept proto txrad

