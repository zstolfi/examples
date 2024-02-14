INCLUDE_PATHS = -I C:/msys64/mingw64/include/SDL2

LIBRARY_PATHS = -L C:/msys64/mingw64/lib

# evil dependencies
DEPENDENCIES = -static-libgcc# -static-libstdc++

COMPILER_FLAGS = -std=c++20 -Wall -O4# -Wl,-subsystem,windows

LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2

#Compile!
all : $(OBJS)
	g++ main.cc $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o Midi_vis