#include "gl.hpp"
#include "glad/glad.h"
#include "pch.hpp"

using namespace Platform;

void OpenGLDebugMessageCallback(
    GLenum,        // source
    GLenum,        // type
    GLuint,        // id
    GLenum,        // severity
    GLsizei,       // length
    const GLchar*, // message
    const void*    // userParam
);

#ifdef DEBUG

const usize IGNORE_IDS[] = { 131185 };
const usize IGNORE_IDS_COUNT = 1;

const char* DebugSourceToString( GLenum source ) {
    switch(source) {
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "3rd Party";
        case GL_DEBUG_SOURCE_APPLICATION: return "Application";
        default: return "Other";
    }
}

const char* DebugTypeToString( GLenum type ) {
    switch( type ) {
        case GL_DEBUG_TYPE_ERROR: return "Error";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behaviour";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined Behaviour";
        case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
        case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
        case GL_DEBUG_TYPE_MARKER: return "Marker";
        case GL_DEBUG_TYPE_PUSH_GROUP: return "Push";
        case GL_DEBUG_TYPE_POP_GROUP: return "Pop";
        default: return "Other";
    }
}

void OpenGLDebugMessageCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei, // message length
    const GLchar *message,
    const void* // userParam
) {
    for( usize i = 0; i < IGNORE_IDS_COUNT; i++ ) {
        if( id == IGNORE_IDS[i] ) {
            return;
        }
    }

    switch(severity) {
        case GL_DEBUG_SEVERITY_HIGH: {
            LOG_ERROR("OpenGL [%i] > [SRC %s] [TYPE %s] %s",
                id,
                DebugSourceToString( source ),
                DebugTypeToString( type ),
                message
            );
        } break;
        case GL_DEBUG_SEVERITY_MEDIUM:
        case GL_DEBUG_SEVERITY_LOW: {
            LOG_WARN("OpenGL [%i] > [SRC %s] [TYPE %s] %s",
                id,
                DebugSourceToString( source ),
                DebugTypeToString( type ),
                message
            );
        } break;
        default: case GL_DEBUG_SEVERITY_NOTIFICATION: {
            LOG_INFO("OpenGL [%i] > [SRC %s] [TYPE %s] %s",
                id,
                DebugSourceToString( source ),
                DebugTypeToString( type ),
                message
            );
        } break;
    }
}

#endif

