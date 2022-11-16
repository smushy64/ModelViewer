/**
 * Description:  Extended math functions
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 13, 2022 
 */
#pragma once
#include "alias.hpp"
#include "math.hpp"

// IMPORTANT(alicia): TEMP
#include <math.h>

// TODO(alicia): check todo.md
// TODO(alicia): SIMD!

namespace smath {
    const f32 CMPF_THRESHOLD = 0.0001f;
    struct vec2;
    struct vec3;
    struct vec4;
    struct mat3;
    struct mat4;
    struct quat;

    vec2 operator+( const vec2& lhs, const vec2& rhs );
    vec2 operator-( const vec2& lhs, const vec2& rhs );
    vec2 operator*( const vec2& lhs, const f32& rhs );
    vec2 operator*( const f32& lhs, const vec2& rhs );
    vec2 operator/( const vec2& lhs, const f32& rhs );
    /// @brief 2-component vector
    struct vec2 {
        f32 x, y;

        vec2( f32 x, f32 y ) : x(x), y(y) {}

        /// @brief Normalize vector
        static void normalize( vec2& v ) {
            v /= v.mag();
        }
        /// @brief Component-wise scale lhs by rhs 
        static void scale( vec2& lhs, const vec2& rhs ) {
            // TODO(alicia): SIMD
            lhs.x *= rhs.x;
            lhs.y *= rhs.y;
        }
        /// @brief Compare two vectors
        /// @return true if they are approx. the same
        static bool cmp( const vec2& a, const vec2& b ) {
            return (a - b).sqrmag() < CMPF_THRESHOLD;
        }
        /// @brief Get the dot product between two vectors
        static f32 dot( const vec2& lhs, const vec2& rhs ) {
            // TODO(alicia): SIMD
            return (lhs.x * rhs.x) + (lhs.y * rhs.y);
        }
        /// @brief Get the signed angle between two vectors
        static f32 signedAngle( const vec2& lhs, const vec2& rhs ) {
            // TODO(alicia): write own acos!
            return acos( vec2::dot( lhs, rhs ) );
        }
        /// @brief Get the unsigned angle between two vectors 
        static f32 angle( const vec2& lhs, const vec2& rhs ) {
            return absf( vec2::signedAngle( lhs, rhs ) );
        }

        /// @brief Get square magnitude 
        f32 sqrmag() const {
            // TODO(alicia): SIMD
            return x*x + y*y;
        }
        /// @brief Get magnitude 
        f32 mag() const { return sqrtf(sqrmag()); }
        /// @brief Clone this vector and normalize it 
        vec2 normal() const { return vec2(*this) /= mag(); }
        /// @brief Clamp magnitude
        /// @param max if magnitude is greater than max, normalize and multiply by max
        void clamp( f32 max ) {
            f32 magnitude = mag();
            if( magnitude > max ) {
                *this /= magnitude;
                *this *= max;
            }
        }

        vec2& operator-() {
            return *this *= -1.0f;
        }

        bool operator==( const vec2& rhs ) {
            return vec2::cmp( *this, rhs );
        }
        bool operator!=( const vec2& rhs ) {
            return !(*this == rhs);
        }

        // TODO(alicia): SIMD

        vec2& operator+=( const vec2& rhs ) {
            this->x += rhs.x;
            this->y += rhs.y;
            return *this;
        }
        vec2& operator-=( const vec2& rhs ) {
            this->x -= rhs.x;
            this->y -= rhs.y;
            return *this;
        }
        vec2& operator*=( const f32& rhs ) {
            this->x *= rhs;
            this->y *= rhs;
            return *this;
        }
        vec2& operator/=( const f32& rhs ) {
            this->x /= rhs;
            this->y /= rhs;
            return *this;
        }

        /// @brief Get pointer to struct as f32
        f32* valuePtr() const { return (f32*)this; }
        static vec2 one()   { return {  1.0f,  1.0f }; }
        static vec2 zero()  { return {  0.0f,  0.0f }; }
        static vec2 left()  { return { -1.0f,  0.0f }; }
        static vec2 right() { return {  1.0f,  0.0f }; }
        static vec2 up()    { return {  0.0f,  1.0f }; }
        static vec2 down()  { return {  0.0f, -1.0f }; }
    };
    inline vec2 operator+( const vec2& lhs, const vec2& rhs ) {
        return vec2(lhs) += rhs;
    }
    inline vec2 operator-( const vec2& lhs, const vec2& rhs ) {
        return vec2(lhs) -= rhs;
    }
    inline vec2 operator*( const vec2& lhs, const f32& rhs ) {
        return vec2(lhs) *= rhs;
    }
    inline vec2 operator*( const f32& lhs, const vec2& rhs ) {
        return vec2(rhs) *= lhs;
    }
    inline vec2 operator/( const vec2& lhs, const f32& rhs ) {
        return vec2(lhs) /= rhs;
    }

    /// @brief 2-component integer vector
    struct ivec2 {
        i32 x, y;

        ivec2( i32 x, i32 y ) : x(x), y(y) {}

        /// @brief Component-wise scale lhs by rhs 
        static void scale( ivec2& lhs, const ivec2& rhs ) {
            // TODO(alicia): SIMD
            lhs.x *= rhs.x;
            lhs.y *= rhs.y;
        }
        /// @brief Compare two vectors
        /// @return true if they are the same
        static bool cmp( const ivec2& a, const ivec2& b ) {
            return a.x == b.x && a.y == b.y;
        }
        /// @brief clamp x and y between min and max
        void clamp( const ivec2& min, const ivec2& max ) {
            x = smath::clamp( x, min.x, max.x );
            y = smath::clamp( y, min.y, max.y );
        }

