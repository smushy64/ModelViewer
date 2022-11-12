# Desired compiler and C++ version
CC = g++ -std=c++20

# Change between DEBUG/RELEASE

# RELEASE BUILD
# CFLAGS    = $(RELEASE)
# LNKFLAGS  = --static -mwindows
# TARGETDIR = ./build/release

# DEBUG BUILD
CFLAGS    = $(DEBUG)
LNKFLAGS  = --static
TARGETDIR = ./build/debug

# Executable name
EXE = ModelViewer.exe

# Source code paths
SRC = $(dir $(wildcard ./src/*/.))

# defines
DEF = -D UNICODE -D WINDOWS

# pre-compiled header
PCH = ./src/pch.hpp

# linker flags
LNK = -static-libstdc++ -static-libgcc -lmingw32

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
INC      = ./src

CPP      = $(foreach D, $(SRC), $(wildcard $(D)*.cpp))
C        = $(foreach D, $(SRC), $(wildcard $(D)*.c))
OBJ      = $(patsubst %.c,%.o, $(C)) $(patsubst %.cpp,%.o, $(CPP))
DEPS     = $(patsubst %.c,%.d,$(C)) $(patsubst %.cpp,%.d,$(CPP))

PCH_TARG = $(PCH).gch

all: $(PCH_TARG) $(BINARY)

-include $(DEPS)
$(BINARY): $(OBJ)
	$(CC) -o $@ $(LIB) $^ $(LNK) $(LNKFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

$(PCH_TARG): $(PCH)
	$(CC) $(CFLAGS) $(PCH)

clean:
	-@rm $(BINARY); -@rm $(OBJ) $(DEPS); -@rm $(PCH_TARG)

.PHONY: all clean