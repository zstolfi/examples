CC        = clang
CXX       = clang++
CXXFLAGS  = -std=c++20 -Wall -O1 -g -ferror-limit=30 -fsanitize=undefined
LDFLAGS   = -lSDL2 -lSDL2_image -fsanitize=undefined
TARGET    = fontDisplay

SOURCES   = $(wildcard *.cc)
HEADERS   = $(wildcard *.hh)
OBJECTS   = $(SOURCES:.cc=.o)

all : $(SOURCES) $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@

%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJECTS) : $(HEADERS)

clean :
	rm -f *.o
