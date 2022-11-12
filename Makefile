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
DIR = ./src ./src/platform ./src/platform/win64 ./src/platform/dx11 ./src/platform/gl ./src/core

# Resources path
RES = ./resources

# Mingw include path
MINGWINC = C:/msys64/mingw64/include

# defines
DEF = -D UNICODE -D WINDOWS 

# precompiled header path
PCH = ./src/pch.hpp
WINPCH = ./src/platform/win64/winpch.hpp

# windows-specific linker flags, not necessary for non-windows builds
WINLNK = -lgdi32 -lcomdlg32 -ld3d11 -ld3dcompiler
# linker flags
LNK = -static-libstdc++ -static-libgcc -lmingw32 -lopengl32 $(WINLNK)

# DONOT EDIT BEYOND THIS POINT!!! ===============================================

# Desired compiler and C++ version
CC = g++ -std=c++17
DD = gdb

DEBUG   = $(DFLAGS) $(foreach D, $(INC), -I$(D)) $(DEPFLAGS) 
RELEASE = $(RFLAGS) $(foreach D, $(INC), -I$(D)) $(DEPFLAGS)

BINARY = $(TARGETDIR)/$(EXE)

WARN     = -Wall -Wextra
DFLAGS   = $(WARN) $(DEF) -O0 -g -D DEBUG
RFLAGS   = $(DEF) -O2
DEPFLAGS = -MP -MD
INC      = ./src $(MINGWINC)

CPP      = $(foreach D, $(DIR), $(wildcard $(D)/*.cpp))
C        = $(foreach D, ./src, $(wildcard $(D)/*.c))
OBJ      = $(patsubst %.c,%.o, $(C)) $(patsubst %.cpp,%.o, $(CPP))
DEPS     = $(patsubst %.c,%.d,$(C)) $(patsubst %.cpp,%.d,$(CPP))

PCH_TARG = $(PCH).gch
WINPCH_TARG = $(WINPCH).gch

all: $(PCH_TARG) $(WINPCH_TARG) $(BINARY)

run: copy all
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