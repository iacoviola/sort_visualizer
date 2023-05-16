SRC_PATH = src/
BUILD_PATH = build/
#OBJS specifies which files to compile as part of the project
OBJS = $(SRC_PATH)main.cpp $(SRC_PATH)Engine.cpp

#CC specifies which compiler we're using
CC = g++

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -O -w -std=c++17

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image

#INCLUDE_PATHS specifies the additional include paths we'll need (only on Mac) empty at the moment
INCLUDE_PATHS =

UNAME := $(shell uname)
ifeq ($(UNAME), Darwin)
	INCLUDE_PATHS += -I/opt/homebrew/include/
endif

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = $(BUILD_PATH)main

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(INCLUDE_PATHS) -o $(OBJ_NAME)
