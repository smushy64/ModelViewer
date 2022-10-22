#pragma once

#include "alias.hpp"
#include <string>

namespace Platform
{
    // File from disk
    // must be freed using FreeFile after done using
    struct File {
        const char* filePath;
        usize size;
        void* contents;
    };

    // Load file from disk at given path
    File LoadFile( const char* filePath );
    // Free file after load
    void FreeFile( File file );

    // Text file from disk
    // because it uses an std::string to store the contents, no need to free after done using
    struct TextFile {
        const char* filePath;
        usize size;
        std::string contents;
    };

    // Load text file from disk at given path
    TextFile LoadTextFile( const char* filePath );
    
} // namespace Platform

