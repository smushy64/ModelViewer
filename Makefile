# Desired compiler and C++ version
CC = g++ -std=c++17
DD = gdb

# Change between DEBUG/RELEASE
# Change target path as well
# comment out -mwindows for debug builds
CFLAGS    = $(DEBUG)
LNKFLAGS  = --static #-mwindows
TARGETDIR = ./bin/debug

# Executable name
EXE = ModelViewer.exe

# Source code paths
DIR = ./src ./src/platform ./src/platform/win64 ./src/core

# Resources path
RES = ./resources

# Mingw include path
MINGWINC = C:/msys64/mingw64/include

# defines
DEF = -D UNICODE -D WINDOWS 

# linker flags
LNK = -static-libstdc++ -static-libgcc -lmingw32 -lopengl32 -lgdi32

# DONOT EDIT BEYOND THIS POINT!!! ===============================================

DEBUG   = $(DFLAGS) $(foreach D, $(INC), -I$(D)) $(DEPFLAGS) 
RELEASE = $(RFLAGS) $(foreach D, $(INC), -I$(D)) $(DEPFLAGS)

BINARY = $(TARGETDIR)/$(EXE)

WARN     = -Wall -Wextra
DOPT     = -O0 -g
ROPT     = -O2
DFLAGS   = $(WARN) $(DEF) $(DOPT) -D DEBUG
RFLAGS   = $(DEF) $(ROPT)
DEPFLAGS = -MP -MD
INC      = ./src $(MINGWINC)

CPP      = $(foreach D, $(DIR), $(wildcard $(D)/*.cpp))
C        = $(foreach D, ./src, $(wildcard $(D)/*.c))
OBJ      = $(patsubst %.c,%.o, $(C)) $(patsubst %.cpp,%.o, $(CPP))
DEPS     = $(patsubst %.c,%.d,$(C)) $(patsubst %.cpp,%.d,$(CPP))

all: $(BINARY)

run: all copy rm_nul
	$(BINARY)

debug:
	$(DD) $(BINARY)

-include $(DEPS)
$(BINARY): $(OBJ)
	$(CC) -o $@ $(LIB) $^ $(LNK) $(LNKFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

copy:
	-@robocopy $(RES) $(TARGETDIR)/resources/ > nul

rm_nul:
	-@rm nul

cleano:
	-@rm $(OBJ) $(DEPS)

clean: cleano
	-@rm $(BINARY); rm -r $(TARGETDIR)/resources

.PHONY: run all clean cleano