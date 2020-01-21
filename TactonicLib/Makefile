CC = g++
CFLAGS= -Wall -Wextra -Iinclude -L./../../Stable/TactonicLib/lib -lftd2xx -Wl,-rpath,

all: compile_tactonic build_test clean

build_test:
	$(CC) -shared -o ../../Stable/TactonicLib/lib/libTactonic.so ../../Stable/TactonicLib/lib/Tactonic.o ../../Stable/TactonicLib/lib/TactonicUSB.o -lpthread $(CFLAGS)
	$(CC) -shared -o ../../Stable/TactonicLib/lib/libTactonicTouch.so ../../Stable/TactonicLib/lib/TactonicTouch.o ../../Stable/TactonicLib/lib/TactonicTouchDetector.o -lpthread $(CFLAGS)

compile_tactonic:
	$(CC) -c -o ../../Stable/TactonicLib/lib/TactonicUSB.o src/TactonicUSB.cpp $(CFLAGS)
	$(CC) -c -o ../../Stable/TactonicLib/lib/Tactonic.o src/Tactonic.cpp $(CFLAGS)
	$(CC) -c -o ../../Stable/TactonicLib/lib/TactonicTouchDetector.o src/TactonicTouchDetector.cpp $(CFLAGS)
	$(CC) -c -o ../../Stable/TactonicLib/lib/TactonicTouch.o src/TactonicTouch.cpp $(CFLAGS)

clean:
	rm ../../Stable/TactonicLib/lib/TactonicUSB.o ../../Stable/TactonicLib/lib/Tactonic.o ../../Stable/TactonicLib/lib/TactonicTouch.o ../../Stable/TactonicLib/lib/TactonicTouchDetector.o
	chmod a+r ../../Stable/TactonicLib/lib/libTactonic.so
	chmod a+r ../../Stable/TactonicLib/lib/libTactonicTouch.so

	
