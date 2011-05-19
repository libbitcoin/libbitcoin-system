set -x
g++ bin/shatest.cpp src/sha256.cpp -std=c++0x -lssl -Wall -Wextra -pedantic -Iinclude/ -o shatest
