#!/bin/bash
g++ -O0 -g -std=c++11 -Wall test_$1.cpp -o test_$1 &&
valgrind  --leak-check=full ./test_$1
