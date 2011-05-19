set -x
g++ bin/test.cpp src/net/*.cpp src/net/dialects/*.cpp -std=c++0x -lboost_system -lboost_thread -Wall -Wextra -pedantic -Iinclude/ -o test
