/**
 * Description:  Simple Math operations
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 07, 2022 
 */
#ifndef SMUSHY_MATH
#define SMUSHY_MATH 1
#include "alias.hpp"
#include <immintrin.h>
// IMPORTANT(alicia): TEMP
#include <math.h>

namespace smath {

    f32 absf( const f32& value );

    /// @brief Floating-point square root
    inline f32 sqrtf( const f32& value ) {
        __m128 temp = _mm_set_ss(value);
        temp = _mm_sqrt_ss( temp );
        return _mm_cvtss_f32( temp );
    }
    // TODO(alicia): SIMD?

    /// @brief Floating-point square 
    inline f32 sqrf( const f32& value ) {
        return value * value;
    }
    /// @brief Double Floating-point square 
    inline f64 sqrd( const f64& value ) {
        return value * value;
    }
    // TODO(alicia): asin!

    /// @brief asin that does not return nan. 
    /// @return If value >= 1, return pi/2 or -(pi/2) depending on sign,
    /// otherwise return asinf( value ).
    inline f32 noNaNAsin( f32 value ) {
        if( absf( value ) >= 1.0f ) {
            if( value > 0.0f ) {
                return F32::PI / 2.0f;
            } else {
                return -(F32::PI / 2.0f);
            }
        } else {
            return asinf( value );
        }
    }
    /// @brief Clamp an int between min and max values
    /// @param value value to clamp
    /// @param min minimum range, inclusive
    /// @param max maximum range, inclusive
    /// @return clamped value
    inline i8 clampi( i8 value, i8 min, i8 max ) {
        const i8 t = value < min ? min : value;
        return t > max ? max : t;
    }
    /// @brief Clamp an int between min and max values
    /// @param value value to clamp
    /// @param min minimum range, inclusive
    /// @param max maximum range, inclusive
    /// @return clamped value
    inline i16 clampi( i16 value, i16 min, i16 max ) {
        const i16 t = value < min ? min : value;
        return t > max ? max : t;
    }
    /// @brief Clamp an int between min and max values
    /// @param value value to clamp
    /// @param min minimum range, inclusive
    /// @param max maximum range, inclusive
    /// @return clamped value
    inline i32 clampi( i32 value, i32 min, i32 max ) {
        const i32 t = value < min ? min : value;
        return t > max ? max : t;
    }
    /// @brief Clamp an int between min and max values
    /// @param value value to clamp
    /// @param min minimum range, inclusive
    /// @param max maximum range, inclusive
    /// @return clamped value
    inline i64 clampi( i64 value, i64 min, i64 max ) {
        const i64 t = value < min ? min : value;
        return t > max ? max : t;
    }
    /// @brief Clamp an unsigned int between min and max values
    /// @param value value to clamp
    /// @param min minimum range, inclusive
    /// @param max maximum range, inclusive
    /// @return clamped value
    inline u8 clampui( u8 value, u8 min, u8 max ) {
        const u8 t = value < min ? min : value;
        return t > max ? max : t;
    }
    /// @brief Clamp an unsigned int between min and max values
    /// @param value value to clamp
    /// @param min minimum range, inclusive
    /// @param max maximum range, inclusive
    /// @return clamped value
    inline u16 clampui( u16 value, u16 min, u16 max ) {
        const u16 t = value < min ? min : value;
        return t > max ? max : t;
    }
    /// @brief Clamp an unsigned int between min and max values
    /// @param value value to clamp
    /// @param min minimum range, inclusive
    /// @param max maximum range, inclusive
    /// @return clamped value
    inline u32 clampui( u32 value, u32 min, u32 max ) {
        const u32 t = value < min ? min : value;
        return t > max ? max : t;
    }
    /// @brief Clamp an unsigned int between min and max values
    /// @param value value to clamp
    /// @param min minimum range, inclusive
    /// @param max maximum range, inclusive
    /// @return clamped value
    inline u64 clampui( u64 value, u64 min, u64 max ) {
        const u64 t = value < min ? min : value;
        return t > max ? max : t;
    }
    /// @brief Clamp a float between min and max values
    /// @param value value to clamp
    /// @param min minimum range, inclusive
    /// @param max maximum range, inclusive
    /// @return clamped value
    inline f32 clampf( f32 value, f32 min, f32 max ) {
        const f32 t = value < min ? min : value;
        return t > max ? max : t;
    }
    /// @brief Clamp a double between min and max values
    /// @param value value to clamp
    /// @param min minimum range, inclusive
    /// @param max maximum range, inclusive
    /// @return clamped value
    inline f64 clampd( f64 value, f64 min, f64 max ) {
        const f64 t = value < min ? min : value;
        return t > max ? max : t;
    }
    /// @brief Linear interpolation
    /// @param a minimum value
    /// @param b maximum value
    /// @param t fraction
    /// @return blend between a and b, based on fraction t
    inline f32 lerp( const f32& a, const f32& b, const f32& t ) {
        return ( 1.0f - t ) * a + b * t;
    }
    /// @brief Linear interpolation
    /// @param a minimum value
    /// @param b maximum value
    /// @param t fraction
    /// @return blend between a and b, based on fraction t
    inline f64 lerp( const f64& a, const f64& b, const f64& t ) {
        return ( 1.0 - t ) * a + b * t;
    }
    /// @brief Linear interpolation, t is clamped between 0.0-1.0
    /// @param a minimum value
    /// @param b maximum value
    /// @param t 0.0-1.0 fraction
    /// @return blend between a and b, based on fraction t
    inline f32 clampedLerp( const f32& a, const f32& b, const f32& t ) {
        return lerp( a, b, clampf( t, 0.0f, 1.0f ) );
    }
    /// @brief Linear interpolation, t is clamped between 0.0-1.0
    /// @param a minimum value
    /// @param b maximum value
    /// @param t 0.0-1.0 fraction
    /// @return blend between a and b, based on fraction t
    inline f64 clampedLerp( const f64& a, const f64& b, const f64& t ) {
        return lerp( a, b, clampd( t, 0.0, 1.0 ) );
    }
    /// @brief Inverse linear interpolation
    /// @param a minimum
    /// @param b maximum
    /// @param v value between
    /// @return fraction that value occupies between a and b
    inline f32 invLerp( const f32& a, const f32& b, const f32& v ) {
        return ( v - a ) / ( b - a );
    }
    /// @brief Inverse linear interpolation
    /// @param a minimum
    /// @param b maximum
    /// @param v value between
    /// @return fraction that value occupies between a and b
    inline f64 invLerp( const f64& a, const f64& b, const f64& v ) {
        return ( v - a ) / ( b - a );
    }
    /// @brief Remap value from input range to output range
    /// @param imin input range minimum
    /// @param imax input range maximum
    /// @param omin output range minimum
    /// @param omax output range maximum
    /// @param v value to remap
    /// @return remapped value
    inline f32 remap( const f32& imin, const f32& imax, const f32& omin, const f32& omax, const f32& v ) {
        const f32 t = invLerp( imin, imax, v );
        return lerp( omin, omax, t );
    }
    /// @brief Remap value from input range to output range
    /// @param imin input range minimum
    /// @param imax input range maximum
    /// @param omin output range minimum
    /// @param omax output range maximum
    /// @param v value to remap
    /// @return remapped value
    inline f64 remap( const f64& imin, const f64& imax, const f64& omin, const f64& omax, const f64& v ) {
        const f64 t = invLerp( imin, imax, v );
        return lerp( omin, omax, t );
    }
    /// @brief Normalize value to range between -1.0-1.0
    /// @param value value to normalize
    /// @return float between -1.0-1.0
    inline f32 normalizei( i8 value ) {
        if( value > 0 ) {
            return (f32)value / (f32)I8::MAX;
        } else {
            return (f32)value / -(f32)I8::MIN;
        }
    }
    /// @brief Normalize value to range between -1.0-1.0
    /// @param value value to normalize
    /// @return float between -1.0-1.0
    inline f32 normalizei( i16 value ) {
        if( value > 0 ) {
            return (f32)value / (f32)I16::MAX;
        } else {
            return (f32)value / -(f32)I16::MIN;
        }
    }
    /// @brief Normalize value to range between -1.0-1.0
    /// @param value value to normalize
    /// @return float between -1.0-1.0
    inline f32 normalizei( i32 value ) {
        if( value > 0 ) {
            return (f32)value / (f32)I32::MAX;
        } else {
            return (f32)value / -(f32)I32::MIN;
        }
    }
    /// @brief Normalize value to range between -1.0-1.0
    /// @param value value to normalize
    /// @return float between -1.0-1.0
    inline f32 normalizei( i64 value ) {
        if( value > 0 ) {
            return (f32)value / (f32)I64::MAX;
        } else {
            return (f32)value / -(f32)I64::MIN;
        }
    }
    /// @brief Normalize value to range between 0.0-1.0
    /// @param value value to normalize
    /// @return float between 0.0-1.0
    inline f32 normalizeui( u8 value ) { return (f32)value / (f32)U8::MAX; }
    /// @brief Normalize value to range between 0.0-1.0
    /// @param value value to normalize
    /// @return float between 0.0-1.0
    inline f32 normalizeui( u16 value ) { return (f32)value / (f32)U16::MAX; }
    /// @brief Normalize value to range between 0.0-1.0
    /// @param value value to normalize
    /// @return float between 0.0-1.0
    inline f32 normalizeui( u32 value ) { return (f32)value / (f32)U32::MAX; }
    /// @brief Normalize value to range between 0.0-1.0
    /// @param value value to normalize
    /// @return float between 0.0-1.0
    inline f32 normalizeui( u64 value ) { return (f32)value / (f32)U64::MAX; }

