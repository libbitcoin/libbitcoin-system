#!/bin/bash
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

source ./bindings-env.sh

# Clean and make required directories.
rm -rf "bindings/target"
#rm -rf "bindings/target/generated-sources/src/main/cpp"
#rm -rf "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID"
#rm -rf "bindings/target/generated-sources/src/main/python"
#rm -rf "bindings/target/generated-sources/src/main/python3"

mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/chain"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/config"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/formats"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/log"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/machine"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/math"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/message"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/unicode"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/utility"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/wallet"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/usingtestexpected"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/usingtestfail"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/usingshorttestexpected"
#mkdir -p "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/usingshorttestfail"
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
#swig $SWIG_OPS -c++ -java -package org.libbitcoin.$SUB_ARTIFACT_ID -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID_error.cpp" "bindings/src/main/swig/$SUB_ARTIFACT_ID-error.i"

swig $SWIG_OPS -c++ -java -package org.libbitcoin.$SUB_ARTIFACT_ID -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID.cpp" "bindings/src/main/swig/$SUB_ARTIFACT_ID.i"
#PACKAGE=config && swig $SWIG_OPS -c++ -java -package org.libbitcoin.$SUB_ARTIFACT_ID -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID""_""$PACKAGE.cpp" "bindings/src/main/swig/$SUB_ARTIFACT_ID-$PACKAGE.i"
#PACKAGE=chain && swig $SWIG_OPS -c++ -java -package org.libbitcoin.$SUB_ARTIFACT_ID -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID""_""$PACKAGE.cpp" "bindings/src/main/swig/$SUB_ARTIFACT_ID-$PACKAGE.i"
#PACKAGE=formats && swig $SWIG_OPS -c++ -java -package org.libbitcoin.$SUB_ARTIFACT_ID -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID""_""$PACKAGE.cpp" "bindings/src/main/swig/$SUB_ARTIFACT_ID-$PACKAGE.i"
#PACKAGE=machine && swig $SWIG_OPS -c++ -java -package org.libbitcoin.$SUB_ARTIFACT_ID -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID""_""$PACKAGE.cpp" "bindings/src/main/swig/$SUB_ARTIFACT_ID-$PACKAGE.i"
#PACKAGE=math && swig $SWIG_OPS -c++ -java -package org.libbitcoin.$SUB_ARTIFACT_ID -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID""_""$PACKAGE.cpp" "bindings/src/main/swig/$SUB_ARTIFACT_ID-$PACKAGE.i"
#PACKAGE=message && swig $SWIG_OPS -c++ -java -package org.libbitcoin.$SUB_ARTIFACT_ID -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID""_""$PACKAGE.cpp" "bindings/src/main/swig/$SUB_ARTIFACT_ID-$PACKAGE.i"
PACKAGE=utility && swig $SWIG_OPS -c++ -java -package org.libbitcoin.$SUB_ARTIFACT_ID -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID""_""$PACKAGE.cpp" "bindings/src/main/swig/$SUB_ARTIFACT_ID-$PACKAGE.i"
##PACKAGE=log && swig $SWIG_OPS -c++ -java -package org.libbitcoin.$SUB_ARTIFACT_ID.$PACKAGE -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/$PACKAGE" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID""_""$PACKAGE.cpp" "bindings/src/main/swig/$SUB_ARTIFACT_ID-$PACKAGE.i"
##PACKAGE=unicode && swig $SWIG_OPS -c++ -java -package org.libbitcoin.$SUB_ARTIFACT_ID.$PACKAGE -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/$PACKAGE" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID""_""$PACKAGE.cpp" "bindings/src/main/swig/$SUB_ARTIFACT_ID-$PACKAGE.i"
#PACKAGE=wallet  && swig $SWIG_OPS -c++ -java -package org.libbitcoin.$SUB_ARTIFACT_ID -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID""_""$PACKAGE.cpp" "bindings/src/main/swig/$SUB_ARTIFACT_ID-$PACKAGE.i"

#swig $SWIG_OPS -c++ -java -outdir "bindings/java/proxy/org/libbitcoin/$SUB_ARTIFACT_ID" -o "bindings/java/wrap/$SUB_ARTIFACT_ID.cpp" "bindings/$SUB_ARTIFACT_ID.i"
#PACKAGE=chain swig $SWIG_OPS -c++ -java -outdir "bindings/java/proxy/org/libbitcoin/$SUB_ARTIFACT_ID_"$PACKAGE -o "bindings/java/wrap/$SUB_ARTIFACT_ID_"$PACKAGE".cpp" "bindings/$SUB_ARTIFACT_ID_"$PACKAGE".i"
#PACKAGE=usingtest && swig $SWIG_OPS -c++ -java -package org.libbitcoin.$SUB_ARTIFACT_ID.$PACKAGE -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/$PACKAGE" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID_$PACKAGE.cpp" "bindings/src/main/swig/$SUB_ARTIFACT_ID-$PACKAGE.i"
#PACKAGE=usingtest1 && swig $SWIG_OPS -c++ -java -package org.libbitcoin.$SUB_ARTIFACT_ID.$PACKAGE -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/$PACKAGE" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID_$PACKAGE.cpp" "bindings/src/main/swig/$SUB_ARTIFACT_ID-$PACKAGE.i"
#PACKAGE=usingshorttestexpected && swig $SWIG_OPS -c++ -java -package org.libbitcoin.$SUB_ARTIFACT_ID.$PACKAGE -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/$PACKAGE" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID_$PACKAGE.cpp" "bindings/src/main/swig/$SUB_ARTIFACT_ID-$PACKAGE.i"
#PACKAGE=usingshorttestfail && swig $SWIG_OPS -c++ -java -package org.libbitcoin.$SUB_ARTIFACT_ID.$PACKAGE -outdir "bindings/target/generated-sources/src/main/java/org/libbitcoin/$SUB_ARTIFACT_ID/$PACKAGE" -o "bindings/target/generated-sources/src/main/cpp/java-wrap/$SUB_ARTIFACT_ID_$PACKAGE.cpp" "bindings/src/main/swig/$SUB_ARTIFACT_ID-$PACKAGE.i"

#swig -c++ -python -outdir "bindings/python/proxy" -o "bindings/python/wrap/$SUB_ARTIFACT_ID.cpp" "bindings/$SUB_ARTIFACT_ID.swg"
