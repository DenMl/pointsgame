#! /bin/bash
g++ -o kkai.lnx -lrt -lboost_random -std=c++0x -fopenmp -Xlinker -zmuldefs -O3 'console.cpp' 'bot.cpp' 'field.cpp' 'minimax.cpp' 'position_estimate.cpp' 'uct.cpp'
