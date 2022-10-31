#include "core/image.hpp"

#include "io.hpp"
#include "renderer.hpp"
#include <cstdlib>

// Platform-independent IO functions

void Platform::FreeFile( File file ) {
    if(file.contents) {
        free( file.contents );
    }
    file.size = 0;
}

Platform::TextFile Platform::LoadTextFile( const char* filePath ) {
    File file = LoadFile( filePath );
    return FileToTextFile( file );
}

Platform::TextFile Platform::FileToTextFile( File file ) {
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

Platform::Texture2D* LoadTextureFromFile( Platform::File file ) {
    Core::Image image = Core::ReadImage( file.contents, file.size );
    if( !image.bitmap ) {
        LOG_ERROR("IO > Could not read texture image!");
        FreeFile( file );
        return nullptr;
    }

    Platform::Texture2D* result = Platform::Texture2D::New(
        image.dimensions,
        image.bitmap,
        Platform::ImageFormatToTextureFormat( image.format ),
        Platform::ImageFormatToTextureInternalFormat( image.format ),
        Platform::BufferDataType::UBYTE,
        AUTO_MIPMAP,
        false
    );

    Core::FreeImage( image );
    FreeFile( file );

    return result;
}

Platform::Texture2D* Platform::LoadTexture( const char* filePath ) {
    File file = LoadFile( filePath );
    if( !file.contents ) {
        LOG_ERROR("IO > Could not load texture file!");
        return nullptr;
    }
    return LoadTextureFromFile( file );
}

Platform::Texture2D* Platform::LoadTexturePopup() {
    File file = LoadFilePopup();
    if( !file.contents ) {
        return nullptr;
    }
    return LoadTextureFromFile( file );
}
