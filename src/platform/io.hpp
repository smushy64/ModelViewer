#pragma once

#include "alias.hpp"

namespace Platform
{
    struct File {
        usize size;
        void* contents;
    };

    File LoadFile( const char* filePath );
    void FreeFile( File file );
    
} // namespace Platform

