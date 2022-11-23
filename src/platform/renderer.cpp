/**
 * Description:  Renderer
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 15, 2022 
*/
#include "renderer.hpp"
#include "gl/gl.hpp"
#include "platform/io.hpp"

using namespace Platform;

bool Platform::CreateOpenGLAPI( RendererAPI* api, OpenGLLoadProc loadProc ) {
    if( !OpenGLLoadFunctions( loadProc ) ) {
        return false;
    }

    api->Initialize          = OpenGLInitialize;
    api->ClearBuffer         = OpenGLClearBuffer;
    api->SetClearColor       = OpenGLSetClearColor;
    api->SetViewport         = OpenGLSetViewport;
    api->SetPackAlignment    = OpenGLSetPackAlignment;
    api->SetUnPackAlignment  = OpenGLSetUnPackAlignment;
    api->SetBlendingEnable   = OpenGLSetBlendingEnable;
    api->IsBlendingEnabled   = OpenGLIsBlendingEnabled;
    api->SetBlendFunction    = OpenGLSetBlendFunction;
    api->SetBlendEquation    = OpenGLSetBlendEquation;
    api->DrawVertexArray     = OpenGLDrawVertexArray;
    api->SetWireframeEnabled = OpenGLSetWireframeEnabled;

    // NOTE(alicia): Shader

    api->CreateShader  = OpenGLCreateShader;
    api->DeleteShaders = OpenGLDeleteShaders;
    api->UseShader     = OpenGLUseShader;
    api->GetUniformID  = OpenGLGetUniformID;
    api->UniformFloat  = OpenGLUniformFloat;
    api->UniformUInt   = OpenGLUniformUInt;
    api->UniformInt    = OpenGLUniformInt;
    api->UniformVec2   = OpenGLUniformVec2;
    api->UniformVec3   = OpenGLUniformVec3;
    api->UniformVec4   = OpenGLUniformVec4;
    api->UniformMat3   = OpenGLUniformMat3;
    api->UniformMat4   = OpenGLUniformMat4;

    // NOTE(alicia): Texture2D

    api->CreateTexture2D      = OpenGLCreateTexture2D;
    api->DeleteTextures2D     = OpenGLDeleteTextures2D;
    api->UseTexture2D         = OpenGLUseTexture2D;
    api->SetTexture2DFilter   = OpenGLSetTexture2DFilter;
    api->SetTexture2DWrapMode = OpenGLSetTexture2DWrapMode;
    
    // NOTE(alicia): Uniform Buffer

    api->CreateUniformBuffer               = OpenGLCreateUniformBuffer;
    api->DeleteUniformBuffers              = OpenGLDeleteUniformBuffers;
    api->UniformBufferData                 = OpenGLUniformBufferData;
    api->UniformBufferSubData              = OpenGLUniformBufferSubData;
    api->UniformBufferSetBindingPoint      = OpenGLUniformBufferSetBindingPoint;
    api->UniformBufferSetBindingPointRange = OpenGLUniformBufferSetBindingPointRange;

    // NOTE(alicia): Vertex Array
    api->CreateVertexArray           = OpenGLCreateVertexArray;
    api->DeleteVertexArrays          = OpenGLDeleteVertexArrays;
    api->UseVertexArray              = OpenGLUseVertexArray;
    api->VertexArrayBindVertexBuffer = OpenGLVertexArrayBindVertexBuffer;
    api->VertexArrayBindIndexBuffer  = OpenGLVertexArrayBindIndexBuffer;

    // NOTE(alicia): Vertex Buffer
    api->CreateVertexBuffer  = OpenGLCreateVertexBuffer;
    api->UseVertexBuffer     = OpenGLUseVertexBuffer;
    api->DeleteVertexBuffers = OpenGLDeleteVertexBuffers;

    // NOTE(alicia): Index Buffer
    api->CreateIndexBuffer  = OpenGLCreateIndexBuffer;
    api->UseIndexBuffer     = OpenGLUseIndexBuffer;
    api->DeleteIndexBuffers = OpenGLDeleteIndexBuffers;

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

usize Platform::TextureFormatComponentCount( TextureFormat format ) {
    switch( format ) {
        case TextureFormat::R:    return 1;
        case TextureFormat::RG:   return 2;
        case TextureFormat::RGB:  return 3;
        case TextureFormat::RGBA: return 4;
        default: return 0;
    }
}

const char* Platform::DataTypeToString( DataType type ) {
    switch(type) {
        case DataType::UNSIGNED_BYTE:  return "UNSIGNED_BYTE";
        case DataType::BYTE:           return "BYTE";
        case DataType::UNSIGNED_SHORT: return "UNSIGNED_SHORT";
        case DataType::SHORT:          return "SHORT";
        case DataType::UNSIGNED_INT:   return "UNSIGNED_INT";
        case DataType::INT:            return "INT";
        case DataType::FLOAT:          return "FLOAT";
        case DataType::DOUBLE:         return "DOUBLE";
        default: return "UNKNOWN";
    }
}

const char* Platform::DataStructureToString( DataStructure structure ) {
    switch( structure ) {
        case DataStructure::SCALAR: return "SCALAR";
        case DataStructure::VEC2:   return "VECTOR2";
        case DataStructure::VEC3:   return "VECTOR3";
        case DataStructure::VEC4:   return "VECTOR4";
        case DataStructure::MAT3:   return "MATRIX3";
        case DataStructure::MAT4:   return "MATRIX4";
        default: return "UNKNOWN";
    }
}

const char* Platform::BlendEqToString( BlendEq eq ) {
    switch( eq ) {
        case BlendEq::ADD:     return "ADD";
        case BlendEq::SUB:     return "SUB";
        case BlendEq::REV_SUB: return "REV_SUB";
        case BlendEq::MIN:     return "MIN";
        case BlendEq::MAX:     return "MAX";
        default: return "UNKNOWN";
    }
}

const char* Platform::BlendFactorToString( BlendFactor blendFactor ) {
    switch( blendFactor ) {
        case BlendFactor::ZERO:                     return "ZERO";
        case BlendFactor::ONE:                      return "ONE";
        case BlendFactor::SRC_COLOR:                return "SRC_COLOR";
        case BlendFactor::ONE_MINUS_SRC_COLOR:      return "ONE_MINUS_SRC_COLOR";
        case BlendFactor::DST_COLOR:                return "DST_COLOR";
        case BlendFactor::ONE_MINUS_DST_COLOR:      return "ONE_MINUS_DST_COLOR";
        case BlendFactor::SRC_ALPHA:                return "SRC_ALPHA";
        case BlendFactor::ONE_MINUS_SRC_ALPHA:      return "ONE_MINUS_SRC_ALPHA";
        case BlendFactor::DST_ALPHA:                return "DST_ALPHA";
        case BlendFactor::ONE_MINUS_DST_ALPHA:      return "ONE_MINUS_DST_ALPHA";
        case BlendFactor::CONSTANT_COLOR:           return "CONSTANT_COLOR";
        case BlendFactor::ONE_MINUS_CONSTANT_COLOR: return "ONE_MINUS_CONSTANT_COLOR";
        case BlendFactor::CONSTANT_ALPHA:           return "CONSTANT_ALPHA";
        case BlendFactor::ONE_MINUS_CONSTANT_ALPHA: return "ONE_MINUS_CONSTANT_ALPHA";
        case BlendFactor::SRC_ALPHA_SATURATE:       return "SRC_ALPHA_SATURATE";
        case BlendFactor::SRC1_COLOR:               return "SRC1_COLOR";
        case BlendFactor::ONE_MINUS_SRC1_COLOR:     return "ONE_MINUS_SRC1_COLOR";
        case BlendFactor::SRC1_ALPHA:               return "SRC1_ALPHA";
        case BlendFactor::ONE_MINUS_SRC1_ALPHA:     return "ONE_MINUS_SRC1_ALPHA";
        default: return "UNKNOWN";
    }
}

usize Platform::DataTypeSize( DataType type ) {
    switch( type ) {
        case DataType::UNSIGNED_BYTE:
        case DataType::BYTE:           return 1;
        case DataType::UNSIGNED_SHORT:
        case DataType::SHORT:          return 2;
        case DataType::UNSIGNED_INT:
        case DataType::INT:
        case DataType::FLOAT:          return 4;
        case DataType::DOUBLE:         return 8;
        default: return 0;
    }
}

usize Platform::DataStructureCount( DataStructure structure ) {
    switch( structure ) {
        case DataStructure::SCALAR: return 1;
        case DataStructure::VEC2:   return 2;
        case DataStructure::VEC3:   return 3;
        case DataStructure::VEC4:   return 4;
        case DataStructure::MAT3:   return 9;
        case DataStructure::MAT4:   return 16;
        default: return 0;
    }
}

VertexBufferLayout Platform::CreateVertexBufferLayout( usize elementCount, VertexBufferElement* elements ) {
    VertexBufferLayout result = {};
    result.elementCount = elementCount;

    usize elementsSize = elementCount * sizeof( VertexBufferElement );
    result.elements = (VertexBufferElement*)Platform::Alloc( elementsSize );
    Platform::MemCopy( elementsSize, elements, result.elements );

    result.elementOffsets = (usize*)Platform::Alloc( elementCount * sizeof( usize ) );
    result.stride = 0;

    ucycles( elementCount ) {
        usize elementSize = DataTypeSize(result.elements[i].dataType) *
            DataStructureCount(result.elements[i].structure);
        result.elementOffsets[i] = result.stride;
        result.stride           += elementSize;
    }

    return result;
}
void Platform::FreeVertexBufferLayout( VertexBufferLayout* layout ) {
    if( layout->elementCount != 0 ) {
        if( layout->elements ) {
            Platform::Free( layout->elements );
        }
        if( layout->elementOffsets ) {
            Platform::Free( layout->elementOffsets );
        }
    }

    *layout = {};
}
