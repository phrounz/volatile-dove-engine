
SOURCES := $(wildcard ../../source/opengl/*.cpp) $(wildcard ../../source/*.cpp)
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))

app: $(OBJECTS)
	g++ -o $@ $^ -I.

%.o: %.cpp
	g++ -c -o $@ $< -DUSES_LINUX

##-Wall
