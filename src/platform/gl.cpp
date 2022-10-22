#include "gl.hpp"
#include "glad/glad.h"
#include "alias.hpp"
#include "debug.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "io.hpp"

using namespace Platform;

const char* FONT_VERT_PATH = "./resources/shaders/font/font.glslVert";
const char* FONT_FRAG_PATH = "./resources/shaders/font/font.glslFrag";

const char* BOUNDS_VERT_PATH = "./resources/shaders/bounds/bounds.glslVert";
const char* BOUNDS_FRAG_PATH = "./resources/shaders/bounds/bounds.glslFrag";

const GLint DEFAULT_UNPACK_ALIGNMENT = 4;

void RendererOpenGL::RenderBoundingBox( const glm::vec4& bounds ) {
    if( !m_renderBoundingBoxes ) {
        return;
    }

    glm::mat4 transform = glm::scale(
        glm::translate( glm::mat4(1.0f), glm::vec3(
            bounds.x,
            bounds.y,
            0.0f
        ) ),
        glm::vec3( bounds.z, bounds.w, 0.0f )
    );

    m_boundsShader->UseShader();
    m_boundsShader->UniformMat4( m_boundsTransformID, transform );

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    glBindVertexArray( m_boundsVAO );
    glDrawElements(
        GL_TRIANGLES,
        6,
        GL_UNSIGNED_INT,
        nullptr
    );

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void RendererOpenGL::SetTextColor( const glm::vec4& color ) {
    m_fontShader->UniformVec4( m_fontColorID, color );
    m_textColor = color;
}
void RendererOpenGL::RenderText( const std::string& text ) {
    if( m_fontAtlas == nullptr ) {
        LOG_ERROR( "OpenGL > Failed to render text, font atlas pointer is null!" );
        return;
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_fontShader->UseShader();

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_fontAtlas->textureID );

    glBindVertexArray( m_fontVAO );
    
    f32 originX = m_textPosition.x;
    f32 yOffset = 0.0f;
    switch(m_textXAnchor) {
        case Core::XAnchor::CENTER: {
            std::string::const_iterator iter;
            f32 stringWidth = 0.0f;
            for( iter = text.begin(); iter < text.end(); iter++ ) {
                if( m_fontAtlas->characterMetrics.count(*iter) == 0 ) {
                    // skip characters not found in character map
                    LOG_WARN("OpenGL > Character \'%c\' not found in font \"%s\"",
                        *iter, m_fontAtlas->name.c_str()
                    );
                    continue;
                }
                auto character = m_fontAtlas->characterMetrics.at( *iter );
                stringWidth += character.advance * m_textScale;
            }
            originX -= stringWidth / 2.0f;
        } break;
        case Core::XAnchor::RIGHT: {
            std::string::const_iterator iter;
            f32 stringWidth = 0.0f;
            for( iter = text.begin(); iter < text.end(); iter++ ) {
                if( m_fontAtlas->characterMetrics.count(*iter) == 0 ) {
                    // skip characters not found in character map
                    LOG_WARN("OpenGL > Character \'%c\' not found in font \"%s\"",
                        *iter, m_fontAtlas->name.c_str()
                    );
                    continue;
                }
                auto character = m_fontAtlas->characterMetrics.at( *iter );
                stringWidth += character.advance * m_textScale;
            }
            originX -= stringWidth;
        } break;
        default: break;
    }

    switch(m_textYAnchor) {
        case Core::YAnchor::CENTER: {
            yOffset = -((m_fontAtlas->pointSize / 2.0f) * m_textScale);
        } break;
        case Core::YAnchor::TOP: {
            yOffset = -(m_fontAtlas->pointSize * m_textScale);
        } break;
        default: break;
    }

    std::string::const_iterator iter;
    for( iter = text.begin(); iter < text.end(); iter++ ) {
        if( m_fontAtlas->characterMetrics.count(*iter) == 0 ) {
            // skip characters not found in character map
            LOG_WARN("OpenGL > Character \'%c\' not found in font \"%s\"",
                *iter, m_fontAtlas->name.c_str()
            );
            continue;
        }
        auto character = m_fontAtlas->characterMetrics.at( *iter );
        RenderCharacter( character, glm::vec2(originX, yOffset) );
        originX += character.advance * m_textScale;
    }

    glDisable(GL_BLEND);
}
void RendererOpenGL::RenderCharacter( const Core::CharMetrics& metrics, const glm::vec2& origin ) {
    glm::vec3 characterScale = glm::vec3( metrics.width, metrics.height, 0.0f ) * m_textScale;
    glm::vec3 characterTranslate = glm::vec3(
        origin.x + (metrics.leftBearing * m_textScale),
        (m_textPosition.y + origin.y) - (metrics.topBearing * m_textScale),
        0.0f
    );

    glm::mat4 transform = glm::scale(
        glm::translate( glm::mat4(1.0f), characterTranslate ),
        characterScale
    );
    m_fontShader->UniformMat4( m_fontTransformID, transform );

    glm::vec4 fontCoords = glm::vec4(
        metrics.atlasX,     // atlas x
        metrics.atlasY,     // atlas y
        metrics.atlasWidth, // width
        metrics.atlasHeight // height
    );

    m_fontShader->UniformVec4( m_fontCoordsID, fontCoords );
    glDrawElements(
        GL_TRIANGLES,
        6,
        GL_UNSIGNED_INT,
        nullptr
    );
}
void RendererOpenGL::LoadFontAtlasBitmap( Core::FontAtlas& fontAtlas ) {
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glGenTextures( 1, &fontAtlas.textureID );
    glBindTexture( GL_TEXTURE_2D, fontAtlas.textureID );
    glTexImage2D(
        GL_TEXTURE_2D,             // target
        0,                         // mipmap level
        GL_RED,                    // internal format
        (i32)fontAtlas.atlasScale, // width
        (i32)fontAtlas.atlasScale, // height
        0,                         // border
        GL_RED,                    // format
        GL_UNSIGNED_BYTE,          // data type
        fontAtlas.bitmap           // data
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(fontAtlas.bitmap);

    glPixelStorei( GL_UNPACK_ALIGNMENT, DEFAULT_UNPACK_ALIGNMENT );
}

void RendererOpenGL::ClearBuffer() {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void RendererOpenGL::SwapBuffer() {
    OpenGLSwapBufferFn();
}

bool RendererOpenGL::LoadOpenGLFunctions( OpenGLLoader loader ) {
    return gladLoadGLLoader( (GLADloadproc)loader ) != 0;
}

void RendererOpenGL::SetClearColor( const glm::vec4& clearColor ) {
    m_clearColor = clearColor;
    glClearColor( clearColor.r, clearColor.g, clearColor.b, clearColor.a );
}

void RendererOpenGL::SetViewport( const glm::vec2& viewport ) {
    m_viewport = viewport;
    glViewport( 0, 0, (GLsizei)viewport.x, (GLsizei)viewport.y );
    glm::mat4 uiProj = glm::ortho( 0.0f, viewport.x, 0.0f, viewport.y );
    m_fontShader->UniformMat4( m_fontProjID, uiProj );
    m_boundsShader->UniformMat4( m_boundsProjID, uiProj );
}

void OpenGLDebugMessageCallback(
    GLenum,        // source
    GLenum,        // type
    GLuint,        // id
    GLenum,        // severity
    GLsizei,       // length
    const GLchar*, // message
    const void*    // userParam
);

bool RendererOpenGL::Initialize() {
#ifdef DEBUG
    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( OpenGLDebugMessageCallback, nullptr );
#endif

    /* Create Font Mesh */ {
        f32 fontVertices[] = {
            /*POSITION*/ 0.0f, 1.0f, /*UV*/ 0.0f, 1.0f,
            /*POSITION*/ 1.0f, 1.0f, /*UV*/ 1.0f, 1.0f,
            /*POSITION*/ 0.0f, 0.0f, /*UV*/ 0.0f, 0.0f,
            /*POSITION*/ 1.0f, 0.0f, /*UV*/ 1.0f, 0.0f
        };
        const usize FONT_VERTEX_COUNT = 16;
        u32 fontIndices[] = {
            0, 1, 2,
            1, 2, 3
        };
        const usize FONT_INDEX_COUNT = 6;

        glGenVertexArrays( 1, &m_fontVAO );
        glBindVertexArray( m_fontVAO );

        glGenBuffers(1, &m_fontVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_fontVBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(f32) * FONT_VERTEX_COUNT,
            &fontVertices,
            GL_STATIC_DRAW
        );
        glVertexAttribPointer(
            0, // index
            4, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized
            sizeof(f32) * 4, // stride
            0 // pointer
        );
        glEnableVertexAttribArray(0);

        glGenBuffers( 1, &m_fontEBO );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_fontEBO );
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(u32) * FONT_INDEX_COUNT,
            &fontIndices,
            GL_STATIC_DRAW
        );

    }

    /* Create Font Shader */ {
        TextFile fontVertSrc = LoadTextFile( FONT_VERT_PATH );
        TextFile fontFragSrc = LoadTextFile( FONT_FRAG_PATH );
        if( fontVertSrc.size == 0 || fontFragSrc.size == 0 ) {
            LOG_ERROR("OpenGL > Failed to load font shaders from disk!");
            return false;
        }
        
        m_fontShader = Shader::New( fontVertSrc.contents, fontFragSrc.contents );

        if( !m_fontShader->CompilationSucceeded() ) {
            LOG_ERROR( "OpenGL > Failed to create font shader!" );
            return false;
        }

        m_fontShader->UseShader();
        m_fontShader->GetUniform( "u_transform", m_fontTransformID );
        m_fontShader->GetUniform( "u_color", m_fontColorID );
        m_fontShader->GetUniform( "u_fontCoords", m_fontCoordsID );
        m_fontShader->GetUniform( "u_viewProjection", m_fontProjID );
        UniformID texSamplerID;
        m_fontShader->GetUniform( "u_texture", texSamplerID );

        m_fontShader->UniformInt( texSamplerID, 0 );
        m_fontShader->UniformVec4( m_fontColorID, m_textColor );
        m_fontShader->UniformMat4( m_fontProjID, glm::ortho( 0.0f, DEFAULT_WINDOW_WIDTH, 0.0f, DEFAULT_WINDOW_HEIGHT ) );
    }

    /* Create Bounds Mesh */ {
        f32 boundsVertices[] = {
            /*POSITION*/ 0.0f, 1.0f,
            /*POSITION*/ 1.0f, 1.0f,
            /*POSITION*/ 0.0f, 0.0f,
            /*POSITION*/ 1.0f, 0.0f
        };
        const usize BOUNDS_VERTEX_COUNT = 8;
        u32 boundsIndices[] = {
            0, 1, 2,
            1, 2, 3
        };
        const usize BOUNDS_INDEX_COUNT = 6;

        glGenVertexArrays( 1, &m_boundsVAO );
        glBindVertexArray( m_boundsVAO );

        glGenBuffers(1, &m_boundsVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_boundsVBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(f32) * BOUNDS_VERTEX_COUNT,
            &boundsVertices,
            GL_STATIC_DRAW
        );
        glVertexAttribPointer(
            0, // index
            2, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized
            sizeof(f32) * 2, // stride
            0 // pointer
        );
        glEnableVertexAttribArray(0);

        glGenBuffers( 1, &m_boundsEBO );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_boundsEBO );
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(u32) * BOUNDS_INDEX_COUNT,
            &boundsIndices,
            GL_STATIC_DRAW
        );

    }

    /* Create Bounds Shader */ {
        TextFile boundsVertSrc = LoadTextFile( BOUNDS_VERT_PATH );
        TextFile boundsFragSrc = LoadTextFile( BOUNDS_FRAG_PATH );
        if( boundsVertSrc.size == 0 || boundsFragSrc.size == 0 ) {
            LOG_ERROR("OpenGL > Failed to load bounds shaders from disk!");
            return false;
        }
        
        m_boundsShader = Shader::New( boundsVertSrc.contents, boundsFragSrc.contents );

        if( !m_boundsShader->CompilationSucceeded() ) {
            LOG_ERROR( "OpenGL > Failed to create bounds shader!" );
            return false;
        }

        m_boundsShader->UseShader();
        m_boundsShader->GetUniform( "u_transform", m_boundsTransformID );
        m_boundsShader->GetUniform( "u_viewProjection", m_boundsProjID );
        m_boundsShader->UniformMat4( m_boundsProjID, glm::ortho( 0.0f, DEFAULT_WINDOW_WIDTH, 0.0f, DEFAULT_WINDOW_HEIGHT ) );
    }

    return true;
}

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

RendererOpenGL::~RendererOpenGL() {
    u32 vao[] = { m_fontVAO, m_boundsVAO };
    glDeleteVertexArrays( 2, vao );
    u32 bo[] = { m_fontVBO, m_fontEBO, m_boundsVBO, m_boundsEBO };
    glDeleteBuffers( 4, bo );
    delete( m_fontShader );
    delete( m_boundsShader );
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

void ShaderOpenGL::UseShader() {
    glUseProgram( m_id );
}
bool ShaderOpenGL::GetUniform( const std::string& uniformName, UniformID& id ) {
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