bool RendererAPIOpenGL::Initialize( void* initData )  {
    OpenGLInitData* openGLInitData = (OpenGLInitData*)initData;
    if(!gladLoadGLLoader( (GLADloadproc)openGLInitData->loaderFn )) {
        LOG_ERROR("OpenGL > Failed to load OpenGL functions!");
        return false;
    }

#ifdef DEBUG
    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( OpenGLDebugMessageCallback, nullptr );
#endif
    glEnable( GL_DEPTH_TEST );

    m_openGLSwapBufferFn = openGLInitData->swapBufferFn;

    return true;
}
void RendererAPIOpenGL::ClearBuffer() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}
void RendererAPIOpenGL::SwapBuffers() {
    DEBUG_ASSERT_LOG( m_openGLSwapBufferFn != nullptr, "Swap Buffer Function is null!" );
    m_openGLSwapBufferFn();
}
void RendererAPIOpenGL::SetClearColor( const glm::vec4& color )  {
    m_clearColor = color;
    glClearColor( color.r, color.g, color.b, color.a );
}
void RendererAPIOpenGL::SetViewport( const glm::vec2& viewport ) {
    m_viewport = viewport;
    glViewport( 0, 0, (GLsizei)viewport.x, (GLsizei)viewport.y );
}
void RendererAPIOpenGL::EnableWireframe() {
    m_wireframe = true;
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
}
void RendererAPIOpenGL::DisableWireframe() {
    m_wireframe = false;
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}
void RendererAPIOpenGL::EnableBlending() {
    m_blending = true;
    glEnable( GL_BLEND );
}
void RendererAPIOpenGL::DisableBlending() {
    m_blending = false;
    glDisable( GL_BLEND );
}
void RendererAPIOpenGL::BlendEquation( BlendEq Color, BlendEq Alpha ) {
    m_blendEqRGB   = Color;
    m_blendEqAlpha = Alpha;
    glBlendEquationSeparate(
        RendererAPIOpenGL::BlendEqToGLenum( Color ),
        RendererAPIOpenGL::BlendEqToGLenum( Alpha )
    );
}
void RendererAPIOpenGL::SetConstantBlendColor( const glm::vec4& blendColor ) {
    m_blendColor = blendColor;
    glBlendColor( blendColor.r, blendColor.g, blendColor.b, blendColor.a );
}
void RendererAPIOpenGL::BlendFunction(
    BlendFactor srcColor,
    BlendFactor dstColor,
    BlendFactor srcAlpha,
    BlendFactor dstAlpha
) {
    m_srcRGB   = srcColor;
    m_srcAlpha = srcAlpha;
    m_dstRGB   = dstColor;
    m_dstAlpha = dstAlpha;
    glBlendFuncSeparate(
        BlendFactorToGLenum( srcColor ),
        BlendFactorToGLenum( dstColor ),
        BlendFactorToGLenum( srcAlpha ),
        BlendFactorToGLenum( dstAlpha )
    );
}
void RendererAPIOpenGL::SetPackAlignment( PixelAlignment alignment ) {
    m_packAlignment = alignment;
    glPixelStorei( GL_PACK_ALIGNMENT, (GLint)alignment );
}
void RendererAPIOpenGL::SetUnpackAlignment( PixelAlignment alignment ) {
    m_unpackAlignment = alignment;
    glPixelStorei( GL_UNPACK_ALIGNMENT, (GLint)alignment );
}
void RendererAPIOpenGL::SetActiveTexture( u32 activeTexture ) const {
    glActiveTexture( GL_TEXTURE0 + activeTexture );
}
void RendererAPIOpenGL::DrawVertexArray( const VertexArray* va ) {
    if( va->HasIndexBuffer() ) {
        glDrawElements(
            GL_TRIANGLES,
            va->GetIndexBuffer()->Count(),
            BufferDataTypeToGLenum( va->GetIndexBuffer()->DataType() ),
            nullptr
        );
    } else {
        auto& vb = va->GetVertexBuffers()[0];
        glDrawArrays(
            GL_TRIANGLES,
            0,
            vb->Count()
        );
    }
}
RendererAPIOpenGL::~RendererAPIOpenGL() {
    
}

u32 RendererAPIOpenGL::BlendEqToGLenum( BlendEq glenum ) {
    switch( glenum ) {
        case BlendEq::SUB: return GL_FUNC_SUBTRACT;
        case BlendEq::REV_SUB: return GL_FUNC_REVERSE_SUBTRACT;
        case BlendEq::MIN: return GL_MIN;
        case BlendEq::MAX: return GL_MAX;
        default: return GL_FUNC_ADD;
    }
}
u32 RendererAPIOpenGL::BlendFactorToGLenum( BlendFactor glenum ) {
    switch( glenum ) {
        case BlendFactor::ZERO: return GL_ZERO;
        case BlendFactor::SRC_COLOR: return GL_SRC_COLOR;
        case BlendFactor::ONE_MINUS_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DST_COLOR: return GL_DST_COLOR;
        case BlendFactor::ONE_MINUS_DST_COLOR: return GL_ONE_MINUS_DST_COLOR;
        case BlendFactor::SRC_ALPHA: return GL_SRC_ALPHA;
        case BlendFactor::ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DST_ALPHA: return GL_DST_ALPHA;
        case BlendFactor::ONE_MINUS_DST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
        case BlendFactor::CONSTANT_COLOR: return GL_CONSTANT_COLOR;
        case BlendFactor::ONE_MINUS_CONSTANT_COLOR: return GL_ONE_MINUS_CONSTANT_COLOR;
        case BlendFactor::CONSTANT_ALPHA: return GL_CONSTANT_ALPHA;
        case BlendFactor::ONE_MINUS_CONSTANT_ALPHA: return GL_ONE_MINUS_CONSTANT_ALPHA;
        case BlendFactor::SRC_ALPHA_SATURATE: return GL_SRC_ALPHA_SATURATE;
        case BlendFactor::SRC1_COLOR: return GL_SRC1_COLOR;
        case BlendFactor::ONE_MINUS_SRC1_COLOR: return GL_ONE_MINUS_SRC1_COLOR;
        case BlendFactor::SRC1_ALPHA: return GL_SRC1_ALPHA;
        case BlendFactor::ONE_MINUS_SRC1_ALPHA: return GL_ONE_MINUS_SRC1_ALPHA;
        default: return GL_ONE;
    }
}


