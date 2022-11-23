/**
 * Description:  Extended renderer data
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 21, 2022 
*/
#pragma once
#include "pch.hpp"

// forward declaration
namespace Platform {
    struct VertexBufferLayout;
};

namespace Core {

struct camera {
    void recalculateView() {
        viewMat = smath::mat4::lookAt( position, position + forward, up );
    }
    void recalculateProjection() {
        projectionMat = smath::mat4::perspective( fovRad, aspectRatio, clippingPlanes.x, clippingPlanes.y );
    }
    void recalculateBasis() {
        forward = rotation * smath::vec3::forward();
        right   = rotation * smath::vec3::right();
        up      = rotation * smath::vec3::up();
    }
    smath::mat4* matrices() { return &viewMat; }
    usize matrixCount() const { return 2; }

    smath::vec3 position = smath::vec3(0.0f);
    smath::quat rotation = smath::quat::identity();

    smath::vec3 forward = smath::vec3::forward();
    smath::vec3 right   = smath::vec3::right();
    smath::vec3 up      = smath::vec3::up();

    smath::vec2 clippingPlanes = smath::vec2( 0.001f, 100.0f );
    f32 fovRad                 = TO_RAD(65.0f);
    f32 aspectRatio            = 1.778f;

    smath::mat4 viewMat       = smath::mat4::lookAt( position, position + forward, up );
    smath::mat4 projectionMat = smath::mat4::perspective( fovRad, aspectRatio, clippingPlanes.x, clippingPlanes.y );
};

struct vertex {
    smath::vec3 position;
    smath::vec2 uv;
    smath::vec3 normal;
    smath::vec3 tangent;
    smath::vec3 bitangent;
};
Platform::VertexBufferLayout vertexLayout();
void calculateTangentBasis( usize verticesCount, vertex* vertices );

struct ambientLight {
    smath::vec4 color;
};

struct directionalLight {
    smath::vec4 direction;
    smath::vec4 diffuse  = smath::vec4(1.0f);
    smath::vec4 specular = smath::vec4(1.0f) * 0.5f;
};

struct pointLight {
    smath::vec4 position;
    smath::vec4 diffuse  = smath::vec4(1.0f);
    smath::vec4 specular = smath::vec4(1.0f) * 0.5f;

    f32 constant  = 1.0f;
    f32 linear    = 0.14f;
    f32 quadratic = 0.07f;

    bool32 isActive = false;
};

#pragma pack(push, 1)
struct lightBuffer {
    ambientLight     ambient;
    directionalLight directional;
    pointLight       point0;
    pointLight       point1;
    pointLight       point2;
    pointLight       point3;
};
#pragma pack(pop)

} // namespace Core