        /// @brief Get square magnitude 
        i32 sqrmag() const {
            // TODO(alicia): SIMD
            return x*x + y*y;
        }
        /// @brief Get magnitude
        f32 mag() const { return sqrtf((f32)sqrmag()); }

        ivec2& operator-() {
            return *this *= -1;
        }

        bool operator==( const ivec2& rhs ) {
            return ivec2::cmp( *this, rhs );
        }
        bool operator!=( const ivec2& rhs ) {
            return !(*this == rhs);
        }

        // TODO(alicia): SIMD

        ivec2& operator+=( const ivec2& rhs ) {
            this->x += rhs.x;
            this->y += rhs.y;
            return *this;
        }
        ivec2& operator-=( const ivec2& rhs ) {
            this->x -= rhs.x;
            this->y -= rhs.y;
            return *this;
        }
        ivec2& operator*=( const i32& rhs ) {
            this->x *= rhs;
            this->y *= rhs;
            return *this;
        }
        ivec2& operator/=( const i32& rhs ) {
            this->x /= rhs;
            this->y /= rhs;
            return *this;
        }

        /// @brief Get pointer to struct as i32
        i32* valuePtr() const { return (i32*)this; }
        static ivec2 one()   { return {  1,  1 }; }
        static ivec2 zero()  { return {  0,  0 }; }
        static ivec2 left()  { return { -1,  0 }; }
        static ivec2 right() { return {  1,  0 }; }
        static ivec2 up()    { return {  0,  1 }; }
        static ivec2 down()  { return {  0, -1 }; }
    };
    inline ivec2 operator+( const ivec2& lhs, const ivec2& rhs ) {
        return ivec2(lhs) += rhs;
    }
    inline ivec2 operator-( const ivec2& lhs, const ivec2& rhs ) {
        return ivec2(lhs) -= rhs;
    }
    inline ivec2 operator*( const ivec2& lhs, const i32& rhs ) {
        return ivec2(lhs) *= rhs;
    }
    inline ivec2 operator*( const i32& lhs, const ivec2& rhs ) {
        return ivec2(rhs) *= lhs;
    }
    inline ivec2 operator/( const ivec2& lhs, const i32& rhs ) {
        return ivec2(lhs) /= rhs;
    }

    vec3 operator+( const vec3& lhs, const vec3& rhs );
    vec3 operator-( const vec3& lhs, const vec3& rhs );
    vec3 operator*( const vec3& lhs, const f32& rhs );
    vec3 operator*( const f32& lhs, const vec3& rhs );
    vec3 operator/( const vec3& lhs, const f32& rhs );
    /// @brief 3-component vector
    struct vec3 {
        f32 x, y, z;
        
        vec3( f32 x, f32 y, f32 z ) : x(x), y(y), z(z) {}

        /// @brief Normalize vector
        static void normalize( vec3& v ) {
            v /= v.mag();
        }
        /// @brief Component-wise scale lhs by rhs 
        static void scale( vec3& lhs, const vec3& rhs ) {
            // TODO(alicia): SIMD
            lhs.x *= rhs.x;
            lhs.y *= rhs.y;
            lhs.z *= rhs.z;
        }
        /// @brief Compare two vectors
        /// @return true if they are approx. the same
        static bool cmp( const vec3& a, const vec3& b ) {
            return (a - b).sqrmag() < CMPF_THRESHOLD;
        }
        /// @brief Get the dot product between two vectors
        static f32 dot( const vec3& lhs, const vec3& rhs ) {
            // TODO(alicia): SIMD
            return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
        }
        /// @brief Get the cross product between two vectors 
        static vec3 cross( const vec3& lhs, const vec3& rhs ) {
            // TODO(alicia): SIMD
            return {
                ( lhs.y * rhs.z ) - ( lhs.z * rhs.y ),
                ( lhs.z * rhs.x ) - ( lhs.x * rhs.z ),
                ( lhs.x * rhs.y ) - ( lhs.y * rhs.x )
            };
        }
        /// @brief Get direction reflected off of given normal 
        static vec3 reflect( const vec3& direction, const vec3& normal ) {
            return ( normal - direction ) * ( 2.0f * ( vec3::dot( direction, normal ) ) );
        }
        /// @brief Get the signed angle between two vectors
        static f32 signedAngle( const vec3& lhs, const vec3& rhs ) {
            // TODO(alicia): write own acos!
            return acos( vec3::dot( lhs, rhs ) );
        }
        /// @brief Get the unsigned angle between two vectors 
        static f32 angle( const vec3& lhs, const vec3& rhs ) {
            return absf( vec3::signedAngle( lhs, rhs ) );
        }

        /// @brief Get square magnitude 
        f32 sqrmag() const {
            // TODO(alicia): SIMD
            return x*x + y*y + z*z;
        }
        /// @brief Get magnitude 
        f32 mag() const { return sqrtf(sqrmag()); }
        /// @brief Clone this vector and normalize it 
        vec3 normal() const { return vec3(*this) /= mag(); }
        /// @brief Clamp magnitude
        /// @param max if magnitude is greater than max, normalize and multiply by max
        void clamp( f32 max ) {
            f32 magnitude = mag();
            if( magnitude > max ) {
                *this /= magnitude;
                *this *= max;
            }
        }

        vec3& operator-() {
            return *this *= -1.0f;
        }

        bool operator==( const vec3& rhs ) {
            return vec3::cmp( *this, rhs );
        }
        bool operator!=( const vec3& rhs ) {
            return !( *this == rhs );
        }

        // TODO(alicia): SIMD