    /// @brief Floating-point absolute value 
    inline f32 absf( const f32& value ) {
        return value * ( ( value > 0.0f ) - ( value < 0.0f ) );
    }
    /// @brief Double Floating-point absolute value 
    inline f64 absd( const f64& value ) {
        return value * ( ( value > 0.0 ) - ( value < 0.0 ) );
    }
    /// @brief Integer absolute value 
    inline i8 absi( const i8& value ) {
        return value * ( ( value > 0 ) - ( value < 0 ) );
    }
    /// @brief Integer absolute value 
    inline i16 absi( const i16& value ) {
        return value * ( ( value > 0 ) - ( value < 0 ) );
    }
    /// @brief Integer absolute value 
    inline i32 absi( const i32& value ) {
        return value * ( ( value > 0 ) - ( value < 0 ) );
    }
    /// @brief Integer absolute value 
    inline i64 absi( const i64& value ) {
        return value * ( ( value > 0 ) - ( value < 0 ) );
    }

    /// @brief Truncate i64->i32 and clamp to max/min i32 range
    inline i32 clampTrunc64to32( i64 value ) {
        if( value > (i64)I32::MAX ) {
            return I32::MAX;
        } else if( value < (i64)I32::MIN ) {
            return I32::MIN;
        } else {
            return (i32)value;
        }
    }
    /// @brief Truncate i64->i16 and clamp to max/min i16 range
    inline i16 clampTrunc64to16( i64 value ) {
        if( value > (i64)I16::MAX ) {
            return I16::MAX;
        } else if( value < (i64)I16::MIN ) {
            return I16::MIN;
        } else {
            return (i16)value;
        }
    }
    /// @brief Truncate i64->i8 and clamp to max/min i8 range
    inline i8 clampTrunc64to8( i64 value ) {
        if( value > (i64)I8::MAX ) {
            return I8::MAX;
        } else if( value < (i64)I8::MIN ) {
            return I8::MIN;
        } else {
            return (i8)value;
        }
    }
    /// @brief Truncate u64->u32 and clamp to max u32 range
    inline u32 clampTrunc64to32( u64 value ) {
        if( value > (u64)U32::MAX ) {
            return U32::MAX;
        } else {
            return (u32)value;
        }
    }
    /// @brief Truncate u64->u32 and clamp to max u16 range
    inline u16 clampTrunc64to16( u64 value ) {
        if( value > (u64)U16::MAX ) {
            return U16::MAX;
        } else {
            return (u16)value;
        }
    }
    /// @brief Truncate u64->u8 and clamp to max u8 range
    inline u8 clampTrunc64to8( u64 value ) {
        if( value > (u64)U8::MAX ) {
            return U8::MAX;
        } else {
            return (u8)value;
        }
    }
    /// @brief Truncate i32->i16 and clamp to max/min i16 range
    inline i16 clampTrunc32to16( i32 value ) {
        if( value > (i32)I16::MAX ) {
            return I16::MAX;
        } else if( value < (i32)I16::MIN ) {
            return I16::MIN;
        } else {
            return (i16)value;
        }
    }
    /// @brief Truncate i32->i8 and clamp to max/min i8 range
    inline i8 clampTrunc32to8( i32 value ) {
        if( value > (i32)I8::MAX ) {
            return I8::MAX;
        } else if( value < (i32)I8::MIN ) {
            return I8::MIN;
        } else {
            return (i8)value;
        }
    }
    /// @brief Truncate u32->u16 and clamp to max u16 range
    inline u16 clampTrunc32to16( u32 value ) {
        if( value > (u32)U16::MAX ) {
            return U16::MAX;
        } else {
            return (u16)value;
        }
    }
    /// @brief Truncate u32->u8 and clamp to max u8 range
    inline u8 clampTrunc32to8( u32 value ) {
        if( value > (u32)U8::MAX ) {
            return U8::MAX;
        } else {
            return (u8)value;
        }
    }
    /// @brief Truncate i16->i8 and clamp to max/min i8 range
    inline i8 clampTrunc16to8( i16 value ) {
        if( value > (i16)I8::MAX ) {
            return I8::MAX;
        } else if( value < (i16)I8::MIN ) {
            return I8::MIN;
        } else {
            return (i8)value;
        }
    }
    /// @brief Truncate u16->u8 and clamp to max u8 range
    inline u8 clampTrunc16to8( u16 value ) {
        if( value > (u16)U8::MAX ) {
            return U8::MAX;
        } else {
            return (u8)value;
        }
    }

} // namespace smath


#endif
