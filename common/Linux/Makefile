
 # additional commands, should be provided via the command line
ADD_CC :=
ADD_LD :=
SRCS :=
STEAMSDK_PATH :=
ARCH := 32

MOPTION := -m32
TARGET := App_Linux_32bits.$(shell uname -r)
ifeq ($(ARCH),64)
MOPTION := -m64
TARGET := App_Linux_64bits.$(shell uname -r)
endif

SOURCES := $(wildcard ../../source/opengl/*.cpp) $(wildcard ../../source/*.cpp) $(SRCS)
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
INCS := -I../../
LIBPATHS := -L/usr/lib -L/usr/lib/unity-settings-daemon-1.0
ifeq ($(ARCH),64)
INCS := $(INCS) -I/usr/include/x86_64-linux-gnu
LIBPATHS := $(LIBPATHS) -L/usr/lib/x86_64-linux-gnu
else
INCS := $(INCS) -I/usr/include/i386-linux-gnu
LIBPATHS := $(LIBPATHS) -L/usr/lib/i386-linux-gnu
endif
LIBS := $(LIBPATHS) -lm -lGL -lGLU -lGLEW -lglut -lopenal -lalut -lpthread -lvorbis -lvorbisfile -lz -lX11
OPTIONS := -Wall -Wno-reorder -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-function $(MOPTION)
DEFINES := -DUSES_LINUX

ifdef STEAMSDK_PATH
ifneq ($(STEAMSDK_PATH),"")
INCS := $(INCS) -I"$(STEAMSDK_PATH)/sdk/public"
LIBS := $(LIBS) -L"$(STEAMSDK_PATH)/sdk/public/steam/lib/linux$(ARCH)" -L"$(STEAMSDK_PATH)/sdk/redistributable_bin/linux$(ARCH)" -lsdkencryptedappticket -lsteam_api
DEFINES := $(DEFINES) -DUSES_STEAM_INTEGRATION
endif
endif

$(TARGET): $(OBJECTS)
	g++ -o $@ $^ $(LIBS) $(ADD_LD) $(OPTIONS)
	mv $(TARGET) ../WorkDir/

%.o: %.cpp
	g++ -c -o $@ $< $(DEFINES) $(INCS) $(ADD_CC) $(OPTIONS)

.PHONY: clean
clean:
	rm -rf $(OBJECTS)
