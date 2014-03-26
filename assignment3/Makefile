all: mm.o test.c  mm.o
	g++ -o t test.c mm.o
	./t
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
