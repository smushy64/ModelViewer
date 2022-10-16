#pragma once

#include "alias.hpp"

namespace Platform
{
    struct File {
        const char* filePath;
        usize size;
        void* contents;
    };

    File LoadFile( const char* filePath );
    void FreeFile( File file );
    
} // namespace Platform

