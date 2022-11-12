/**
 * Description:  Simple Math operations
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 07, 2022 
 */
#ifndef SMUSHY_MATH
#define SMUSHY_MATH 1

namespace smath {
    /// @brief Linear interpolation
    /// @param a minimum value
    /// @param b maximum value
    /// @param t 0.0-1.0 fraction
    /// @return blend between a and b, based on fraction t
    inline float lerp( float a, float b, float t ) {
        return ( 1.0f - t ) * a + b * t;
    }
    /// @brief Inverse linear interpolation
    /// @param a minimum
    /// @param b maximum
    /// @param v value between
    /// @return fraction that value occupies between a and b
    inline float invLerp( float a, float b, float v ) {
        return ( v - a ) / ( b - a );
    }
    /// @brief Remap value from input range to output range
    /// @param imin input range minimum
    /// @param imax input range maximum
    /// @param omin output range minimum
    /// @param omax output range maximum
    /// @param v value to remap
    /// @return remapped value
    inline float remap( float imin, float imax, float omin, float omax, float v ) {
        float t = invLerp( imin, imax, v );
        return lerp( omin, omax, t );
    }
} // namespace smath


#endif
