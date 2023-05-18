SRC_PATH = src/
BUILD_PATH = build/
#OBJS specifies which files to compile as part of the project
OBJS = $(SRC_PATH)main.cpp $(SRC_PATH)Engine.cpp $(SRC_PATH)LTexture.cpp

#CC specifies which compiler we're using
CC = g++

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -O -w -std=c++17

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_ttf

#INCLUDE_PATHS specifies the additional include paths needed empty at the moment
INCLUDE_PATHS =

#LIBRARY_PATHS specifies the additional library paths needed empty at the moment
LIBRARY_PATHS =

ifeq ($(OS),Windows_NT)
	COMPILER_FLAGS += -Wl,-subsystem,windows
	INCLUDE_PATHS += -IC:\SDL2\SDL2-2.26.5\x86_64-w64-mingw32\include -IC:\SDL2\SDL2_ttf-2.20.2\x86_64-w64-mingw32\include
	LIBRARY_PATHS += -LC:\SDL2\SDL2-2.26.5\x86_64-w64-mingw32\lib -LC:\SDL2\SDL2_ttf-2.20.2\x86_64-w64-mingw32\lib
	LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf
endif

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	INCLUDE_PATHS += -I/opt/homebrew/include/
endif

#OBJ_NAME specifies the name of the exectuable
OBJ_NAME = $(BUILD_PATH)main

#This is the target that compiles the executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
