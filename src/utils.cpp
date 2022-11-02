#include "utils.hpp"
#include "alias.hpp"
#include "debug.hpp"
#include "consts.hpp"
#include "platform/renderer.hpp"

#include <locale>
#include <codecvt>

std::wstring Utils::CStringToWString( const char* cstring ) {
    isize strLen = mbstowcs( nullptr, cstring, 0 );
    if( strLen == -1 ) {
        LOG_ERROR("Failed to convert const char* to std::wstring. Invalid source str!");
        return std::wstring();
    }

    std::wstring wstring( strLen, 0 );
    mbstowcs( &wstring[0], cstring, wstring.size() );

    return wstring;
}

void Utils::EraseSubString( std::string& string, const std::string& subString ) {
    usize position = string.find(subString);
    if( position != std::string::npos ) {
        string.erase( position, subString.length() );
    }
}

std::wstring Utils::StringtoWString( const std::string& string ) {
    isize strLen = mbstowcs( nullptr, &string[0], 0 );
    if( strLen == -1 ) {
        LOG_ERROR("Failed to convert std::string to std::wstring. Invalid source str!");
        return std::wstring();
    }

    std::wstring wstring( strLen, 0 );
    mbstowcs( &wstring[0], &string[0], wstring.size() );

    return wstring;
}

std::string Utils::WStringToString( const std::wstring& wstring ) {
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;
    return converter.to_bytes( wstring );
}

void Utils::CalculateTangentBasis( std::vector<Platform::Vertex>& vertices ) {
    for( usize i = 0; i < vertices.size(); i += 3 ) {
        auto& p0 = vertices[0 + i].position;
        auto& p1 = vertices[1 + i].position;
        auto& p2 = vertices[2 + i].position;

        auto& uv0 = vertices[0 + i].uv;
        auto& uv1 = vertices[1 + i].uv;
        auto& uv2 = vertices[2 + i].uv;

        auto deltaPos1 = p1 - p0;
        auto deltaPos2 = p2 - p0;

        auto deltaUV1 = uv1 - uv0;
        auto deltaUV2 = uv2 - uv0;

        f32 r = 1.0f / ( deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x );
        auto t = ( deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y ) * r;
        auto b = ( deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x ) * r;

        vertices[0 + i].tangent = t;
        vertices[1 + i].tangent = t;
        vertices[2 + i].tangent = t;

        vertices[0 + i].bitangent = b;
        vertices[1 + i].bitangent = b;
        vertices[2 + i].bitangent = b;
    }
}