bool CompileShader( const char* source, i32 sourceLen, GLenum shaderType, RendererID& shaderID ) {
    shaderID = glCreateShader( shaderType );
    glShaderSource(
        shaderID,
        1,
        &source,
        &sourceLen
    );
    glCompileShader( shaderID );

    i32 success;
    glGetShaderiv( shaderID, GL_COMPILE_STATUS, &success );
    if( success == GL_FALSE ) {
        i32 len = 0;
        glGetShaderiv( shaderID, GL_INFO_LOG_LENGTH, &len );
        char* buffer = (char*)malloc( len );
        glGetShaderInfoLog(
            shaderID,
            len,
            nullptr,
            buffer
        );
        LOG_ERROR("OpenGL Compilation Error > %s", buffer);
        free(buffer);
        return false;
    } else { return true; }
}
bool LinkShaders( RendererID shaders[], usize count, RendererID& programID ) {
    programID = glCreateProgram();
    for( usize i = 0; i < count; i++ ) {
        glAttachShader( programID, shaders[i] );
    }
    glLinkProgram( programID );
    for( usize i = 0; i < count; i++ ) {
        glDetachShader( programID, shaders[i] );
        glDeleteShader( shaders[i] );
    }

    i32 success;
    glGetProgramiv( programID, GL_LINK_STATUS, &success );
    if( success == GL_FALSE ) {
        i32 len = 0;
        glGetProgramiv( programID, GL_INFO_LOG_LENGTH, &len );
        char* buffer = (char*)malloc( len );
        glGetProgramInfoLog(
            programID,
            len,
            nullptr,
            buffer
        );
        LOG_ERROR("OpenGL Linking Error > %s", buffer);
        free(buffer);
        return false;
    } else { return true; }

}

void ShaderOpenGL::UseShader() const {
    glUseProgram( m_id );
}
bool ShaderOpenGL::GetUniform( const std::string& uniformName, UniformID& id ) const {
    id = glGetUniformLocation( m_id, uniformName.c_str() );
    if( id < 0 ) {
        LOG_ERROR("OpenGL > Uniform \"%s\" could not be found!", uniformName.c_str() );
        return false;
    } else {
        return true;
    }
}
void ShaderOpenGL::UniformFloat( const UniformID& id, f32 value ) {
    glProgramUniform1f( m_id, id, value );
}
void ShaderOpenGL::UniformInt( const UniformID& id, i32 value )   {
    glProgramUniform1i( m_id, id, value );
}
void ShaderOpenGL::UniformVec2( const UniformID& id, const glm::vec2& value ) {
    glProgramUniform2fv( m_id, id, 1, glm::value_ptr(value) );
}
void ShaderOpenGL::UniformVec3( const UniformID& id, const glm::vec3& value ) {
    glProgramUniform3fv( m_id, id, 1, glm::value_ptr(value) );
}
void ShaderOpenGL::UniformVec4( const UniformID& id, const glm::vec4& value ) {
    glProgramUniform4fv( m_id, id, 1, glm::value_ptr(value) );
}
void ShaderOpenGL::UniformMat4( const UniformID& id, const glm::mat4x4& value ) {
    glProgramUniformMatrix4fv( m_id, id, 1, GL_FALSE, glm::value_ptr(value) );
}
void ShaderOpenGL::UniformMat3( const UniformID& id, const glm::mat3x3& value ) {
    glProgramUniformMatrix3fv( m_id, id, 1, GL_FALSE, glm::value_ptr(value) );
}
ShaderOpenGL::ShaderOpenGL( const std::string& vertex, const std::string& fragment ) {
    RendererID vert, frag;
    if(!CompileShader( vertex.c_str(), vertex.length(), GL_VERTEX_SHADER, vert )) {
        LOG_ERROR("OpenGL > Failed to create vertex shader!");
    }
    if(!CompileShader( fragment.c_str(), fragment.length(), GL_FRAGMENT_SHADER, frag )) {
        LOG_ERROR("OpenGL > Failed to create fragment shader!");
    }
    
    RendererID shaders[] = { vert, frag };
    if( !LinkShaders( shaders, 2, m_id ) ) {
        LOG_ERROR("OpenGL > Failed to link shaders!");
    } else {
        m_success = true;
    }
}
ShaderOpenGL::~ShaderOpenGL() {
    glDeleteProgram( m_id );
}

