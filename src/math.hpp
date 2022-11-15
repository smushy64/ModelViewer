/**
 * Description:  Simple Math operations
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 07, 2022 
 */
#ifndef SMUSHY_MATH
#define SMUSHY_MATH 1
#include "alias.hpp"
#include <immintrin.h>

namespace smath {

    /// @brief Floating-point square root
    inline f32 sqrtf( const f32& value ) {
        __m128 temp = _mm_set_ss(value);
        temp = _mm_sqrt_ss( temp );
        return _mm_cvtss_f32( temp );
    }
    /// @brief Clamp a value between min and max values
    /// @param value value to clamp
    /// @param min minimum range, inclusive
    /// @param max maximum range, inclusive
    /// @return clamped value
    template<class T>
    inline constexpr const T& clamp( const T& value, const T& min, const T& max ) {
        const T t = value < min ? min : value;
        return t > max ? max : t;
    }
    /// @brief Linear interpolation
    /// @param a minimum value
    /// @param b maximum value
    /// @param t fraction
    /// @return blend between a and b, based on fraction t
    template<class T>
    inline constexpr T lerp( const T& a, const T& b, const f32& t ) {
        return ( 1.0f - t ) * a + b * t;
    }
    /// @brief Linear interpolation
    /// @param a minimum value
    /// @param b maximum value
    /// @param t fraction
    /// @return blend between a and b, based on fraction t
    template<class T>
    inline constexpr T lerp( const T& a, const T& b, const f64& t ) {
        return ( 1.0 - t ) * a + b * t;
    }
    /// @brief Linear interpolation, t is clamped between 0.0-1.0
    /// @param a minimum value
    /// @param b maximum value
    /// @param t 0.0-1.0 fraction
    /// @return blend between a and b, based on fraction t
    template<class T>
    inline constexpr T clampedLerp( const T& a, const T& b, const f32& t ) {
        return lerp( a, b, clamp( t, 0.0f, 1.0f ) );
    }
    /// @brief Linear interpolation, t is clamped between 0.0-1.0
    /// @param a minimum value
    /// @param b maximum value
    /// @param t 0.0-1.0 fraction
    /// @return blend between a and b, based on fraction t
    template<class T>
    inline constexpr T clampedLerp( const T& a, const T& b, const f64& t ) {
        return lerp( a, b, clamp( t, 0.0, 1.0 ) );
    }
    /// @brief Inverse linear interpolation
    /// @param a minimum
    /// @param b maximum
    /// @param v value between
    /// @return fraction that value occupies between a and b
    template<class T>
    inline constexpr T invLerp( const T& a, const T& b, const T& v ) {
        return ( v - a ) / ( b - a );
    }
    /// @brief Remap value from input range to output range
    /// @param imin input range minimum
    /// @param imax input range maximum
    /// @param omin output range minimum
    /// @param omax output range maximum
    /// @param v value to remap
    /// @return remapped value
    template<class T>
    inline constexpr T remap( const T& imin, const T& imax, const T& omin, const T& omax, const T& v ) {
        const T t = invLerp( imin, imax, v );
        return lerp( omin, omax, t );
    }
    /// @brief Normalize value to range between -1.0-1.0
    /// @param value value to normalize
    /// @return float between -1.0-1.0
    inline f32 normalizeInt( i8 value ) {
        if( value > 0 ) {
            return (f32)value / (f32)I8::MAX;
        } else {
            return (f32)value / -(f32)I8::MIN;
        }
    }
    /// @brief Normalize value to range between -1.0-1.0
    /// @param value value to normalize
    /// @return float between -1.0-1.0
    inline f32 normalizeInt( i16 value ) {
        if( value > 0 ) {
            return (f32)value / (f32)I16::MAX;
        } else {
            return (f32)value / -(f32)I16::MIN;
        }
    }
    /// @brief Normalize value to range between -1.0-1.0
    /// @param value value to normalize
    /// @return float between -1.0-1.0
    inline f32 normalizeInt( i32 value ) {
        if( value > 0 ) {
            return (f32)value / (f32)I32::MAX;
        } else {
            return (f32)value / -(f32)I32::MIN;
        }
    }
    /// @brief Normalize value to range between -1.0-1.0
    /// @param value value to normalize
    /// @return float between -1.0-1.0
    inline f32 normalizeInt( i64 value ) {
        if( value > 0 ) {
            return (f32)value / (f32)I64::MAX;
        } else {
            return (f32)value / -(f32)I64::MIN;
        }
    }
    /// @brief Normalize value to range between 0.0-1.0
    /// @param value value to normalize
    /// @return float between 0.0-1.0
    inline f32 normalizeUint( u8 value ) { return (f32)value / (f32)U8::MAX; }
    /// @brief Normalize value to range between 0.0-1.0
    /// @param value value to normalize
    /// @return float between 0.0-1.0
    inline f32 normalizeUint( u16 value ) { return (f32)value / (f32)U16::MAX; }
    /// @brief Normalize value to range between 0.0-1.0
    /// @param value value to normalize
    /// @return float between 0.0-1.0
    inline f32 normalizeUint( u32 value ) { return (f32)value / (f32)U32::MAX; }
    /// @brief Normalize value to range between 0.0-1.0
    /// @param value value to normalize
    /// @return float between 0.0-1.0
    inline f32 normalizeUint( u64 value ) { return (f32)value / (f32)U64::MAX; }
    
} // namespace smath


#endif
