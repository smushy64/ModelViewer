#pragma once
#include "alias.hpp"
#include <string>
#include <vector>

// Forward Declaration
namespace Platform {
    struct Vertex;
}

namespace Utils {
    std::wstring CStringToWString( const char* cstring );
    void EraseSubString( std::string& string, const std::string& subString );
    std::wstring StringtoWString( const std::string& string );
    std::string WStringToString( const std::wstring& wstring );
    void CalculateTangentBasis( std::vector<Platform::Vertex>& vertices );
    std::vector<Platform::Vertex> CreateCubeMesh();
    std::vector<u32> CubeIndices();
    std::string GetProgramTitle();

} // namespace Utils

