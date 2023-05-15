INCLUDE_PATHS = -IC:/msys64/mingw64/include/SDL2

LIBRARY_PATHS = -LC:/msys64/mingw64/lib

COMPILER_FLAGS = -std=c99 -Wall #-Wl,-subsystem,windows

LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2

build :
	gcc *.c -o sketch.exe $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS)

clean:
	del sketch.exe