# ~~~~ Compilers & Options ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
CXX         = clang++
CXXFLAGS    = -std=c++20 -O1 -Wall -ferror-limit=24
LDFLAGS     = 

# ~~~~ Rules ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
all : main

main : main.o
	$(CXX) $(LDFLAGS) $^ -o $@

%.o : %.cc $(wildcard *.hh) Makefile
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ~~~~ Commands ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ #
clean :
	rm -f *.o main
