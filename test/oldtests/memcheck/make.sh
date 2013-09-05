#!/bin/bash
g++ begin.cpp `pkg-config --cflags --libs libbitcoin`
valgrind --tool=memcheck --leak-check=yes ./a.out

