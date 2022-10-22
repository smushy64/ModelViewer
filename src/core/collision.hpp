#pragma once
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace Core {

// Check if point is within a bounding box
bool PointInBoundingBox( const glm::vec2& point, const glm::vec4& bounds );

} // namespace Core

