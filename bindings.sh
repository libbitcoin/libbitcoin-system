#!/bin/sh
###############################################################################
#  Copyright (c) 2014-2015 libbitcoin developers (see COPYING).
#
#         GENERATED SOURCE CODE, DO NOT EDIT EXCEPT EXPERIMENTALLY
#
###############################################################################
# Script to build language bindings for libbitcoin c++ public interfaces.
# This script requires SWIG - "Simplified Wrapper and Interface Generator".
# SWIG is a simple tool available for most platforms at http://www.swig.org
# Add the path to swig.exe to the path of this process or your global path.

# Exit this script on the first error.
set -e

echo Generating bitcoin bindings...

# Do everything relative to this file location.
cd `dirname "$0"`

# Clean and make required directories.
rm -rf "bindings/target"
#rm -rf "bindings/target/generated-sources/src/main/cpp"
#rm -rf "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin"
#rm -rf "bindings/target/generated-sources/src/main/python"
#rm -rf "bindings/target/generated-sources/src/main/python3"

mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/chain"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/config"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/formats"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/log"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/machine"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/math"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/message"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/unicode"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/utility"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/wallet"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/usingtestexpected"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/usingtestfail"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/usingshorttestexpected"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/usingshorttestfail"
mkdir -p "bindings/target/generated-sources/src/main/cpp/java-wrap"
mkdir -p "bindings/target/generated-sources/src/main/python"
mkdir -p "bindings/target/generated-sources/src/main/cpp/python-wrap"
mkdir -p "bindings/target/generated-sources/src/main/python3"
mkdir -p "bindings/target/generated-sources/src/main/cpp/python3-wrap"

# Generate bindings.

SWIG_OPS="-D'static_assert(...)='"
SWIG_OPS="-I/usr/include -I./include -I/usr/include/x86_64-linux-gnu "$SWIG_OPS
SWIG_OPS="-I./bindings/src/main/cpp "$SWIG_OPS
#SWIG_OPS="-I/usr/share/swig3.0/std "$SWIG_OPS
#SWIG_OPS="-importall "$SWIG_OPS
#SWIG_OPS="-v -E "$SWIG_OPS
#SWIG_OPS="-debug-tmsearch "$SWIG_OPS
#swig $SWIG_OPS -c++ -java -package org.libbitcoin.bitcoin -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/bitcoin_error.cpp" "bindings/src/main/swig/bitcoin-error.i"

#swig $SWIG_OPS -c++ -java -package org.libbitcoin.bitcoin -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/bitcoin.cpp" "bindings/src/main/swig/bitcoin.i"
PACKAGE=config && swig $SWIG_OPS -c++ -java -package org.libbitcoin.bitcoin -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/bitcoin_$PACKAGE.cpp" "bindings/src/main/swig/bitcoin-$PACKAGE.i"
#PACKAGE=utility && swig $SWIG_OPS -c++ -java -package org.libbitcoin.bitcoin -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/bitcoin_$PACKAGE.cpp" "bindings/src/main/swig/bitcoin-$PACKAGE.i"
#PACKAGE=math && swig $SWIG_OPS -c++ -java -package org.libbitcoin.bitcoin -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/bitcoin_$PACKAGE.cpp" "bindings/src/main/swig/bitcoin-$PACKAGE.i"
#PACKAGE=formats && swig $SWIG_OPS -c++ -java -package org.libbitcoin.bitcoin -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/bitcoin_$PACKAGE.cpp" "bindings/src/main/swig/bitcoin-$PACKAGE.i"
#PACKAGE=chain && swig $SWIG_OPS -c++ -java -package org.libbitcoin.bitcoin -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/bitcoin_$PACKAGE.cpp" "bindings/src/main/swig/bitcoin-$PACKAGE.i"
##PACKAGE=log && swig $SWIG_OPS -c++ -java -package org.libbitcoin.bitcoin.$PACKAGE -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/$PACKAGE" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/bitcoin_$PACKAGE.cpp" "bindings/src/main/swig/bitcoin-$PACKAGE.i"
#PACKAGE=machine && swig $SWIG_OPS -c++ -java -package org.libbitcoin.bitcoin -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/bitcoin_$PACKAGE.cpp" "bindings/src/main/swig/bitcoin-$PACKAGE.i"
#PACKAGE=message && swig $SWIG_OPS -c++ -java -package org.libbitcoin.bitcoin -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/bitcoin_$PACKAGE.cpp" "bindings/src/main/swig/bitcoin-$PACKAGE.i"
##PACKAGE=unicode && swig $SWIG_OPS -c++ -java -package org.libbitcoin.bitcoin.$PACKAGE -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/$PACKAGE" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/bitcoin_$PACKAGE.cpp" "bindings/src/main/swig/bitcoin-$PACKAGE.i"
#PACKAGE=wallet  && swig $SWIG_OPS -c++ -java -package org.libbitcoin.bitcoin -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/bitcoin_$PACKAGE.cpp" "bindings/src/main/swig/bitcoin-$PACKAGE.i"

#swig $SWIG_OPS -c++ -java -outdir "bindings/java/proxy/org/libbitcoin/bitcoin" -o "bindings/java/wrap/bitcoin.cpp" "bindings/bitcoin.i"
#PACKAGE=chain swig $SWIG_OPS -c++ -java -outdir "bindings/java/proxy/org/libbitcoin/bitcoin_"$PACKAGE -o "bindings/java/wrap/bitcoin_"$PACKAGE".cpp" "bindings/bitcoin_"$PACKAGE".i"
#PACKAGE=usingtest && swig $SWIG_OPS -c++ -java -package org.libbitcoin.bitcoin.$PACKAGE -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/$PACKAGE" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/bitcoin_$PACKAGE.cpp" "bindings/src/main/swig/bitcoin-$PACKAGE.i"
#PACKAGE=usingtest1 && swig $SWIG_OPS -c++ -java -package org.libbitcoin.bitcoin.$PACKAGE -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/$PACKAGE" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/bitcoin_$PACKAGE.cpp" "bindings/src/main/swig/bitcoin-$PACKAGE.i"
#PACKAGE=usingshorttestexpected && swig $SWIG_OPS -c++ -java -package org.libbitcoin.bitcoin.$PACKAGE -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/$PACKAGE" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/bitcoin_$PACKAGE.cpp" "bindings/src/main/swig/bitcoin-$PACKAGE.i"
#PACKAGE=usingshorttestfail && swig $SWIG_OPS -c++ -java -package org.libbitcoin.bitcoin.$PACKAGE -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/bitcoin/$PACKAGE" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/bitcoin_$PACKAGE.cpp" "bindings/src/main/swig/bitcoin-$PACKAGE.i"

#swig -c++ -python -outdir "bindings/python/proxy" -o "bindings/python/wrap/bitcoin.cpp" "bindings/bitcoin.swg"
