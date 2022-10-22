#include "collision.hpp"
#include "debug.hpp"

bool Core::PointInBoundingBox( const glm::vec2& point, const glm::vec4& bounds ) {

    if( point.x < bounds.x ) {
        return false;
    } else if( point.y < bounds.y ) {
        return false;
    } else if( point.x > bounds.x + bounds.z ) {
        return false;
    } else if( point.y > bounds.y + bounds.w ) {
        return false;
    }

    return true;

}
