INCLUDE_PATHS = 

LIBRARY_PATHS = -LC:/msys64/mingw64/lib

COMPILER_FLAGS = -std=c++20 -Wall #-Wl,-subsystem,windows

LINKER_FLAGS = -lmingw32

build :
	g++ main.cc -o makegif.exe $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS)

clean:
	del makegif.exe