        vec3& operator+=( const vec3& rhs ) {
            this->x += rhs.x;
            this->y += rhs.y;
            this->z += rhs.z;
            return *this;
        }
        vec3& operator-=( const vec3& rhs ) {
            this->x -= rhs.x;
            this->y -= rhs.y;
            this->z -= rhs.z;
            return *this;
        }
        vec3& operator*=( const f32& rhs ) {
            this->x *= rhs;
            this->y *= rhs;
            this->z *= rhs;
            return *this;
        }
        vec3& operator/=( const f32& rhs ) {
            this->x /= rhs;
            this->y /= rhs;
            this->z /= rhs;
            return *this;
        }

        /// @brief Get pointer to struct as f32
        f32* valuePtr() const { return (f32*)this; }
        static vec3 one()     { return {  1.0f,  1.0f,  1.0f }; }
        static vec3 zero()    { return {  0.0f,  0.0f,  0.0f }; }
        static vec3 left()    { return { -1.0f,  0.0f,  0.0f }; }
        static vec3 right()   { return {  1.0f,  0.0f,  0.0f }; }
        static vec3 up()      { return {  0.0f,  1.0f,  0.0f }; }
        static vec3 down()    { return {  0.0f, -1.0f,  0.0f }; }
        static vec3 forward() { return {  0.0f,  0.0f,  1.0f }; }
        static vec3 back()    { return {  0.0f,  0.0f, -1.0f }; }
    };
    inline vec3 operator+( const vec3& lhs, const vec3& rhs ) {
        return vec3(lhs) += rhs;
    }
    inline vec3 operator-( const vec3& lhs, const vec3& rhs ) {
        return vec3(lhs) -= rhs;
    }
    inline vec3 operator*( const vec3& lhs, const f32& rhs ) {
        return vec3(lhs) *= rhs;
    }
    inline vec3 operator*( const f32& lhs, const vec3& rhs ) {
        return vec3(rhs) *= lhs;
    }
    inline vec3 operator/( const vec3& lhs, const f32& rhs ) {
        return vec3(lhs) /= rhs;
    }
    
    vec4 operator+( const vec4& lhs, const vec4& rhs );
    vec4 operator-( const vec4& lhs, const vec4& rhs );
    vec4 operator*( const vec4& lhs, const f32& rhs );
    vec4 operator*( const f32& lhs, const vec4& rhs );
    vec4 operator/( const vec4& lhs, const f32& rhs );
    /// @brief 4-component vector
    struct vec4 {
        f32 x, y, z, w;

        vec4( f32 x, f32 y, f32 z, f32 w ) : x(x), y(y), z(z), w(w) {}

        /// @brief Normalize vector
        static void normalize( vec4& v ) {
            v /= v.mag();
        }
        /// @brief Compare two vectors
        /// @return true if they are approx. the same
        static bool cmp( const vec4& a, const vec4& b ) {
            return (a - b).sqrmag() < CMPF_THRESHOLD;
        }

        /// @brief Get square magnitude 
        f32 sqrmag() const {
            // TODO(alicia): SIMD
            return x*x + y*y + z*z;
        }
        /// @brief Get magnitude 
        f32 mag() const { return sqrtf(sqrmag()); }
        /// @brief Clone this vector and normalize it 
        vec4 normal() const { return vec4(*this) /= mag(); }

        vec4& operator-() {
            return *this *= -1.0f;
        }

        bool operator==( const vec4& rhs ) {
            return vec4::cmp( *this, rhs );
        }
        bool operator!=( const vec4& rhs ) {
            return !( *this == rhs );
        }

        // TODO(alicia): SIMD

        vec4& operator+=( const vec4& rhs ) {
            this->x += rhs.x;
            this->y += rhs.y;
            this->z += rhs.z;
            this->w += rhs.w;
            return *this;
        }
        vec4& operator-=( const vec4& rhs ) {
            this->x -= rhs.x;
            this->y -= rhs.y;
            this->z -= rhs.z;
            this->w -= rhs.w;
            return *this;
        }
        vec4& operator*=( const f32& rhs ) {
            this->x *= rhs;
            this->y *= rhs;
            this->z *= rhs;
            this->w *= rhs;
            return *this;
        }
        vec4& operator/=( const f32& rhs ) {
            this->x /= rhs;
            this->y /= rhs;
            this->z /= rhs;
            this->w /= rhs;
            return *this;
        }
        
        /// @brief Get pointer to struct as f32
        f32* valuePtr() const { return (f32*)this; }
        static vec4 one()  { return {  1.0f, 1.0f, 1.0f, 1.0f }; }
        static vec4 zero() { return {  0.0f, 0.0f, 0.0f, 0.0f }; }
    };
    inline vec4 operator+( const vec4& lhs, const vec4& rhs ) {
        return vec4(lhs) += rhs;
    }
    inline vec4 operator-( const vec4& lhs, const vec4& rhs ) {
        return vec4(lhs) -= rhs;
    }
    inline vec4 operator*( const vec4& lhs, const f32& rhs ) {
        return vec4(lhs) *= rhs;
    }
    inline vec4 operator*( const f32& lhs, const vec4& rhs ) {
        return vec4(rhs) *= lhs;
    }
    inline vec4 operator/( const vec4& lhs, const f32& rhs ) {
        return vec4(lhs) /= rhs;
    }

    quat operator+( const quat& lhs, const quat& rhs );
    quat operator-( const quat& lhs, const quat& rhs );
    quat operator*( const quat& lhs, const f32& rhs );
    quat operator*( const f32& lhs, const quat& rhs );
    quat operator/( const quat& lhs, const f32& rhs );
    /// @brief quaternion
    struct quat {
        f32 w, x, y, z;

