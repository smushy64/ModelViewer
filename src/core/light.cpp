#include "light.hpp"
#include <glm/geometric.hpp>
#include "platform/renderer.hpp"

inline const f32 LIGHT_CONSTANT = 1.0f;

#define TRUE 1
#define FALSE 0

Core::AmbientLight::AmbientLight() {
    m_pod = {};
    m_pod.color = glm::vec4(0.0f);
}

Core::DirectionalLight::DirectionalLight() {
    m_pod = {};
    m_pod.direction     = glm::normalize(glm::vec4( -1.0f, 1.0f, 0.0f, 0.0f ));
    m_pod.diffuseColor  = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f) * 0.5f;
    m_pod.specularColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f) * 0.5f;
}

Core::PointLight::PointLight() {
    m_pod = {};
    m_pod.diffuseColor  = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    m_pod.specularColor = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f) * 0.5f;
    m_pod.constant  = LIGHT_CONSTANT;
    m_pod.linear    = 0.14f;
    m_pod.quadratic = 0.07;
    m_pod.isActive  = FALSE;
}
void Core::PointLight::Activate() {
    m_pod.isActive = TRUE;
}
void Core::PointLight::Deactivate() {
    m_pod.isActive = FALSE;
}
bool Core::PointLight::IsActive() const {
    return m_pod.isActive == TRUE;
}

Core::Lights::Lights() {
    isize maxLights = (isize)MAX_POINT_LIGHTS;
    isize pointLight0Offset = (isize)(m_ambient.Size() + m_directional.Size());
    m_totalSize = 0;
    for( isize i = 0; i < maxLights; i++ ) {
        m_pointLightOffsets[i] = usize( pointLight0Offset + ((isize)m_pointLights[0].Size() * i) );
        m_totalSize += m_pointLightOffsets[i];
    }
    m_totalSize += m_directionalOffset + m_ambientOffset;

    m_buffer = Platform::UniformBuffer::New( m_totalSize, nullptr );
    m_buffer->SetBindingPoint( LIGHTS_BINDING_POINT );
    UploadAmbient();
    UploadDirectional();
    UploadPointLightsInitializer();
}
Core::Lights::~Lights() {
    delete(m_buffer);
}
void Core::Lights::UploadAmbient() const {
    m_buffer->BufferSubData( m_ambientOffset, m_ambient.Size(), m_ambient.GetPOD() );
}
void Core::Lights::UploadDirectional() const {
    m_buffer->BufferSubData( m_directionalOffset, m_directional.Size(), m_directional.GetPOD() );
}
void Core::Lights::UploadPointLight( usize idx ) const {
    if( m_pointLights[idx].IsActive() ) {
        m_buffer->BufferSubData(
            m_pointLightOffsets[idx],
            m_pointLights[0].Size(),
            m_pointLights[idx].GetPOD()
        );
    } else {
        m_buffer->BufferSubData(
            m_pointLightOffsets[idx] + m_pointLights[idx].GetIsActiveOffset(),
            sizeof(u32),
            &m_pointLights[idx].GetPOD()->isActive
        );
    }
}
void Core::Lights::UploadPointLightsInitializer() const {
    for( usize i = 0; i < MAX_POINT_LIGHTS; i++ ) {
        m_buffer->BufferSubData(
            m_pointLightOffsets[i],
            m_pointLights[0].Size(),
            m_pointLights[i].GetPOD()
        );
    }
}
void Core::Lights::UploadPointLights() const {
    for( usize i = 0; i < MAX_POINT_LIGHTS; i++ ) {
        UploadPointLight(i);
    }
}

