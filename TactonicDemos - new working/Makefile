CC = g++
CFLAGS= -Wall -Wextra -Iinclude -L./../../Stable/TactonicLib/lib -lftd2xx -lglut -lGL -lGLU -lTactonic -lTactonicTouch -Wl,-rpath,

all: compile_visualizer build_visualizer clean

build_visualizer:
	$(CC) -o lib/TactonicVisualizer TactonicVisualizer.o DisplayViews.o $(CFLAGS)

compile_visualizer:
	$(CC) -c -o TactonicVisualizer.o src/TactonicVisualizer.cpp $(CFLAGS)
	$(CC) -c -o DisplayViews.o src/DisplayViews.cpp $(CFLAGS)

clean:
	rm TactonicVisualizer.o DisplayViews.o
	chmod a+rx lib/TactonicVisualizer
