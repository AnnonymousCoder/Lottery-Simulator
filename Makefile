
COMPILER_DIRECTORY = ./raylib/raylib/w64devkit\bin
CC   ?= $(COMPILER_DIRECTORY)/gcc.exe
MAKE ?= $(COMPILER_DIRECTORY)/mingw32-make.exe

WORKING_DIR  ?= ./Simulator
INCLUDE_PATH = $(WORKING_DIR)/inc
LIBRARY_PATH = $(WORKING_DIR)/lib
SRC_PATH 	 = $(WORKING_DIR)/src
OBJ_PATH 	 = $(WORKING_DIR)/obj
BIN_PATH 	 = $(WORKING_DIR)/bin

SRC  := $(wildcard $(SRC_PATH)/*.c)
OBJS = $(SRC:$(SRC_PATH)/%.c=$(OBJ_PATH)/%.o)

# debug:
# 	@echo "Hello World!"

PROJECT_NAME ?= default
EXT 		 ?= .exe

PROG_NAME = $(BIN_PATH)\$(PROJECT_NAME)$(EXT)

CFLAGS = -Wall -Wextra -Wpedantic -std=c99 -Wno-missing-braces 
LDLIBS = -lraylib -lbox2d -lopengl32 -lgdi32 -lwinmm

.PHONY: all clean


all:
	$(MAKE) $(PROG_NAME)
	
run:
ifeq ($(OS),Windows_NT)
	.\$(subst /,\,$(PROG_NAME))	
else
	.\$(PROG_NAME)
endif

$(PROG_NAME): $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS) -I $(INCLUDE_PATH) -L $(LIBRARY_PATH) $(LDLIBS)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC) -c $< -o $@ $(CFLAGS) -I $(INCLUDE_PATH)


clean:
ifeq ($(OS),Windows_NT)
	cmd.exe /C del "$(subst /,\,$(OBJ_PATH))\*.o"
	cmd.exe /C del "$(subst /,\,$(BIN_PATH))\*.exe"
else
	rm "$(OBJ_PATH)"/*.o
	rm "$(BIN_PATH)"/*.exe 
endif