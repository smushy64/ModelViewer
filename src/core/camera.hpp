#pragma once

#include "alias.hpp"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Core {

inline const f32 DEFAULT_NEAR_CLIP = 0.001f;
inline const f32 DEFAULT_FAR_CLIP  = 1000.0f;
inline const f32 DEFAULT_FOV       = 1.13446f;
inline const f32 DEFAULT_ASPECT_RATIO = 1.7778f;
inline const glm::vec3 DEFAULT_CAMERA_POS = glm::vec3(0.0f);
inline const glm::fquat DEFAULT_CAMERA_ROT = glm::fquat(1.0f, 0.0f, 0.0f, 0.0f);

// Perspective Camera
class Camera {
public:
    Camera(
        const glm::vec3& position,
        const glm::fquat& rotation,
        f32 fov, f32 aspect, f32 near, f32 far
    );
    Camera(
        const glm::vec3& position,
        const glm::fquat& rotation,
        f32 fov, f32 aspect
    ) : Camera( position, rotation, fov, aspect, DEFAULT_NEAR_CLIP, DEFAULT_FAR_CLIP ) {}
    Camera(
        const glm::vec3& position,
        const glm::fquat& rotation
    ) : Camera( position, rotation, DEFAULT_FOV, DEFAULT_ASPECT_RATIO ) {}
    Camera() : Camera( DEFAULT_CAMERA_POS, DEFAULT_CAMERA_ROT ) {}

public: // Getters
    const glm::vec3&  Position() const { return m_position; }
    const glm::fquat& Rotation() const { return m_rotation; }

    // NOTE: if these are dirty, update them before returning value

    const glm::mat4& ViewMatrix();
    const glm::mat4& ProjectionMatrix();
    const glm::vec3& Forward();
    const glm::vec3& Right();
    const glm::vec3& Up();

    f32 FieldOfView() const { return m_fov; }
    f32 AspectRatio() const { return m_aspect; }
    f32 NearClip()    const { return m_near; }
    f32 FarClip()     const { return m_far; }

public: // Setters
    void SetFieldOfView( f32 fov );
    void SetAspectRatio( f32 aspect );
    void SetClippingFields( f32 near, f32 far );

    void SetPosition( const glm::vec3& position );
    void SetRotation( const glm::fquat& rotation );

private:
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();
    void UpdateBasisVectors();

    bool m_projectionDirty;
    bool m_viewDirty;
    bool m_basisDirty;

    f32 m_fov;
    f32 m_aspect;
    f32 m_near, m_far;

    glm::vec3       m_position;

    glm::vec3 m_basisF;
    glm::vec3 m_basisR;
    glm::vec3 m_basisU;

    glm::fquat m_rotation;

    glm::mat4 m_viewMat;
    glm::mat4 m_projMat;
};

} // namespace Core