        /// @brief spherically interpolate from a to b 
        static quat slerp( const quat& a, const quat& b, f32 t ) {
            f32 dot    = quat::dot( a, b );
            f32 lambda = t / 2.0f;
            // TODO(alicia): acos!
            f32 theta  = acosf( dot );
            if( theta < 0.0f ) {
                theta = -theta;
            }
            // TODO(alicia): sin!
            f32 thetaSin = sinf( theta );
            f32 coeff1 = sinf( ( 1.0f - lambda ) * theta) / thetaSin;
            f32 coeff2 = sinf( lambda * theta ) / thetaSin;

            quat result = {
                ( coeff1 * a.w ) + ( coeff2 * b.w ),
                ( coeff1 * a.x ) + ( coeff2 * b.x ),
                ( coeff1 * a.y ) + ( coeff2 * b.y ),
                ( coeff1 * a.z ) + ( coeff2 * b.z )
            };

            return result.normal();
        }
        /// @brief Normalize quaternion
        static void normalize( quat& v ) {
            v /= v.mag();
        }
        /// @brief Get the dot product between two quaternions
        static f32 dot( const quat& lhs, const quat& rhs ) {
            // TODO(alicia): SIMD
            return (lhs.w * rhs.w) + (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
        }
        /// @brief square magnitude 
        f32 sqrMag() const {
            return ( w * w ) + ( x * x ) + ( y * y ) + ( z * z );
        }
        /// @brief square magnitude 
        f32 mag() const {
            return sqrtf( sqrMag() );
        }
        /// @brief Clone this quaternion and normalize it 
        quat normal() const { return quat(*this) /= mag(); }
        /// @brief get conjugate 
        quat conjugate() const { return { w, -x, -y, -z }; }
        /// @brief get inverse
        quat inverse() const { return conjugate() / pow( mag(), 2.0f ); }

        /// @brief construct euler angles from quaternion
        void toEuler( vec3* result ) {
            f32 y2 = y * y;
            // TODO(alicia): atan2f!
            result->x = atan2f(
                2.0f * ( (w * x) + (y * z) ),
                1.0f - (2.0f * ( (x * x) + y2 ))
            );
            result->y = noNaNAsin( 2.0f * ( ( w * y ) - ( z * x ) ) );
            result->z = atan2f(
                2.0f * ( (w * z) + (x * y) ),
                1.0f - (2.0f * ( y2 + (z * z) ))
            );
        }
        /// @brief construct angle axis from quaternion
        void toAngleAxis( f32* angle, vec3* axis ) {
            // TODO(alicia): acos!
            *angle  = acosf(2.0f * w);

            f32 sqrtinvw2 = sqrtf( 1.0f - (w * w) );
            axis->x = x / sqrtinvw2;
            axis->y = y / sqrtinvw2;
            axis->z = z / sqrtinvw2;
        }

        // TODO(alicia): SIMD!

        quat& operator+=( const quat& rhs ) {
            this->x += rhs.x;
            this->y += rhs.y;
            this->z += rhs.z;
            this->w += rhs.w;
            return *this;
        }
        quat& operator-=( const quat& rhs ) {
            this->x -= rhs.x;
            this->y -= rhs.y;
            this->z -= rhs.z;
            this->w -= rhs.w;
            return *this;
        }
        quat& operator*=( const f32& rhs ) {
            this->x *= rhs;
            this->y *= rhs;
            this->z *= rhs;
            this->w *= rhs;
            return *this;
        }
        quat& operator/=( const f32& rhs ) {
            this->x /= rhs;
            this->y /= rhs;
            this->z /= rhs;
            this->w /= rhs;
            return *this;
        }

        /// @brief identity quaternion 
        static quat identity() { return { 1.0f, 0.0f, 0.0f, 0.0f }; }
        /// @brief construct quaternion from angle axis 
        static quat angleAxis( f32 rad, vec3 axis ) {
            // TODO(alicia): sinf! cosf!
            f32 s = sinf(rad / 2.0f);
            return {
                cosf( rad / 2.0f ),
                axis.x * s,
                axis.y * s,
                axis.z * s,
            };
        }
    };
    inline quat operator+( const quat& lhs, const quat& rhs ) {
        return quat(lhs) += rhs;
    }
    inline quat operator-( const quat& lhs, const quat& rhs ) {
        return quat(lhs) -= rhs;
    }
    inline quat operator*( const quat& lhs, const f32& rhs ) {
        return quat(lhs) *= rhs;
    }
    inline quat operator*( const f32& lhs, const quat& rhs ) {
        return quat(rhs) *= lhs;
    }
    inline quat operator/( const quat& lhs, const f32& rhs ) {
        return quat(lhs) /= rhs;
    }
    // TODO(alicia): SIMD!

    inline quat operator*( const quat& lhs, const quat& rhs ) {
        return {
            ( lhs.w * rhs.w ) - ( (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z) ),
            ( lhs.w * rhs.x ) + ( rhs.w * lhs.x ) + ( ( lhs.y * rhs.z ) - ( lhs.z * rhs.y ) ),
            ( lhs.w * rhs.y ) + ( rhs.w * lhs.y ) + ( ( lhs.z * rhs.x ) - ( lhs.x * rhs.z ) ),
            ( lhs.w * rhs.z ) + ( rhs.w * lhs.z ) + ( ( lhs.x * rhs.y ) - ( lhs.y * rhs.x ) )
        };
    }
    inline quat operator*( const quat& lhs, const vec3& rhs ) {
        quat p = { 0.0f, rhs.x, rhs.y, rhs.z };
        quat result = lhs * p * lhs.conjugate();
        return { result.x, result.y, result.z };
    }