std::vector<Platform::Vertex> Utils::CreateCubeMesh() {
    using namespace Platform;
    const usize VERTEX_COUNT = 24;
    std::vector<Vertex> vertices;
    vertices.reserve(VERTEX_COUNT);
    for(usize i = 0; i < VERTEX_COUNT; i++) {
        Vertex zero = {};
        vertices.push_back( zero );
    }
    /*Defining Position, UV, Normal*/ {
        vertices[ 0].position = glm::vec3( -0.5f,  0.5f,  0.5f );
        vertices[ 1].position = glm::vec3(  0.5f,  0.5f,  0.5f );
        vertices[ 2].position = glm::vec3( -0.5f, -0.5f,  0.5f );
        vertices[ 3].position = glm::vec3(  0.5f, -0.5f,  0.5f );
        vertices[ 4].position = glm::vec3( -0.5f,  0.5f, -0.5f );
        vertices[ 5].position = glm::vec3(  0.5f,  0.5f, -0.5f );
        vertices[ 6].position = glm::vec3( -0.5f, -0.5f, -0.5f );
        vertices[ 7].position = glm::vec3(  0.5f, -0.5f, -0.5f );
        vertices[ 8].position = glm::vec3( -0.5f,  0.5f, -0.5f );
        vertices[ 9].position = glm::vec3( -0.5f,  0.5f,  0.5f );
        vertices[10].position = glm::vec3( -0.5f, -0.5f, -0.5f );
        vertices[11].position = glm::vec3( -0.5f, -0.5f,  0.5f );
        vertices[12].position = glm::vec3(  0.5f,  0.5f, -0.5f );
        vertices[13].position = glm::vec3(  0.5f,  0.5f,  0.5f );
        vertices[14].position = glm::vec3(  0.5f, -0.5f, -0.5f );
        vertices[15].position = glm::vec3(  0.5f, -0.5f,  0.5f );
        vertices[16].position = glm::vec3( -0.5f,  0.5f,  0.5f );
        vertices[17].position = glm::vec3(  0.5f,  0.5f,  0.5f );
        vertices[18].position = glm::vec3( -0.5f,  0.5f, -0.5f );
        vertices[19].position = glm::vec3(  0.5f,  0.5f, -0.5f );
        vertices[20].position = glm::vec3( -0.5f, -0.5f,  0.5f );
        vertices[21].position = glm::vec3(  0.5f, -0.5f,  0.5f );
        vertices[22].position = glm::vec3( -0.5f, -0.5f, -0.5f );
        vertices[23].position = glm::vec3(  0.5f, -0.5f, -0.5f );
        vertices[ 0].uv = glm::vec2( 0.0f,  1.0f );
        vertices[ 1].uv = glm::vec2( 1.0f,  1.0f );
        vertices[ 2].uv = glm::vec2( 0.0f,  0.0f );
        vertices[ 3].uv = glm::vec2( 1.0f,  0.0f );
        vertices[ 4].uv = glm::vec2( 0.0f,  1.0f );
        vertices[ 5].uv = glm::vec2( 1.0f,  1.0f );
        vertices[ 6].uv = glm::vec2( 0.0f,  0.0f );
        vertices[ 7].uv = glm::vec2( 1.0f,  0.0f );
        vertices[ 8].uv = glm::vec2( 0.0f,  1.0f );
        vertices[ 9].uv = glm::vec2( 1.0f,  1.0f );
        vertices[10].uv = glm::vec2( 0.0f,  0.0f );
        vertices[11].uv = glm::vec2( 1.0f,  0.0f );
        vertices[12].uv = glm::vec2( 0.0f,  1.0f );
        vertices[13].uv = glm::vec2( 1.0f,  1.0f );
        vertices[14].uv = glm::vec2( 0.0f,  0.0f );
        vertices[15].uv = glm::vec2( 1.0f,  0.0f );
        vertices[16].uv = glm::vec2( 0.0f,  1.0f );
        vertices[17].uv = glm::vec2( 1.0f,  1.0f );
        vertices[18].uv = glm::vec2( 0.0f,  0.0f );
        vertices[19].uv = glm::vec2( 1.0f,  0.0f );
        vertices[20].uv = glm::vec2( 0.0f,  1.0f );
        vertices[21].uv = glm::vec2( 1.0f,  1.0f );
        vertices[22].uv = glm::vec2( 0.0f,  0.0f );
        vertices[23].uv = glm::vec2( 1.0f,  0.0f );
        vertices[ 0].normal = glm::vec3(  0.0f,  0.0f,  1.0f );
        vertices[ 1].normal = glm::vec3(  0.0f,  0.0f,  1.0f );
        vertices[ 2].normal = glm::vec3(  0.0f,  0.0f,  1.0f );
        vertices[ 3].normal = glm::vec3(  0.0f,  0.0f,  1.0f );
        vertices[ 4].normal = glm::vec3(  0.0f,  0.0f, -1.0f );
        vertices[ 5].normal = glm::vec3(  0.0f,  0.0f, -1.0f );
        vertices[ 6].normal = glm::vec3(  0.0f,  0.0f, -1.0f );
        vertices[ 7].normal = glm::vec3(  0.0f,  0.0f, -1.0f );
        vertices[ 8].normal = glm::vec3( -1.0f,  0.0f,  0.0f );
        vertices[ 9].normal = glm::vec3( -1.0f,  0.0f,  0.0f );
        vertices[10].normal = glm::vec3( -1.0f,  0.0f,  0.0f );
        vertices[11].normal = glm::vec3( -1.0f,  0.0f,  0.0f );
        vertices[12].normal = glm::vec3(  1.0f,  0.0f,  0.0f );
        vertices[13].normal = glm::vec3(  1.0f,  0.0f,  0.0f );
        vertices[14].normal = glm::vec3(  1.0f,  0.0f,  0.0f );
        vertices[15].normal = glm::vec3(  1.0f,  0.0f,  0.0f );
        vertices[16].normal = glm::vec3(  0.0f,  1.0f,  0.0f );
        vertices[17].normal = glm::vec3(  0.0f,  1.0f,  0.0f );
        vertices[18].normal = glm::vec3(  0.0f,  1.0f,  0.0f );
        vertices[19].normal = glm::vec3(  0.0f,  1.0f,  0.0f );
        vertices[20].normal = glm::vec3(  0.0f, -1.0f,  0.0f );
        vertices[21].normal = glm::vec3(  0.0f, -1.0f,  0.0f );
        vertices[22].normal = glm::vec3(  0.0f, -1.0f,  0.0f );
        vertices[23].normal = glm::vec3(  0.0f, -1.0f,  0.0f );

    }

    Utils::CalculateTangentBasis( vertices );

    return vertices;
}

std::vector<u32> Utils::CubeIndices() {
    std::vector<u32> result = {
        0, 1, 2,
        1, 3, 2,

        4, 5, 6,
        5, 7, 6,

        8,  9, 10,
        9, 11, 10,

        12, 13, 14,
        13, 15, 14,

        16, 17, 18,
        17, 19, 18,

        20, 21, 22,
        21, 23, 22,
    };
    return result;
}

std::string Utils::GetProgramTitle() {
#ifdef DEBUG
    std::string title = PROGRAM_TITLE;
    title = title + " -- DEBUG --";
    return title;
#else
    return PROGRAM_TITLE;
#endif
}
