#include "camera.hpp"
#include "platform/renderer.hpp"

Core::Camera::Camera() {
    m_rotation = glm::fquat(1.0f, 0.0f, 0.0f, 0.0f);
    m_buffer = Platform::UniformBuffer::New( sizeof(glm::mat4) * 2, nullptr );
    m_buffer->SetBindingPoint( MATRIX_3D_BINDING_POINT );
    RecalculateView();
    RecalculateProjection();
    RecalculateBasisVectors();
}
Core::Camera::~Camera() {
    delete(m_buffer);
}

void Core::Camera::Zoom( f32 scalar ) {
    f32 result = m_zoom + scalar;
    if( result > 0.0f ) {
        m_zoom = result;
    }
}
void Core::Camera::RecalculateView() {
    m_view = glm::lookAt(
        GetCameraPoint(),
        m_position,
        Up()
    );
    m_buffer->BufferSubData(0,
        sizeof(glm::mat4),
        glm::value_ptr(m_view)
    );
}
glm::vec3 Core::Camera::GetCameraPoint() const {
    return m_position - (m_rotation * glm::vec3( 0.0f, 0.0f, m_zoom ));
}
void Core::Camera::RecalculateProjection() {
    m_proj = glm::perspective( m_fov, m_aspectRatio, m_near, m_far );
    m_buffer->BufferSubData(sizeof(glm::mat4),
        sizeof(glm::mat4),
        glm::value_ptr(m_proj)
    );
}
void Core::Camera::SetFarClip( f32 far ) {
    m_far = far;
}
void Core::Camera::SetNearClip( f32 near ) {
    m_near = near;
}
void Core::Camera::SetClippingFields( f32 near, f32 far ) {
    SetNearClip(near);
    SetFarClip(far);
}
void Core::Camera::RecalculateBasisVectors() {
    m_forward = glm::normalize( m_position + ( m_rotation * glm::vec3( 0.0f, 0.0f, m_zoom ) ) );
    m_right   = glm::cross( m_forward, glm::vec3( 0.0f, 1.0f, 0.0f ) );
    m_up      = glm::vec3(0.0f, 1.0f, 0.0f);
}
