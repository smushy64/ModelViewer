/**
 * Description:  .obj loading
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 23, 2022 
*/
#pragma once

// forward declaration
namespace Platform {
    struct RendererAPI;
    struct VertexArray;
    struct File;
};

namespace Core {
    /// @brief Parse OBJ model from file
    /// @param sourceFile file to parse
    /// @param result result
    /// @return true if successful
    bool ParseOBJ( Platform::File* sourceFile, Platform::VertexArray* result, Platform::RendererAPI* api );
} // namespace Core

