#include "camera.hpp"

Core::Camera::Camera(
    const glm::vec3& position,
    const glm::fquat& rotation,
    f32 fov, f32 aspect, f32 near, f32 far
) : m_fov(fov), m_aspect(aspect), m_near(near), m_far(far), m_position(position), m_rotation(rotation) {
    UpdateBasisVectors();
    UpdateViewMatrix();
    UpdateProjectionMatrix();

    m_viewDirty  = false;
    m_basisDirty = false;
    m_projectionDirty = false;
}

const glm::mat4& Core::Camera::ViewMatrix() {
    if( m_viewDirty ) {
        UpdateViewMatrix();
        m_viewDirty = false;
    }
    return m_viewMat;
}
const glm::mat4& Core::Camera::ProjectionMatrix() {
    if( m_projectionDirty ) {
        UpdateProjectionMatrix();
        m_projectionDirty = false;
    }
    return m_projMat;
}

const glm::vec3& Core::Camera::Forward() {
    if( m_basisDirty ) {
        UpdateBasisVectors();
        m_basisDirty = false;
    }
    return m_basisF;
}
const glm::vec3& Core::Camera::Right() {
    if( m_basisDirty ) {
        UpdateBasisVectors();
        m_basisDirty = false;
    }
    return m_basisR;
}
const glm::vec3& Core::Camera::Up() {
    if( m_basisDirty ) {
        UpdateBasisVectors();
        m_basisDirty = false;
    }
    return m_basisU;
}

void Core::Camera::SetFieldOfView( f32 fov ) {
    m_fov = fov;
    m_projectionDirty = true;
}
void Core::Camera::SetAspectRatio( f32 aspect ) {
    m_aspect = aspect;
    m_projectionDirty = true;
}
void Core::Camera::SetClippingFields( f32 near, f32 far ) {
    m_near = near;
    m_far  = far;
    m_projectionDirty = true;
}
void Core::Camera::SetPosition( const glm::vec3& position ) {
    m_position  = position;
    m_viewDirty = true;
}
void Core::Camera::SetRotation( const glm::fquat& rotation ) {
    m_rotation   = rotation;
    m_viewDirty  = true;
    m_basisDirty = true;
}

void Core::Camera::UpdateViewMatrix() {
    m_viewMat = glm::lookAt( m_position, m_position + Forward(), glm::vec3( 0.0f, 1.0f, 0.0f ) );
}
void Core::Camera::UpdateProjectionMatrix() {
    m_projMat = glm::perspective( m_fov, m_aspect, m_near, m_far );
}
void Core::Camera::UpdateBasisVectors() {
    m_basisF = m_rotation * glm::vec3( 0.0f, 0.0f, 1.0f );
    m_basisR = m_rotation * glm::vec3( 1.0f, 0.0f, 0.0f );
    m_basisU = m_rotation * glm::vec3( 0.0f, 1.0f, 0.0f );
}
