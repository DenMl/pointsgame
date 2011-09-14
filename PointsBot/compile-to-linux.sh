# Set WINDOWS to 0,
# SURROUND_CONDITIONS to 0,
# DEBUG to 0. 

g++ 'PointsConsole.cpp' 'bot_engine.cpp' 'field.cpp' 'minimax.cpp' 'position_estimate.cpp' 'uct.cpp' -lrt -std=c++0x -fopenmp -Xlinker -zmuldefs -O3