UniformBufferOpenGL::UniformBufferOpenGL( usize size, const void* data ) {
    glGenBuffers( 1, &m_bufferID );
    glBindBuffer( GL_UNIFORM_BUFFER, m_bufferID );
    glBufferData(
        GL_UNIFORM_BUFFER,
        (GLsizeiptr)size,
        data,
        GL_STATIC_DRAW // TODO: usage
    );
    m_size = size;
}
void UniformBufferOpenGL::BufferData( usize size, const void* data ) {
#ifdef DEBUG
    if( size != m_size ) {
        LOG_WARN("OpenGL > Attempted to buffer data with a size that does not match the buffer size!");
        return;
    }
#endif
    glBindBuffer( GL_UNIFORM_BUFFER, m_bufferID );
    glBufferData(
        GL_UNIFORM_BUFFER,
        (GLsizeiptr)size,
        data,
        GL_STATIC_DRAW // TODO: usage
    );
}
void UniformBufferOpenGL::BufferSubData( usize offset, usize size, const void* data ) {
#ifdef DEBUG
    if( offset + size > m_size ) {
        LOG_WARN("OpenGL > Attempted to buffer sub data with a size that is larger than the buffer size!");
        return;
    }
#endif
    glBindBuffer( GL_UNIFORM_BUFFER, m_bufferID );
    glBufferSubData(
        GL_UNIFORM_BUFFER,
        (GLintptr)offset,
        (GLsizeiptr)size,
        data
    );
}
void UniformBufferOpenGL::SetBindingPoint( usize point ) {
    glBindBufferBase( GL_UNIFORM_BUFFER, point, m_bufferID );
}
void UniformBufferOpenGL::SetBindingPointRange( usize offset, usize size, usize point ) {
#ifdef DEBUG
    if( offset + size > m_size ) {
        LOG_WARN("OpenGL > Attempted to set binding point range that is larger than the buffer size!");
        return;
    }
#endif
    glBindBufferRange(
        GL_UNIFORM_BUFFER,
        point,
        m_bufferID,
        offset,
        size
    );
}
UniformBufferOpenGL::~UniformBufferOpenGL() {
    glDeleteBuffers( 1, &m_bufferID );
}

u32 Platform::BufferDataTypeToGLenum( BufferDataType dataType ) {
    switch( dataType ) {
        case BufferDataType::FLOAT:  return GL_FLOAT;
        case BufferDataType::DOUBLE: return GL_DOUBLE;
        case BufferDataType::UBYTE:  return GL_UNSIGNED_BYTE;
        case BufferDataType::USHORT: return GL_UNSIGNED_SHORT;
        case BufferDataType::UINT:   return GL_UNSIGNED_INT;
        case BufferDataType::BYTE:   return GL_BYTE;
        case BufferDataType::SHORT:  return GL_SHORT;
        case BufferDataType::INT:    return GL_INT;
        case BufferDataType::BOOL:   return GL_BOOL;
        default: return 0;
    }
}

