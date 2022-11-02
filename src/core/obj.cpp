#include "obj.hpp"
#include "platform/io.hpp"
#include "platform/renderer.hpp"
#include "debug.hpp"
#include "utils.hpp"
#include <vector>
#include <string>

#define TINYOBJLOADER_IMPLEMENTATION 1
#include "include/tiny_obj_loader.h"

const char* OBJ_EXT = "obj";

// assumes that the layout is equivalent to the blinn_phong shader layout
Platform::VertexArray* Core::ParseOBJ( const Platform::TextFile& objText ) {
    std::string path = objText.filePath;
    if( path.substr( path.find_first_of(".") + 1 ) != OBJ_EXT ) {
        LOG_WARN("ParseOBJ > Attempted to parse a file that is not an obj!");
        return nullptr;
    }

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

    std::vector<Platform::Vertex> vertices;
    usize idxOffset = 0;
    for( usize f = 0; f < shape.mesh.num_face_vertices.size(); f++ ) {
        usize fv = (usize)(shape.mesh.num_face_vertices[f]);
        for( usize v = 0; v < fv; v++ ) {
            auto idx = shape.mesh.indices[idxOffset + v];
            Platform::Vertex vertex = {};
            vertex.position.x = attrib.vertices[ 3 * usize( idx.vertex_index ) + 0 ];
            vertex.position.y = attrib.vertices[ 3 * usize( idx.vertex_index ) + 1 ];
            vertex.position.z = attrib.vertices[ 3 * usize( idx.vertex_index ) + 2 ];
            
            if (idx.texcoord_index >= 0) {
                vertex.uv.x = attrib.texcoords[ 2 * usize(idx.texcoord_index) + 0 ];
                vertex.uv.y = attrib.texcoords[ 2 * usize(idx.texcoord_index) + 1 ];
            } else {
                vertex.uv.x = vertex.position.x;
                vertex.uv.y = vertex.position.y;
                vertex.uv = glm::normalize( vertex.uv );
            }

            if (idx.normal_index >= 0) {
                vertex.normal.x = attrib.normals[ 3 * usize(idx.normal_index) + 0 ];
                vertex.normal.y = attrib.normals[ 3 * usize(idx.normal_index) + 1 ];
                vertex.normal.z = attrib.normals[ 3 * usize(idx.normal_index) + 2 ];
            } else {
                vertex.normal = glm::normalize( vertex.position );
            }

            // TODO: calculate tangents

            vertices.push_back( vertex );
        }
        
        idxOffset += fv;
    }

    Utils::CalculateTangentBasis( vertices );

    auto result = Platform::VertexArray::New();
    result->UseArray();

    auto vBuffer = Platform::VertexBuffer::New( sizeof(Platform::Vertex) * vertices.size(), &vertices[0] );
    vBuffer->SetCount( vertices.size() );
    vBuffer->SetLayout( Platform::StandardVertexLayout() );
    result->AddVertexBuffer( vBuffer );

    return result;
}
