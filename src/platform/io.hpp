/**
 * Description:  Input-Output Platform Functions
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 15, 2022 
 */
#pragma once
#include "pch.hpp"

namespace Platform {

/// @brief Append string to title
/// @param append string to append
void AppendToWindowTitle( const char* append, usize appendLen );

/// @brief Allocate memory in the heap
/// @param size amount to alloc
/// @return Pointer to memory
void* Alloc( usize size );

/// @brief Free memory in the heap
/// @param mem pointer to memory
void Free( void* mem );

} // namespace Platform

