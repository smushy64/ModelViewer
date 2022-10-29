#pragma once

#include "alias.hpp"

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Platform {
    class UniformBuffer;
}

namespace Core {

inline const glm::vec3 CAMERA_POSITION = glm::vec3( 0.0f, 0.0f, -1.0f );
inline const f32 CAMERA_NEAR_CLIP = 0.001f;
inline const f32 CAMERA_FAR_CLIP  = 1000.0f;
inline const f32 CAMERA_FOV       = 1.13446f;
inline const f32 CAMERA_ZOOM      = 2.0f;
inline const f32 CAMERA_ASPECT_RATIO = 1.7778f;

class Camera {
public:
    Camera();
    ~Camera();

    // Zoom Camera
    void Zoom( f32 scalar );
    // Set focus point for camera
    void SetPosition( const glm::vec3& focus ) { m_position = focus; }
    // Move camera focus point
    void Move( const glm::vec3& delta ) { SetPosition( m_position + delta ); }
    // Rotate camera around focus point
    void SetRotation( const glm::fquat& rotation ) { m_rotation = rotation; }
    // Rotate camera rotation
    void Rotate( const glm::fquat& rotator ) { SetRotation( rotator * m_rotation ); }
    // Recalculate view matrix and upload to buffer
    void RecalculateView();
    // Recalculate projection matrix and upload to buffer
    void RecalculateProjection();
    // Recalculate basis vectors
    void RecalculateBasisVectors();
    // Set Far Clip
    void SetFarClip( f32 far );
    // Set Near Clip
    void SetNearClip( f32 near );
    // Set Clipping Fields
    void SetClippingFields( f32 near, f32 far );
    // Set Field of View in radians
    void SetFOV( f32 fovRad ) { m_fov = fovRad; }
    // Set aspect ratio
    void SetAspectRatio( f32 aspect ) { m_aspectRatio = aspect; }
    // Set aspect ratio
    void SetAspectRatio( const glm::vec2& resolution ) {
        SetAspectRatio( resolution.x / resolution.y );
    }


public: // Getters
    const glm::fquat& GetRotation() const { return m_rotation; }
    const glm::mat4& GetView() const { return m_view; }
    const glm::mat4& GetProjection() const { return m_proj; }
    // Get Focus point
    const glm::vec3& GetPosition() const { return m_position; }
    const glm::vec3& Forward() const { return m_forward; }
    const glm::vec3& Right() const { return m_right; }
    const glm::vec3& Up() const { return m_up; }

    f32 GetNearClip() const { return m_near; }
    f32 GetFarClip() const { return m_far; }
    glm::vec2 GetClippingFields() const { return glm::vec2( m_near, m_far ); }
    // Get Position of camera
    glm::vec3 GetCameraPoint() const;

    f32 GetFOV() const { return m_fov; }
    f32 GetAspectRatio() const { return m_aspectRatio; }
    f32 GetZoom() const { return m_zoom; }

private:
    glm::vec3  m_position    = glm::vec3(0.0f);

    glm::vec3  m_forward = glm::vec3(0.0f), m_right = glm::vec3(0.0f), m_up = glm::vec3(0.0f);
    glm::fquat m_rotation;

    glm::mat4 m_view;
    glm::mat4 m_proj;

    f32 m_zoom        = CAMERA_ZOOM;
    f32 m_near        = CAMERA_NEAR_CLIP;
    f32 m_far         = CAMERA_FAR_CLIP;
    f32 m_fov         = CAMERA_FOV;
    f32 m_aspectRatio = CAMERA_ASPECT_RATIO;

    Platform::UniformBuffer* m_buffer;
};

} // namespace Core

