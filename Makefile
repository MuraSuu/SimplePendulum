CC = g++
CCFLAGS = -Wall -Wextra -Wpedantic -O1
GFLAGS = -Wall -Wextra -Wpedantic -Og -g
LFLAGS = -lgsl -lgslcblas -lm
SDLFLAGS = -lSDL2
OBJ1 = main.o

default: output

output: main.cc
	$(CC) main.cc $(CCFLAGS) $(LFLAGS) -o output.o
	
simulation: pendulum_sim.cc
	$(CC) pendulum_sim.cc SDL_Manager.cc $(GFLAGS) $(LFLAGS) $(SDLFLAGS) -o sim.o