#include "io.hpp"
#include <cstdlib>

// Platform-independent IO functions

void Platform::FreeFile( File file ) {
    if(file.contents) {
        free( file.contents );
    }
}

Platform::TextFile Platform::LoadTextFile( const char* filePath ) {
    File file = LoadFile( filePath );
    TextFile result = {};
    if( !file.contents ) {
        return result;
    }

    result.filePath = file.filePath;
    result.size     = file.size;
    result.contents = std::string( (char*)file.contents, file.size );

    FreeFile( file );

    return result;
}
