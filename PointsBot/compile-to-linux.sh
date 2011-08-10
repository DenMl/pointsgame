# Set WINDOWS to 0,
# SURROUND_CONDITIONS to 0,
# DEBUG to 0. 

g++ 'PointsConsole.cpp' 'BotEngine.cpp' 'Field.cpp' 'MinMaxEstimate.cpp' 'PointsConsole.cpp' 'PositionEstimate.cpp' 'UCTEstimate.cpp' 'Zobrist.cpp' -lrt -std=c++0x -fopenmp -Xlinker -zmuldefs -O3