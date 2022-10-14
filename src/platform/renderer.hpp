#pragma once
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include "consts.hpp"

namespace Platform
{
typedef void (*OpenGLSwapBuffer)(void);
typedef void* (*OpenGLLoader)(const char* functionName);
    
class Renderer {
public:

    enum class Backend {
        OPENGL
    };

    // Create new renderer with given Backend
    static Renderer* New( Backend backend );

    const glm::vec4& GetClearColor() { return m_clearColor; }
    const glm::vec2& GetViewport()   { return m_viewport;   }

public: // NOTE: virtual
    virtual bool Initialize()   = 0;
    virtual void ClearBuffer()  = 0;
    virtual void SwapBuffer()   = 0;
    virtual void SetClearColor( const glm::vec4& clearColor ) = 0;
    virtual void SetViewport( const glm::vec2& viewport ) = 0;

    virtual bool LoadOpenGLFunctions( OpenGLLoader loader ) = 0;
protected:
    glm::vec4 m_clearColor = glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f );
    glm::vec2 m_viewport   = glm::vec2( DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT );

public:
    OpenGLSwapBuffer OpenGLSwapBufferFn;

    virtual ~Renderer() = default;
};

} // namespace Platform

