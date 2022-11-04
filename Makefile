# Desired compiler and C++ version
CC = g++ -std=c++17
DD = gdb

# Change between DEBUG/RELEASE

# RELEASE BUILD
# CFLAGS    = $(RELEASE)
# LNKFLAGS  = --static -mwindows
# TARGETDIR = ./bin/release

# DEBUG BUILD
CFLAGS    = $(DEBUG)
LNKFLAGS  = --static
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

# precompiled header path
PCH = ./src/pch.hpp
WINPCH = ./src/platform/win64/winpch.hpp

# linker flags
LNK = -static-libstdc++ -static-libgcc -lmingw32 -lopengl32 -lgdi32 -lcomdlg32 -ld3d11

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

PCH_TARG = $(PCH).gch
WINPCH_TARG = $(WINPCH).gch

all: $(PCH_TARG) $(WINPCH_TARG) $(BINARY)

run: all copy
	$(BINARY)

debug:
	$(DD) $(BINARY)

-include $(DEPS)
$(BINARY): $(OBJ)
	$(CC) -o $@ $(LIB) $^ $(LNK) $(LNKFLAGS) -g

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

rm_pch:
	-@rm $(PCH_TARG) $(WINPCH_TARG)

$(PCH_TARG): $(PCH)
	$(CC) $(CFLAGS) $(PCH)

$(WINPCH_TARG): $(WINPCH)
	$(CC) $(CFLAGS) $(WINPCH)

copy:
	-@robocopy $(RES) $(TARGETDIR)/resources/ -e -NFL -NDL -NJH -NJS -nc -ns -np

rm_nul:
	-@rm nul

cleano:
	-@rm $(OBJ) $(DEPS)

clean: cleano rm_pch
	-@rm $(BINARY); rm -r $(TARGETDIR)/resources

.PHONY: run all clean cleano pch rm_pch