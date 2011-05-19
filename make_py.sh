set -x
test ! -d out && mkdir out
test ! -d out/py && mkdir out/py
PY_CFLAGS=`python-config --cflags`
PY_LIBS=`python-config --libs`
SWIG_FLAGS="-shared -export-dynamic"

swig2.0 -c++ -python -O -threads -outdir out/py/ -o out/libbitcoin_wrap.cxx -Iinclude include/bindings/libbitcoin.i

g++-4.5 bin/test.cpp out/libbitcoin_wrap.cxx src/net/*.cpp src/net/dialects/*.cpp -std=c++0x -lboost_system -lboost_thread -Wall -Wextra -pedantic -Iinclude/ $SWIG_FLAGS $PY_CFLAGS $PY_LIBS -o out/py/_libbitcoin.so
