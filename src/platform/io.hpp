#pragma once

#include "pch.hpp"

namespace Platform
{
    // File from disk
    // must be freed using FreeFile after done using
    struct File {
        std::string filePath;
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
        std::string filePath;
        usize size;
        std::string contents;
    };
    // Convert File to TextFile, cleans up File resources
    TextFile FileToTextFile( File file );

    // Load text file from disk at given path
    TextFile LoadTextFile( const char* filePath );
    // Load file using the OS pop up window
    File LoadFilePopup();

    class Texture2D;
    // Load Texture2D from disk at given path
    Texture2D* LoadTexture( const char* filePath );
    // Load Texture2D using the OS pop up window
    Texture2D* LoadTexturePopup();
    
} // namespace Platform
