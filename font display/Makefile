INCLUDE_PATHS = -IC:/msys64/mingw64/include/SDL2

LIBRARY_PATHS = -LC:/msys64/mingw64/lib
# evil dependencies
DEPENDENCIES = -static-libgcc -static-libstdc++

COMPILER_FLAGS = -std=c++20 -Wall -O0# -Wl,-subsystem,windows

LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2

all :
	g++ main.cc -o fontDisplay $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS)