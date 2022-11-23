/**
 * Description:  Extended renderer data
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 21, 2022 
*/
#include "renderex.hpp"
#include "platform/renderer.hpp"

Platform::VertexBufferLayout Core::vertexLayout() {
    const usize ELEMENT_COUNT = 5;
    Platform::VertexBufferElement elements[ELEMENT_COUNT] = {
        { Platform::DataStructure::VEC3, Platform::DataType::FLOAT, false },
        { Platform::DataStructure::VEC2, Platform::DataType::FLOAT, false },
        { Platform::DataStructure::VEC3, Platform::DataType::FLOAT, false },
        { Platform::DataStructure::VEC3, Platform::DataType::FLOAT, false },
        { Platform::DataStructure::VEC3, Platform::DataType::FLOAT, false },
    };

    return Platform::CreateVertexBufferLayout( ELEMENT_COUNT, elements );
}

void Core::calculateTangentBasis( usize verticesCount, vertex* vertices ) {
    for( usize i = 0; i < verticesCount; i += 3 ) {
        smath::vec3& p0 = vertices[0 + i].position;
        smath::vec3& p1 = vertices[1 + i].position;
        smath::vec3& p2 = vertices[2 + i].position;

        smath::vec2& uv0 = vertices[0 + i].uv;
        smath::vec2& uv1 = vertices[1 + i].uv;
        smath::vec2& uv2 = vertices[2 + i].uv;

        smath::vec3 deltaPos1 = p1 - p0;
        smath::vec3 deltaPos2 = p2 - p0;

        smath::vec2 deltaUV1 = uv1 - uv0;
        smath::vec2 deltaUV2 = uv2 - uv0;

        f32 r = 1.0f / ( deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x );
        smath::vec3 t = ( deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y ) * r;
        smath::vec3 b = ( deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x ) * r;

        vertices[0 + i].tangent = t;
        vertices[1 + i].tangent = t;
        vertices[2 + i].tangent = t;

        vertices[0 + i].bitangent = b;
        vertices[1 + i].bitangent = b;
        vertices[2 + i].bitangent = b;
    }
}
