#!/bin/bash
TESTBINARY=test_libbitcoin
CXX=g++
CXXFLAGS=$(pkg-config --cflags libbitcoin)
LDFLAGS="$(pkg-config --libs libbitcoin) -lboost_unit_test_framework"
mkdir -p obj/
for i in $(ls *.cpp); do
    OBJ_FILE=obj/${i%.cpp}.o
    if test $i -nt $OBJ_FILE; then
        echo "Compiling $i..."
        $CXX $CXXFLAGS -c $i -o $OBJ_FILE
    fi
done
echo "Linking..."
$CXX -o $TESTBINARY obj/*.o $LDFLAGS
./$TESTBINARY

