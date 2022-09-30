#pragma once
#include "defines.hpp"

namespace Platform
{

namespace IO
{

struct File {
    usize size;
    void* contents;
};

File LoadFile( const char* filePath );
void FreeFile( File file );

} // namespace IO

} // namespace Platform

