set -x
g++ tests/net.cpp src/net/*.cpp -std=c++0x -lboost_system -lboost_thread -Wall -Wextra -pedantic -Iinclude/ -o bin/tests/net  -g
