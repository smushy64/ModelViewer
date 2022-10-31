#include "obj.hpp"
#include "platform/io.hpp"
#include "platform/renderer.hpp"
#include "debug.hpp"
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION 1
#include "include/tiny_obj_loader.h"

// assumes that the layout is equivalent to the blinn_phong shader layout
Platform::VertexArray* Core::ParseOBJ( const Platform::TextFile& objText ) {
    tinyobj::ObjReaderConfig config;
    config.triangulate = true;
    tinyobj::ObjReader reader;
    if( !reader.ParseFromString( objText.contents, "", config ) ) {
        LOG_ERROR( "ParseOBJ > tinyobjloader > %s", reader.Error().c_str() );
        return nullptr;
    }

    if( !reader.Warning().empty() ) {
        LOG_WARN( "ParseOBJ > tinyobjloader > %s", reader.Warning().c_str() );
    }

    auto& attrib = reader.GetAttrib();
    auto& shape  = reader.GetShapes()[0];

    std::vector<f32> vertices;
    std::vector<u32> indices;
    u32 index = 0;
    usize idxOffset = 0;
    for( usize f = 0; f < shape.mesh.num_face_vertices.size(); f++ ) {
        usize fv = (usize)(shape.mesh.num_face_vertices[f]);
        for( usize v = 0; v < fv; v++ ) {
            auto idx = shape.mesh.indices[idxOffset + v];
            vertices.push_back( attrib.vertices[ 3 * usize( idx.vertex_index ) + 0 ] );
            vertices.push_back( attrib.vertices[ 3 * usize( idx.vertex_index ) + 1 ] );
            vertices.push_back( attrib.vertices[ 3 * usize( idx.vertex_index ) + 2 ] );
            
            if (idx.texcoord_index >= 0) {
                vertices.push_back( attrib.texcoords[ 2 * usize(idx.texcoord_index) + 0 ] );
                vertices.push_back( attrib.texcoords[ 2 * usize(idx.texcoord_index) + 1 ] );
            } else {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }

            if (idx.normal_index >= 0) {
                vertices.push_back( attrib.normals[ 3 * usize(idx.normal_index) + 0 ] );
                vertices.push_back( attrib.normals[ 3 * usize(idx.normal_index) + 1 ] );
                vertices.push_back( attrib.normals[ 3 * usize(idx.normal_index) + 2 ] );
            } else {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }

            indices.push_back( index );
            index++;

        }
        
        idxOffset += fv;
    }

    auto result = Platform::VertexArray::New();
    result->UseArray();

    auto vBuffer = Platform::VertexBuffer::New( sizeof(f32) * vertices.size(), &vertices[0] );
    auto vLayout = Platform::BufferLayout({
        Platform::NewBufferElement(
            "Position",
            Platform::BufferDataType::FLOAT,
            Platform::BufferDataStructure::VEC3,
            false
        ),
        Platform::NewBufferElement(
            "UV",
            Platform::BufferDataType::FLOAT,
            Platform::BufferDataStructure::VEC2,
            false
        ),
        Platform::NewBufferElement(
            "Normal",
            Platform::BufferDataType::FLOAT,
            Platform::BufferDataStructure::VEC3,
            false
        )
    });
    vBuffer->SetLayout( vLayout );
    result->AddVertexBuffer( vBuffer );

    auto iBuffer = Platform::IndexBuffer::New( Platform::BufferDataType::UINT, indices.size(), &indices[0] );
    result->SetIndexBuffer(iBuffer);

    return result;
}