    /// @brief 3x3 matrix
    struct mat3 {
        f32 _c0r0, _c1r0, _c2r0;
        f32 _c0r1, _c1r1, _c2r1;
        f32 _c0r2, _c1r2, _c2r2;

        /// @brief get matrix as row-major
        mat3 transpose() const {
            return {
                (*this)[0], (*this)[3], (*this)[6],
                (*this)[1], (*this)[4], (*this)[7],
                (*this)[2], (*this)[5], (*this)[8]
            };
        }
        /// @brief get determinant 
        f32 determinant() const {
            return ( (*this)[0] * ( ( (*this)[4] * (*this)[8] ) - ( (*this)[7] * (*this)[5] ) ) ) +
            -( (*this)[3] * ( ( (*this)[1] * (*this)[8] ) - ( (*this)[7] * (*this)[2] ) ) ) +
             ( (*this)[6] * ( ( (*this)[1] * (*this)[5] ) - ( (*this)[4] * (*this)[2] ) ) );
        }
        /// @brief swap columns with rows
        void transpose_self() {
            *this = transpose();
        }
        /// @brief get column
        /// @param column column index [0-2]
        /// @param buffer buffer to store values
        /// IMPORTANT: buffer must be able to hold at least 3 values!
        void getColumn( usize column, f32* buffer ) {
            forloop(3) {
                buffer[i] = (*this)[column + (i * 3)];
            }
        }
        /// @brief get row
        /// @param row row index [0-2]
        /// @param buffer buffer to store values
        /// IMPORTANT: buffer must be able to hold at least 3 values!
        void getRow( usize row, f32* buffer ) const {
            forloop(3) {
                buffer[i] = (*this)[(row * 3) + i];
            }
        }

        f32& operator[]( i32 index ) {
            return valuePtr()[index];
        }
        f32 operator[]( i32 index ) const {
            return valuePtr()[index];
        }

        // TODO(alicia): SIMD!

        mat3& operator+=( const mat3& rhs ) {
            (*this)[0] += rhs[0];
            (*this)[1] += rhs[1];
            (*this)[2] += rhs[2];
            (*this)[3] += rhs[3];
            (*this)[4] += rhs[4];
            (*this)[5] += rhs[5];
            (*this)[6] += rhs[6];
            (*this)[7] += rhs[7];
            (*this)[8] += rhs[8];
            return *this;
        }
        mat3& operator-=( const mat3& rhs ) {
            (*this)[0] -= rhs[0];
            (*this)[1] -= rhs[1];
            (*this)[2] -= rhs[2];
            (*this)[3] -= rhs[3];
            (*this)[4] -= rhs[4];
            (*this)[5] -= rhs[5];
            (*this)[6] -= rhs[6];
            (*this)[7] -= rhs[7];
            (*this)[8] -= rhs[8];
            return *this;
        }
        mat3& operator*=( const f32& rhs ) {
            (*this)[0] *= rhs;
            (*this)[1] *= rhs;
            (*this)[2] *= rhs;
            (*this)[3] *= rhs;
            (*this)[4] *= rhs;
            (*this)[5] *= rhs;
            (*this)[6] *= rhs;
            (*this)[7] *= rhs;
            (*this)[8] *= rhs;
            return *this;
        }
        mat3& operator/=( const f32& rhs ) {
            (*this)[0] /= rhs;
            (*this)[1] /= rhs;
            (*this)[2] /= rhs;
            (*this)[3] /= rhs;
            (*this)[4] /= rhs;
            (*this)[5] /= rhs;
            (*this)[6] /= rhs;
            (*this)[7] /= rhs;
            (*this)[8] /= rhs;
            return *this;
        }

        /// @brief Get pointer to struct as f32
        f32* valuePtr() const { return (f32*)this; }
        /// @brief create normal mat
        /// @param transform transform matrix to build normal matrix from
        /// @param result 
        /// @return true if transform matrix can be inverted
        static bool normalMat( const mat4* transform, mat3* result ) {
            mat4 inverseTransform = {};
            if( transform->inverse( &inverseTransform ) ) {
                *result = inverseTransform.transpose().toMat3();
                return true;
            } else {
                return false;
            }
        }
        static mat3 zero() { return {}; }
        static mat3 identity() {
            mat3 result = {};
            result[0] = 1.0f;
            result[4] = 1.0f;
            result[8] = 1.0f;
            return result;
        }
    };
    inline mat3 operator+( const mat3& lhs, const mat3& rhs ) {
        return mat3(lhs) += rhs;
    }
    inline mat3 operator-( const mat3& lhs, const mat3& rhs ) {
        return mat3(lhs) -= rhs;
    }
    inline mat3 operator*( const mat3& lhs, const f32& rhs ) {
        return mat3(lhs) *= rhs;
    }
    inline mat3 operator*( const f32& lhs, const mat3& rhs ) {
        return mat3(rhs) *= lhs;
    }
    inline mat3 operator/( const mat3& lhs, const f32& rhs ) {
        return mat3(lhs) /= rhs;
    }
    // TODO(alicia): SIMD!

    inline mat3 operator*( const mat3& lhs, const mat3& rhs ) {
        return {// column - 0
            ( lhs[0] * rhs[0] ) + ( lhs[3] * rhs[1] ) + ( lhs[6] * rhs[2] ),
            ( lhs[1] * rhs[0] ) + ( lhs[4] * rhs[1] ) + ( lhs[7] * rhs[2] ),
            ( lhs[2] * rhs[0] ) + ( lhs[5] * rhs[1] ) + ( lhs[8] * rhs[2] ),
            // column - 1
            ( lhs[0] * rhs[3] ) + ( lhs[3] * rhs[4] ) + ( lhs[6] * rhs[5] ),
            ( lhs[1] * rhs[3] ) + ( lhs[4] * rhs[4] ) + ( lhs[7] * rhs[5] ),
            ( lhs[2] * rhs[3] ) + ( lhs[5] * rhs[4] ) + ( lhs[8] * rhs[5] ),
            // column - 2
            ( lhs[0] * rhs[6] ) + ( lhs[3] * rhs[7] ) + ( lhs[6] * rhs[8] ),
            ( lhs[1] * rhs[6] ) + ( lhs[4] * rhs[7] ) + ( lhs[7] * rhs[8] ),
            ( lhs[2] * rhs[6] ) + ( lhs[5] * rhs[7] ) + ( lhs[8] * rhs[8] )
        };
    }

