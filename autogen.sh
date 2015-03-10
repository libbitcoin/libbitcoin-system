#!/bin/bash

# testing for OS X builds
mkdir -p build-aux
cp config.rpath build-aux/config.rpath

autoreconf -i

