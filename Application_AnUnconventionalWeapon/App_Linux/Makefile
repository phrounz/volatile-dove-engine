
SOURCES := $(wildcard ../../source/opengl/*.cpp) $(wildcard ../../source/*.cpp) $(wildcard ../App_VS2008GL/AnUnconventionalWeapon/*.cpp)
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
INCS := -I../../
LIBS := -L/usr/lib -L/usr/lib/i386-linux-gnu -L/usr/lib/unity-settings-daemon-1.0 -lm -lGL -lGLU -lGLEW -lglut -lopenal -lalut -lpthread -lvorbis -lvorbisfile -lz -lX11 -lXrandr
OPTIONS := -Wall -Wno-reorder -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function

app: $(OBJECTS)
	g++ -o $@ $^ $(LIBS) $(OPTIONS)
	mv app ../WorkDir/

%.o: %.cpp
	g++ -c -o $@ $< -DUSES_LINUX $(INCS) $(OPTIONS)

.PHONY: clean
clean:
	rm -rf $(OBJECTS)
