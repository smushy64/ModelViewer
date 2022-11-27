/**
 * Description:  .obj loading
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 23, 2022 
*/
#include "core/obj.hpp"
#include "util.hpp"
#include "core/renderex.hpp"
#include "platform/io.hpp"
#include "platform/renderer.hpp"

#define TINYOBJLOADER_IMPLEMENTATION 1
#include "tiny_obj_loader.h"

bool Core::ParseOBJ( Platform::File* sourceFile, Platform::VertexArray* result, Platform::RendererAPI* api ) {
    usize subStrPos = 0;
    if( !subStringPos( sourceFile->filePath, ".obj", &subStrPos ) ) {
        LOG_WARN("ParseOBJ > Attempted to parse a file that is not an obj!");
        return false;
    }

    tinyobj::ObjReaderConfig config;
    config.triangulate          = true;
    // config.triangulation_method = "earcut";
    tinyobj::ObjReader reader;

    std::string srcString = std::string( (char*)sourceFile->data, sourceFile->size );

    if( !reader.ParseFromString( srcString, "", config ) ) {
        LOG_ERROR( "ParseOBJ > tinyobjloader > %s", reader.Error().c_str() );
        return false;
    }

    if( !reader.Warning().empty() ) {
        LOG_WARN( "ParseOBJ > tinyobjloader > %s", reader.Warning().c_str() );
    }

    auto& attrib = reader.GetAttrib();
    auto& shape  = reader.GetShapes()[0];

    usize vertexCount = 0;
    ucycles( shape.mesh.num_face_vertices.size() ) {
        vertexCount += shape.mesh.num_face_vertices[i];
    }

    Core::vertex* vertices = (Core::vertex*)Platform::Alloc( vertexCount * sizeof( Core::vertex ) );
    usize idxOffset = 0;
    usize vertexCounter = 0;
    bool uvsPresent = false;
    bool normalsPresent = false;
    ucyclesi( shape.mesh.num_face_vertices.size(), f ) {
        usize fv = (usize)(shape.mesh.num_face_vertices[f]);
        ucyclesi( fv, v ) {
            auto idx = shape.mesh.indices[idxOffset + v];
            Core::vertex* vertex = &vertices[vertexCounter];
            vertexCounter++;

            vertex->position.x = attrib.vertices[ 3 * idx.vertex_index + 0 ];
            vertex->position.y = attrib.vertices[ 3 * idx.vertex_index + 1 ];
            vertex->position.z = attrib.vertices[ 3 * idx.vertex_index + 2 ];

            if( idx.texcoord_index >= 0 ) {
                vertex->uv.x = attrib.texcoords[ 2 * idx.texcoord_index + 0 ];
                vertex->uv.y = attrib.texcoords[ 2 * idx.texcoord_index + 1 ];
            } else {
                vertex->uv.x = vertex->position.x;
                vertex->uv.y = vertex->position.y;
                smath::normalize( vertex->uv );
            }

            if( idx.normal_index >= 0 ) {
                vertex->normal.x = attrib.normals[ 3 * idx.normal_index + 0 ];
                vertex->normal.y = attrib.normals[ 3 * idx.normal_index + 1 ];
                vertex->normal.z = attrib.normals[ 3 * idx.normal_index + 2 ];
            } else {
                vertex->normal = smath::normalize(vertex->position);
            }
        }
        idxOffset += fv;
    }

    Core::calculateTangentBasis( vertexCount, vertices );

    *result = api->CreateVertexArray();
    api->UseVertexArray( result );
    
    auto vbuffer = api->CreateVertexBuffer(
        sizeof( Core::vertex ) * vertexCount,
        &vertices[0],
        Core::vertexLayout()
    );

    Platform::Free( vertices );

    api->VertexArrayBindVertexBuffer( result, vbuffer );

    return true;
}
