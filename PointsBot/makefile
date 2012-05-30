OBJS = console.o bot.o field.o minimax.o position_estimate.o uct.o
BINS = kkai
CXXFLAGS = -lrt -std=c++0x -fopenmp -O3 -D LINUX -D ENVIRONMENT_32

.PHONY = all clean

all: $(BINS)

kkai: $(OBJS)
	$(CXX) -o kkai $(CXXFLAGS) $(OBJS)

console.o: config.h basic_types.h field.h bot.h

bot.o: config.h basic_types.h bot.h time.h zobrist.h field.h minimax.h uct.h position_estimate.h

field.o: field.h

minimax.o: config.h minimax.h field.h trajectories.h

position_estimate.o: config.h basic_types.h player.h position_estimate.h

uct.o: config.h basic_types.h uct.h player.h field.h time.h

clean:
	rm -f $(BINS)
	rm -f $(OBJS)
	rm -f *~