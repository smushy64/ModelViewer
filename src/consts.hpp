#pragma once
#include "alias.hpp"

const i32 DEFAULT_WINDOW_WIDTH  = 1280;
const i32 DEFAULT_WINDOW_HEIGHT = 720;

const u32 U32_MAX = 0xFFFFFFFF;

inline const char* PROGRAM_TITLE = "Model Viewer | Version 0.1";

inline const usize MATRIX_2D_BINDING_POINT   = 0;
inline const usize MATRIX_3D_BINDING_POINT   = 1;
inline const usize LIGHTS_BINDING_POINT      = 2;
inline const usize SHARED_DATA_BINDING_POINT = 3;

inline const usize SHARED_DATA_SIZE = 24;

inline const usize MAX_POINT_LIGHTS = 4;