    /// @brief 4x4 matrix
    struct mat4 {
        f32 _c0r0, _c1r0, _c2r0, _c3r0;
        f32 _c0r1, _c1r1, _c2r1, _c3r1;
        f32 _c0r2, _c1r2, _c2r2, _c3r2;
        f32 _c0r3, _c1r3, _c2r3, _c3r3;

        /// @brief get determinant of matrix 
        f32 determinant() const {
            return ( (*this)[0] * cellSubMatrix( 0, 0 ).determinant() ) -
            ( (*this)[4] * cellSubMatrix( 0, 1 ).determinant() ) +
            ( (*this)[8] * cellSubMatrix( 0, 2 ).determinant() ) -
            ( (*this)[12] * cellSubMatrix( 0, 3 ).determinant() );
        }
        /// @brief get matrix as row-major
        mat4 transpose() const {
            return {
                (*this)[0], (*this)[4],  (*this)[8], (*this)[12],
                (*this)[1], (*this)[5],  (*this)[9], (*this)[13],
                (*this)[2], (*this)[6], (*this)[10], (*this)[14],
                (*this)[3], (*this)[7], (*this)[11], (*this)[15]
            };
        }
        /// @brief get sub matrix of given index 
        mat3 cellSubMatrix( usize row, usize col ) const {
            mat3 result = {};
            usize i = 0;
            for( usize c = 0; c < 4; c++ ) {
                if( c == col ) {
                    continue;
                }
                f32 column[4];
                getColumn( c, column );

                for( usize r = 0; r < 4; r++ ) {
                    if( r != row ) {
                        result[i] = column[r];
                    }
                    i++;
                }
            }
            return result;
        }
        /// @brief get minor of given index 
        f32 cellMinor( usize row, usize col ) const {
            return cellSubMatrix( row, col ).determinant();
        }
        /// @brief get cofactor of given index 
        f32 cellCofactor( usize row, usize col ) const {
            f32 minor = cellMinor( row, col );
            return pow( -1.0f, ( (f32)row + 1.0f ) + ( (f32)col + 1.0f ) ) * minor;
        }
        /// @brief get cofactor of matrix
        mat4 cofactor() const {
            mat4 result = {};
            forloop(16) {
                usize row = i / 4;
                usize col = i % 4;
                result[i] = cellCofactor( row, col );
            }
            return result;
        }
        /// @brief get adjoint of matrix 
        mat4 adjoint() const {
            return cofactor().transpose();
        }
        /// @brief get inverse matrix
        /// @param result inverse matrix
        /// @return true if inverse is possible
        bool inverse( mat4* result ) const {
            f32 d = determinant();
            if( d == 0.0f ) {
                return false;
            } else {
                *result = adjoint() / d;
                return true;
            }
        }

        /// @brief swap columns with rows
        void transpose_self() {
            *this = transpose();
        }

        /// @brief get column
        /// @param column column index [0-3]
        /// @param buffer buffer to store values
        /// IMPORTANT: buffer must be able to hold at least 3 values!
        void getColumn( usize column, f32* buffer ) const {
            forloop(4) {
                buffer[i] = (*this)[column + (i * 4)];
            }
        }
        /// @brief get row
        /// @param row row index [0-3]
        /// @param buffer buffer to store values
        /// IMPORTANT: buffer must be able to hold at least 3 values!
        void getRow( usize row, f32* buffer ) const {
            forloop(4) {
                buffer[i] = (*this)[(row * 4) + i];
            }
        }

        /// @brief to mat3 
        mat3 toMat3() const {
            return {
                (*this)[0], (*this)[1], (*this)[2],//3
                (*this)[4], (*this)[5], (*this)[6],//7
                (*this)[8], (*this)[9], (*this)[10]
            };
        }

        f32& operator[]( i32 index ) {
            return valuePtr()[index];
        }
        f32 operator[]( i32 index ) const {
            return valuePtr()[index];
        }

        // TODO(alicia): SIMD!

