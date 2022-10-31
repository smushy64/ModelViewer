#pragma once

// Forward Declaration
namespace Platform {
    struct TextFile;
    class VertexArray;
    class BufferLayout;
}

namespace Core {

Platform::VertexArray* ParseOBJ( const Platform::TextFile& objText );

} // namespace Core

