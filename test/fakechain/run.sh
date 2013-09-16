#!/bin/bash
mkdir -p blockchain/
rm -fr blockchain/*
sx initchain blockchain/
make
./fake 2>&1 | tee out