VertexBufferOpenGL::VertexBufferOpenGL( usize size, const void* data ) {
    m_size = size;
    glGenBuffers( 1, &m_id );
    glBindBuffer( GL_ARRAY_BUFFER, m_id );
    glBufferData(
        GL_ARRAY_BUFFER,
        m_size,
        data,
        GL_STATIC_DRAW // TODO: usage?
    );
}
void VertexBufferOpenGL::UseBuffer() const {
    glBindBuffer( GL_ARRAY_BUFFER, m_id );
}
void VertexBufferOpenGL::SetLayout( BufferLayout layout ) {
    m_bufferLayout = layout;
    u32 index = 0;
    for( auto const& element : m_bufferLayout.Elements() ) {
        GLboolean normalized;
        if( element.normalized ) {
            normalized = GL_TRUE;
        } else {
            normalized = GL_FALSE;
        }
        glVertexAttribPointer(
            index,
            BufferDataStructureCount( element.dataStructure ),
            BufferDataTypeToGLenum( element.dataType ),
            normalized,
            m_bufferLayout.Stride(),
            (const void*)element.offset
        );
        glEnableVertexAttribArray(index);
        index++;
    }
}
VertexBufferOpenGL::~VertexBufferOpenGL() {
    glDeleteBuffers( 1, &m_id );
}

IndexBufferOpenGL::IndexBufferOpenGL( BufferDataType type, usize count, const void* data ) {
    m_type  = type;
    m_count = count;
    m_size  = count * BufferDataTypeByteSize( m_type );
    glGenBuffers( 1, &m_id );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_id );
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        m_size,
        data,
        GL_STATIC_DRAW
    );
}
IndexBufferOpenGL::~IndexBufferOpenGL() {
    glDeleteBuffers( 1, &m_id );
}
void IndexBufferOpenGL::UseBuffer() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

VertexArrayOpenGL::VertexArrayOpenGL() {
    glGenVertexArrays( 1, &m_id );
}
VertexArrayOpenGL::~VertexArrayOpenGL() {
    for( auto& vbuffer : m_vertexBuffers ) {
        delete(vbuffer);
    }
    if( m_hasIndexBuffer ) {
        delete(m_indexBuffer);
    }
    glDeleteVertexArrays( 1, &m_id );
}
void VertexArrayOpenGL::UseArray() const {
    glBindVertexArray(m_id);
}
void VertexArrayOpenGL::Unbind() const {
    glBindVertexArray(0);
}
void VertexArrayOpenGL::AddVertexBuffer( VertexBuffer* vertexBuffer ) {
    m_vertexBuffers.push_back(vertexBuffer);
}
void VertexArrayOpenGL::SetIndexBuffer( IndexBuffer* indexBuffer ) {
    m_indexBuffer = indexBuffer;
    m_hasIndexBuffer = true;
}

