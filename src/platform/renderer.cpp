/**
 * Description:  Renderer
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 15, 2022 
*/
#include "renderer.hpp"
#include "gl/gl.hpp"

using namespace Platform;

bool Platform::CreateOpenGLAPI( RendererAPI* api, OpenGLLoadProc loadProc ) {
    if( !OpenGLLoadFunctions( loadProc ) ) {
        return false;
    }

    api->Initialize         = OpenGLInitialize;
    api->ClearBuffer        = OpenGLClearBuffer;
    api->SetClearColor      = OpenGLSetClearColor;
    api->SetViewport        = OpenGLSetViewport;

    api->CreateShader       = OpenGLCreateShader;
    api->DeleteShaders      = OpenGLDeleteShaders;
    api->UseShader          = OpenGLUseShader;
    api->GetUniformID       = OpenGLGetUniformID;
    api->UniformFloat       = OpenGLUniformFloat;
    api->UniformUInt        = OpenGLUniformUInt;
    api->UniformInt         = OpenGLUniformInt;

    api->CreateTexture2D      = OpenGLCreateTexture2D;
    api->UseTexture2D         = OpenGLUseTexture2D;
    api->SetTexture2DFilter   = OpenGLSetTexture2DFilter;
    api->SetTexture2DWrapMode = OpenGLSetTexture2DWrapMode;

    return true;
}

const char* Platform::RendererBackendToString( RendererBackend backend ) {
    switch( backend ) {
        case RendererBackend::OPENGL: return "OpenGL Core 4.6";
        default: return "UNKNOWN BACKEND";
    }
}

const char* Platform::TextureMinFilterToString( TextureMinFilter minFilter ) {
    switch( minFilter ) {
        case TextureMinFilter::NEAREST:                return "NEAREST";
        case TextureMinFilter::LINEAR:                 return "LINEAR";
        case TextureMinFilter::LINEAR_MIPMAP_NEAREST:  return "LINEAR_MIPMAP_NEAREST";
        case TextureMinFilter::LINEAR_MIPMAP_LINEAR:   return "LINEAR_MIPMAP_LINEAR";
        case TextureMinFilter::NEAREST_MIPMAP_NEAREST: return "NEAREST_MIPMAP_NEAREST";
        case TextureMinFilter::NEAREST_MIPMAP_LINEAR:  return "NEAREST_MIPMAP_LINEAR";
        default: return "UNKNOWN";
    }
}

const char* Platform::TextureMagFilterToString( TextureMagFilter magFilter ) {
    switch( magFilter ) {
        case TextureMagFilter::NEAREST: return "NEAREST";
        case TextureMagFilter::LINEAR:  return "LINEAR";
        default: return "UNKNOWN";
    }
}

const char* Platform::TextureWrapModeToString( TextureWrapMode wrapMode ) {
    switch( wrapMode ) {
        case TextureWrapMode::CLAMP:         return "CLAMP";
        case TextureWrapMode::REPEAT:        return "REPEAT";
        case TextureWrapMode::MIRROR_REPEAT: return "MIRROR_REPEAT";
        case TextureWrapMode::MIRROR_CLAMP:  return "MIRROR_CLAMP";
        default: return "UNKNOWN";
    }
}

const char* Platform::TextureFormatToString( TextureFormat format ) {
    switch( format ) {
        case TextureFormat::R:    return "RED";
        case TextureFormat::RG:   return "RED_GREEN";
        case TextureFormat::RGB:  return "RED_GREEN_BLUE";
        case TextureFormat::RGBA: return "RED_GREEN_BLUE_ALPHA";
        default: return "UNKNOWN";
    }
}

const char* TypeToString( Type type ) {
    switch(type) {
        case Type::UNSIGNED_BYTE:  return "UNSIGNED_BYTE";
        case Type::BYTE:           return "BYTE";
        case Type::UNSIGNED_SHORT: return "UNSIGNED_SHORT";
        case Type::SHORT:          return "SHORT";
        case Type::UNSIGNED_INT:   return "UNSIGNED_INT";
        case Type::INT:            return "INT";
        case Type::FLOAT:          return "FLOAT";
        case Type::DOUBLE:         return "DOUBLE";
        default: return "UNKNOWN";
    }
}

const char* StructureToString( Structure structure ) {
    switch( structure ) {
        case Structure::SCALAR: return "SCALAR";
        case Structure::VEC2:   return "VECTOR2";
        case Structure::VEC3:   return "VECTOR3";
        case Structure::VEC4:   return "VECTOR4";
        case Structure::MAT3:   return "MATRIX3";
        case Structure::MAT4:   return "MATRIX4";
        default: return "UNKNOWN";
    }
}

const char* Texture2D::toString() {
    DEBUG_UNIMPLEMENTED("Texture2D::toString not yet implemented!");
    return "";
}
