#pragma once
#include "alias.hpp"
#include "consts.hpp"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Platform {
    class UniformBuffer;
}

namespace Core {

inline const usize AMBIENT_COLOR_OFFSET = 0;
struct AmbientLightPOD {
    glm::vec4 color;
};

inline const usize DIRECTIONAL_DIRECTION_OFFSET = 0;
inline const usize DIRECTIONAL_DIFFUSE_OFFSET   = 16;
inline const usize DIRECTIONAL_SPECULAR_OFFSET  = 32;
struct DirectionalLightPOD {
    glm::vec4 direction;
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;
};

inline const usize POINT_POSITION_OFFSET  = 0;
inline const usize POINT_DIFFUSE_OFFSET   = 16;
inline const usize POINT_SPECULAR_OFFSET  = 32;
inline const usize POINT_CONSTANT_OFFSET  = 48;
inline const usize POINT_LINEAR_OFFSET    = 52;
inline const usize POINT_QUADRATIC_OFFSET = 56;
inline const usize POINT_ISACTIVE_OFFSET  = 60;
struct PointLightPOD {
    glm::vec4 position;
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;

    f32 constant;
    f32 linear;
    f32 quadratic;

    bool32 isActive;
};

class AmbientLight {
public:
    AmbientLight();

    void SetColor( const glm::vec3& color ) {
        m_pod.color = glm::vec4( color.r, color.g, color.b, 0.0f );
    }

public: // Getters
    const AmbientLightPOD* GetPOD() const { return &m_pod; }
    usize Size() const { return m_size; }
    usize ColorOffset() const { return AMBIENT_COLOR_OFFSET; }

    glm::vec3 GetColor() const { return m_pod.color; }

private:
    AmbientLightPOD m_pod;
    usize m_size = sizeof(AmbientLightPOD);
};

class DirectionalLight {
public: 
    DirectionalLight();

    void SetDirection( const glm::vec3& direction ) {
        m_pod.direction = glm::vec4( direction.x, direction.y, direction.z, 0.0f );
    }
    void SetDiffuse( const glm::vec3& diffuse ) {
        m_pod.diffuseColor = glm::vec4( diffuse.r, diffuse.g, diffuse.b, 0.0f );
    }
    void SetSpecular( const glm::vec3& specular ) {
        m_pod.specularColor = glm::vec4( specular.r, specular.g, specular.b, 0.0f );
    }

public: // Getters
    const DirectionalLightPOD* GetPOD() const { return &m_pod; }
    usize Size() const { return m_size; }
    usize GetDirectionOffset()     const { return DIRECTIONAL_DIRECTION_OFFSET; }
    usize GetDiffuseColorOffset()  const { return DIRECTIONAL_DIFFUSE_OFFSET; }
    usize GetSpecularColorOffset() const { return DIRECTIONAL_SPECULAR_OFFSET; }

    glm::vec3 GetDirection() const { return m_pod.direction; }
    glm::vec3 GetDiffuseColor() const { return m_pod.diffuseColor; }
    glm::vec3 GetSpecularColor() const { return m_pod.specularColor; }

private:
    DirectionalLightPOD m_pod;
    usize m_size = sizeof(DirectionalLightPOD);
};

class PointLight {
public:
    PointLight();

    void SetPosition( const glm::vec3& position ) {
        m_pod.position = glm::vec4( position.x, position.y, position.z, 0.0f );
    }
    void SetDiffuse( const glm::vec3& diffuse ) {
        m_pod.diffuseColor = glm::vec4( diffuse.r, diffuse.g, diffuse.b, 0.0f );
    }
    void SetSpecular( const glm::vec3& specular ) {
        m_pod.specularColor = glm::vec4( specular.r, specular.g, specular.b, 0.0f );
    }
    void Activate();
    void Deactivate();
public: // Getters
    const PointLightPOD* GetPOD() const { return &m_pod; }
    usize Size() const { return m_size; }
    usize GetPositionOffset()      const { return POINT_POSITION_OFFSET; }
    usize GetDiffuseColorOffset()  const { return POINT_DIFFUSE_OFFSET;  }
    usize GetSpecularColorOffset() const { return POINT_SPECULAR_OFFSET; }
    usize GetConstantOffset()      const { return POINT_CONSTANT_OFFSET; }
    usize GetLinearOffset()        const { return POINT_LINEAR_OFFSET; }
    usize GetQuadraticOffset()     const { return POINT_QUADRATIC_OFFSET; }
    usize GetIsActiveOffset()      const { return POINT_ISACTIVE_OFFSET; }

    glm::vec3 GetPosition() const { return m_pod.position; }
    glm::vec3 GetDiffuseColor() const { return m_pod.diffuseColor; }
    glm::vec3 GetSpecularColor() const { return m_pod.specularColor; }

    bool IsActive() const;
private:
    PointLightPOD m_pod;
    usize m_size = sizeof(PointLightPOD);
};

class Lights {
public:
    Lights();
    ~Lights();

public:
    void UploadAmbient() const;
    void UploadDirectional() const;
    void UploadPointLight( usize idx ) const;
    void UploadPointLights() const;
public: // Getters
    AmbientLight&     GetAmbientLight()          { return m_ambient; }
    DirectionalLight& GetDirectionalLight()      { return m_directional; }
    PointLight&       GetPointLight( usize idx ) { return m_pointLights[idx]; }
    usize Size() const { return m_totalSize; }

public:
    void SetAmbientColor( const glm::vec3& color ) { m_ambient.SetColor( color ); }

private:
    void UploadPointLightsInitializer() const;
    AmbientLight     m_ambient;
    DirectionalLight m_directional;
    PointLight       m_pointLights[MAX_POINT_LIGHTS];

    Platform::UniformBuffer* m_buffer;
    usize m_ambientOffset     = 0;
    usize m_directionalOffset = sizeof(AmbientLightPOD);
    usize m_pointLightOffsets[MAX_POINT_LIGHTS];

    usize m_totalSize = 0;
};

} // namespace Core
