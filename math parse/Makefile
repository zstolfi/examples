INCLUDE_PATHS = -IC:/msys64/mingw64/include

LIBRARY_PATHS = -LC:/msys64/mingw64/lib

# evil dependencies
DEPENDENCIES = -static-libgcc -static-libstdc++

COMPILER_FLAGS = -std=c++23 -Wall -O3 #-fmodules-ts

LINKER_FLAGS = -lgmp -lgmpxx

#Compile!
all :
	g++ main.cc -o parseVal $(LIBRARY_PATHS) $(DEPENDENCIES) $(COMPILER_FLAGS) $(LINKER_FLAGS)