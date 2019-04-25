.PHONY: all clean bundle dist

# Define required raylib variables
PLATFORM ?= PLATFORM_DESKTOP
RAYLIB_PATH ?= deps/raylib
PROJECT_NAME ?= 2048
DESTINATION ?= build

# Define required application bundle variables
ORIGIN_ICON=resources/icon.png
ICON_DIR=resources/icon.iconset

# Define required distribution variables
TMP_DMG = build/osx/$(PROJECT_NAME).dmg
TMP_DIR = build/osx/tmp
OUT_DMG = dist/osx/$(PROJECT_NAME).dmg

RAYLIB_RELEASE_PATH = $(RAYLIB_PATH)/release/libs/osx

# Define default C compiler: gcc
# NOTE: define g++ compiler if using C++
CC = gcc

# Define default make program: make
MAKE = make

# Define compiler flags:
#  -O1                  defines optimization level
#  -g                   enable debugging
#  -s                   strip unnecessary data from build
#  -Wall                turns on most, but not all, compiler warnings
#  -std=c99             defines C language mode (standard C from 1999 revision)
#  -std=gnu99           defines C language mode (GNU C from 1999 revision)
#  -Wno-missing-braces  ignore invalid warning (GCC bug 53119)
#  -D_DEFAULT_SOURCE    use with -std=c99 on Linux and PLATFORM_WEB, required for timespec
CFLAGS += -O3 -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces

# Define include paths for required headers
# NOTE: Several external required libraries (stb and others)
INCLUDE_PATHS = -I. -I$(RAYLIB_PATH)/release/include -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external

# Define library paths containing required libs
LDFLAGS = -L. -L$(RAYLIB_RELEASE_PATH) -L$(RAYLIB_PATH)/src 

# Define any libraries required on linking
# if you want to link libraries (libname.so or libname.a), use the -lname
# Libraries for OSX 10.9 desktop compiling
# NOTE: Required packages: libopenal-dev libegl1-mesa-dev
LDLIBS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL $(RAYLIB_RELEASE_PATH)/libraylib.a

# Define all source files required
PROJECT_SOURCE_FILES ?= src/game.c \
                        src/shapes.c \
			src/utils.c \
                        src/screens/screen_gameplay.c \
                        src/screens/screen_gameover.c \
                        src/screens/screen_gamewin.c \
                        src/screens/screen_menu.c

# Define all object files from source files
OBJS = $(patsubst %.c, %.o, $(PROJECT_SOURCE_FILES))

MAKEFILE_PARAMS = $(PROJECT_NAME)

# Default target entry
all: clean
	$(MAKE) $(MAKEFILE_PARAMS)

# Project target defined by PROJECT_NAME
$(PROJECT_NAME): $(OBJS)
	$(CC) -o $(DESTINATION)/$(PROJECT_NAME)$(EXT) $(OBJS) $(CFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS) -D$(PLATFORM)

# Compile source files
# NOTE: This pattern will compile every module defined on $(OBJS)
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS) -D$(PLATFORM)

# Clean everything
clean:
	# find . -type f -perm +ugo+x -delete
	rm -f src/*.o
	rm -f src/screens/*.o
	rm -r build/$(PROJECT_NAME)
	@echo Cleaning done

bundle: all

	rm -rf build/osx

	mkdir build/osx
	mkdir build/osx/$(PROJECT_NAME).app
	mkdir build/osx/$(PROJECT_NAME).app/Contents
	mkdir build/osx/$(PROJECT_NAME).app/Contents/MacOS
	mkdir build/osx/$(PROJECT_NAME).app/Contents/Resources

	cp build/$(PROJECT_NAME) build/osx/$(PROJECT_NAME).app/Contents/MacOS
	cp pkg/osx/Info.plist build/osx/$(PROJECT_NAME).app/Contents
	cp -r resources/audio/ build/osx/$(PROJECT_NAME).app/Contents/Resources/audio/

	scripts/icon.sh build/osx/$(PROJECT_NAME).app/Contents/Resources

dist: bundle
	scripts/dist_osx.sh
