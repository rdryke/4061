all: mm.o test.c  mm.o
	g++ -o mm test.c mm.o
mm.o: mm.c mm.h
	g++ -c mm.c -o mm.o

clean:
	rm *.o, mm, mmTests.cpp, mmTests

runTests: mm.o makeTests compileTests
	$ ./mm_tests

makeTests:
	cxxtest/bin/cxxtestgen --error-printer --abort-on-fail --have-eh -o mmTests.cpp mmTests.h

compileTests:
	g++ -Wall -Icxxtest -o mm_tests mmTests.cpp mm.o  