        mat4& operator+=( const mat4& rhs ) {
            (*this)[0]  += rhs[0];
            (*this)[1]  += rhs[1];
            (*this)[2]  += rhs[2];
            (*this)[3]  += rhs[3];
            (*this)[4]  += rhs[4];
            (*this)[5]  += rhs[5];
            (*this)[6]  += rhs[6];
            (*this)[7]  += rhs[7];
            (*this)[8]  += rhs[8];
            (*this)[9]  += rhs[9];
            (*this)[10] += rhs[10];
            (*this)[11] += rhs[11];
            (*this)[12] += rhs[12];
            (*this)[13] += rhs[13];
            (*this)[14] += rhs[14];
            (*this)[15] += rhs[15];
            return *this;
        }
        mat4& operator-=( const mat4& rhs ) {
            (*this)[0]  -= rhs[0];
            (*this)[1]  -= rhs[1];
            (*this)[2]  -= rhs[2];
            (*this)[3]  -= rhs[3];
            (*this)[4]  -= rhs[4];
            (*this)[5]  -= rhs[5];
            (*this)[6]  -= rhs[6];
            (*this)[7]  -= rhs[7];
            (*this)[8]  -= rhs[8];
            (*this)[9]  -= rhs[9];
            (*this)[10] -= rhs[10];
            (*this)[11] -= rhs[11];
            (*this)[12] -= rhs[12];
            (*this)[13] -= rhs[13];
            (*this)[14] -= rhs[14];
            (*this)[15] -= rhs[15];
            return *this;
        }
        mat4& operator*=( const f32& rhs ) {
            (*this)[0]  *= rhs;
            (*this)[1]  *= rhs;
            (*this)[2]  *= rhs;
            (*this)[3]  *= rhs;
            (*this)[4]  *= rhs;
            (*this)[5]  *= rhs;
            (*this)[6]  *= rhs;
            (*this)[7]  *= rhs;
            (*this)[8]  *= rhs;
            (*this)[9]  *= rhs;
            (*this)[10] *= rhs;
            (*this)[11] *= rhs;
            (*this)[12] *= rhs;
            (*this)[13] *= rhs;
            (*this)[14] *= rhs;
            (*this)[15] *= rhs;
            return *this;
        }
        mat4& operator/=( const f32& rhs ) {
            (*this)[0]  /= rhs;
            (*this)[1]  /= rhs;
            (*this)[2]  /= rhs;
            (*this)[3]  /= rhs;
            (*this)[4]  /= rhs;
            (*this)[5]  /= rhs;
            (*this)[6]  /= rhs;
            (*this)[7]  /= rhs;
            (*this)[8]  /= rhs;
            (*this)[9]  /= rhs;
            (*this)[10] /= rhs;
            (*this)[11] /= rhs;
            (*this)[12] /= rhs;
            (*this)[13] /= rhs;
            (*this)[14] /= rhs;
            (*this)[15] /= rhs;
            return *this;
        }

        /// @brief Get pointer to struct as f32
        f32* valuePtr() const { return (f32*)this; }
        static mat4 zero() { return {}; }
        static mat4 identity() {
            mat4 result = {};
            result[0]  = 1.0f;
            result[5]  = 1.0f;
            result[10] = 1.0f;
            result[15] = 1.0f;
            return result;
        }
        static mat4 lookAt( const vec3& position, const vec3& target, const vec3& up ) {
            vec3 zaxis = ( target - position ).normal();
            vec3 xaxis = vec3::cross( zaxis, up ).normal();
            vec3 yaxis = vec3::cross( xaxis, zaxis );
            zaxis = -zaxis;
            return {
                xaxis.x, yaxis.x, zaxis.x, 0.0f,
                xaxis.y, yaxis.y, zaxis.y, 0.0f,
                xaxis.z, yaxis.z, zaxis.z, 0.0f,

                -vec3::dot( xaxis, position ),
                -vec3::dot( yaxis, position ),
                -vec3::dot( zaxis, position ),
                1.0f
            };
        }
        static mat4 ortho( f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far ) {
            mat4 result = mat4::identity();
            result[ 0] =  2.0f / ( right - left );
            result[ 5] =  2.0f / ( top - bottom );
            result[10] = -2.0f / ( far - near );
            result[12] = -( right + left   ) / ( right - left   );
            result[13] = -( top   + bottom ) / ( top   - bottom );
            result[14] = -( far   + near   ) / ( far   - near   );
            return result;
        }
        static mat4 ortho( f32 left, f32 right, f32 bottom, f32 top ) {
            return mat4::ortho( left, right, bottom, top, -1.0f, 1.0f );
        }
        static mat4 perspective( f32 fov, f32 aspect, f32 near, f32 far ) {
            mat4 result = mat4::zero();
            // TODO(alicia): tanf!
            result[ 0] = 1.0f / tanf( aspect * ( fov / 2.0f ) );
            result[ 5] = 1.0f / tanf( fov / 2.0f );
            result[10] = -(( far + near ) / ( far - near ));
            result[11] = -1.0f;
            result[14] = -(( 2.0f * far * near ) / ( far - near ));
            return result;
        }
        static mat4 translate( const vec3& translation ) {
            mat4 result = mat4::identity();
            result[12] = translation.x;
            result[13] = translation.y;
            result[14] = translation.z;
            return result;
        }
        static mat4 rotation( const quat& rotation ) {
            // TODO(alicia): SIMD!
            mat4 result = mat4::identity();
            result[0]   = 1.0f - ( 2.0f * sqrf( rotation.y ) ) - ( 2.0f * sqrf( rotation.z ) );
            result[1]   = ( 2.0f * ( rotation.x * rotation.y ) ) + ( 2.0f * ( rotation.w * rotation.z ) );
            result[2]   = ( 2.0f * ( rotation.x * rotation.z ) ) - ( 2.0f * ( rotation.w * rotation.y ) );
            result[4]   = ( 2.0f * ( rotation.x * rotation.y ) ) - ( 2.0f * ( rotation.w * rotation.z ) );
            result[5]   = 1.0f - ( 2.0f * sqrf( rotation.x ) ) - ( 2.0f * sqrf( rotation.z ) );
            result[6]   = ( 2.0f * ( rotation.y * rotation.z ) ) + ( 2.0f * ( rotation.w * rotation.x ) );
            result[8]   = ( 2.0f * ( rotation.x * rotation.z ) ) + ( 2.0f * ( rotation.w * rotation.y ) );
            result[9]   = ( 2.0f * ( rotation.y * rotation.z ) ) - ( 2.0f * ( rotation.w * rotation.x ) );
            result[10]  = 1.0f - ( 2.0f * sqrf( rotation.x ) ) - ( 2.0f * sqrf( rotation.y ) );
            return result;
        }
        static mat4 scale( const vec3& scale ) {
            mat4 result = mat4::identity();
            result[0]   = scale.x;
            result[5]   = scale.y;
            result[10]  = scale.z;
            return result;
        }
        static mat4 transform( const vec3& translation, const quat& rotation, const vec3& scale ) {
            return mat4::translate( translation ) * mat4::rotation( rotation ) * mat4::scale( scale );
        }
    };
    inline mat4 operator+( const mat4& lhs, const mat4& rhs ) {
        return mat4(lhs) += rhs;
    }
    inline mat4 operator-( const mat4& lhs, const mat4& rhs ) {
        return mat4(lhs) -= rhs;
    }
    inline mat4 operator*( const mat4& lhs, const f32& rhs ) {
        return mat4(lhs) *= rhs;
    }
    inline mat4 operator*( const f32& lhs, const mat4& rhs ) {
        return mat4(rhs) *= lhs;
    }
    inline mat4 operator/( const mat4& lhs, const f32& rhs ) {
        return mat4(lhs) /= rhs;
    }
    // TODO(alicia): SIMD!