u32 Platform::TextureFormatToGLenum( TextureFormat format ) {
    switch( format ) {
        case TextureFormat::DEPTH: return GL_DEPTH_COMPONENT;
        case TextureFormat::DEPTH_STENCIL: return GL_DEPTH_STENCIL;
        case TextureFormat::R: return GL_RED;
        case TextureFormat::RG: return GL_RG;
        case TextureFormat::RGB: return GL_RGB;
        case TextureFormat::RGBA: return GL_RGBA;
        default: return 0;
    }
}
i32 Platform::TextureInternalFormatToGLint( TextureInternalFormat format ) {
    switch( format ) {
        case TextureInternalFormat::DEPTH: return GL_DEPTH_COMPONENT;
        case TextureInternalFormat::DEPTH_STENCIL: return GL_DEPTH_STENCIL;
        case TextureInternalFormat::R: return GL_RED;
        case TextureInternalFormat::RG: return GL_RG;
        case TextureInternalFormat::RGB: return GL_RGB;
        case TextureInternalFormat::RGBA: return GL_RGBA;
        default: return 0;
    }
}
i32 Platform::TextureWrapModeToGLint( TextureWrapMode wrap ) {
    switch( wrap ) {
        case TextureWrapMode::CLAMP: return GL_CLAMP_TO_EDGE;
        case TextureWrapMode::REPEAT: return GL_REPEAT;
        case TextureWrapMode::MIRROR_REPEAT: return GL_MIRRORED_REPEAT;
        case TextureWrapMode::MIRROR_CLAMP: return GL_MIRROR_CLAMP_TO_EDGE;
        default: return 0;
    }
}
i32 Platform::TextureFilterMinToGLint( TextureFilterMin filter ) {
    switch( filter ) {
        case TextureFilterMin::NEAREST: return GL_NEAREST;
        case TextureFilterMin::LINEAR:  return GL_LINEAR;
        case TextureFilterMin::LINEAR_MM_NEAREST:  return GL_LINEAR_MIPMAP_NEAREST;
        case TextureFilterMin::LINEAR_MM_LINEAR:   return GL_LINEAR_MIPMAP_LINEAR;
        case TextureFilterMin::NEAREST_MM_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
        case TextureFilterMin::NEAREST_MM_LINEAR:  return GL_NEAREST_MIPMAP_LINEAR;
        default: return 0;
    }
}
i32 Platform::TextureFilterMagToGLint( TextureFilterMag filter ) {
    switch( filter ) {
        case TextureFilterMag::NEAREST: return GL_NEAREST;
        case TextureFilterMag::LINEAR:  return GL_LINEAR;
        default: return 0;
    }
}

Texture2DOpenGL::Texture2DOpenGL(
    const glm::ivec2& dimensions,
    const void* data,
    TextureFormat format,
    TextureInternalFormat internalFormat,
    BufferDataType dataType,
    i32 mipmapLevel,
    bool storeData
) {
    glGenTextures( 1, &m_id );
    glBindTexture( GL_TEXTURE_2D, m_id );
    glTexImage2D(
        GL_TEXTURE_2D,
        mipmapLevel,
        TextureInternalFormatToGLint( internalFormat ),
        dimensions.x,
        dimensions.y,
        0,
        TextureFormatToGLenum( format ),
        BufferDataTypeToGLenum( dataType ),
        data
    );
    if( mipmapLevel == AUTO_MIPMAP ) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    m_dimensions     = dimensions;
    m_format         = format;
    m_internalFormat = internalFormat;
    m_dataType       = dataType;
    m_mipmapLevel    = mipmapLevel;
    m_storeData      = storeData;
    if( storeData ) {
        m_data = (void*)data;
    }
    m_horizontalWrap = TextureWrapMode::REPEAT;
    m_verticalWrap   = TextureWrapMode::REPEAT;
    m_minificationFilter  = TextureFilterMin::NEAREST_MM_LINEAR;
    m_magnificationFilter = TextureFilterMag::LINEAR;
}
Texture2DOpenGL::~Texture2DOpenGL() {
    if( m_storeData ) {
        free( m_data );
    }
    glDeleteTextures( 1, &m_id );
}
void Texture2DOpenGL::SetHorizontalWrap( TextureWrapMode wrap ) {
    m_horizontalWrap = wrap;
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrapModeToGLint( wrap ) );
}
void Texture2DOpenGL::SetVerticalWrap( TextureWrapMode wrap ) {
    m_verticalWrap = wrap;
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrapModeToGLint( wrap ) );
}
void Texture2DOpenGL::SetMinificationFilter( TextureFilterMin filter ) {
    m_minificationFilter = filter;
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFilterMinToGLint( filter ) );
}
void Texture2DOpenGL::SetMagnificationFilter( TextureFilterMag filter ) {
    m_magnificationFilter = filter;
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFilterMagToGLint( filter ) );
}
void Texture2DOpenGL::UseTexture(usize unit) const {
    glBindTextureUnit( (u32)unit, m_id );
}