    inline mat4 operator*( const mat4& lhs, const mat4& rhs ) {
        return {
            // column - 0
            ( lhs[0] *  rhs[0] ) + ( lhs[4] *  rhs[1] ) + ( lhs[8]  * rhs[2] ) + ( lhs[12] * rhs[3] ),
            ( lhs[1] *  rhs[0] ) + ( lhs[5] *  rhs[1] ) + ( lhs[9]  * rhs[2] ) + ( lhs[13] * rhs[3] ),
            ( lhs[2] *  rhs[0] ) + ( lhs[6] *  rhs[1] ) + ( lhs[10] * rhs[2] ) + ( lhs[14] * rhs[3] ),
            ( lhs[3] *  rhs[0] ) + ( lhs[7] *  rhs[1] ) + ( lhs[11] * rhs[2] ) + ( lhs[15] * rhs[3] ),
            // column - 1
            ( lhs[0] *  rhs[4] ) + ( lhs[4] *  rhs[5] ) + ( lhs[8]  * rhs[6] ) + ( lhs[12] * rhs[7] ),
            ( lhs[1] *  rhs[4] ) + ( lhs[5] *  rhs[5] ) + ( lhs[9]  * rhs[6] ) + ( lhs[13] * rhs[7] ),
            ( lhs[2] *  rhs[4] ) + ( lhs[6] *  rhs[5] ) + ( lhs[10] * rhs[6] ) + ( lhs[14] * rhs[7] ),
            ( lhs[3] *  rhs[4] ) + ( lhs[7] *  rhs[5] ) + ( lhs[11] * rhs[6] ) + ( lhs[15] * rhs[7] ),
            // column - 2
            ( lhs[0] *  rhs[8] ) + ( lhs[4] *  rhs[9] ) + ( lhs[8]  * rhs[10] ) + ( lhs[12] * rhs[11] ),
            ( lhs[1] *  rhs[8] ) + ( lhs[5] *  rhs[9] ) + ( lhs[9]  * rhs[10] ) + ( lhs[13] * rhs[11] ),
            ( lhs[2] *  rhs[8] ) + ( lhs[6] *  rhs[9] ) + ( lhs[10] * rhs[10] ) + ( lhs[14] * rhs[11] ),
            ( lhs[3] *  rhs[8] ) + ( lhs[7] *  rhs[9] ) + ( lhs[11] * rhs[10] ) + ( lhs[15] * rhs[11] ),
            // column - 3
            ( lhs[0] *  rhs[12] ) + ( lhs[4] *  rhs[13] ) + ( lhs[8]  * rhs[14] ) + ( lhs[12] * rhs[15] ),
            ( lhs[1] *  rhs[12] ) + ( lhs[5] *  rhs[13] ) + ( lhs[9]  * rhs[14] ) + ( lhs[13] * rhs[15] ),
            ( lhs[2] *  rhs[12] ) + ( lhs[6] *  rhs[13] ) + ( lhs[10] * rhs[14] ) + ( lhs[14] * rhs[15] ),
            ( lhs[3] *  rhs[12] ) + ( lhs[7] *  rhs[13] ) + ( lhs[11] * rhs[14] ) + ( lhs[15] * rhs[15] ),
        };
    }
    inline vec4 operator*( const mat4& lhs, const vec4& rhs ) {
        return {
            ( lhs[0] * rhs.x ) + ( lhs[4] * rhs.y ) + ( lhs[8]  * rhs.z ) + ( lhs[12] * rhs.w ),
            ( lhs[1] * rhs.x ) + ( lhs[5] * rhs.y ) + ( lhs[9]  * rhs.z ) + ( lhs[13] * rhs.w ),
            ( lhs[2] * rhs.x ) + ( lhs[6] * rhs.y ) + ( lhs[10] * rhs.z ) + ( lhs[14] * rhs.w ),
            ( lhs[3] * rhs.x ) + ( lhs[7] * rhs.y ) + ( lhs[11] * rhs.z ) + ( lhs[15] * rhs.w ),
        };
    }
    inline vec3 operator*( const mat4& lhs, const vec3& rhs ) {
        return {
            ( lhs[0] * rhs.x ) + ( lhs[4] * rhs.y ) + ( lhs[8]  * rhs.z ) + lhs[12],
            ( lhs[1] * rhs.x ) + ( lhs[5] * rhs.y ) + ( lhs[9]  * rhs.z ) + lhs[13],
            ( lhs[2] * rhs.x ) + ( lhs[6] * rhs.y ) + ( lhs[10] * rhs.z ) + lhs[14]
        };
    }

} // namespace smath

