#pragma once

namespace Platform {
    
enum class PointerStyle {
    ARROW,
    HAND,
    TEXT,
    WAIT,
    SIZE_ALL,
    SIZE_TR_BL,
    SIZE_TL_BR,
    SIZE_T_B,
    SIZE_L_R,
    NOT_ALLOWED
};

void SetPointerStyle( PointerStyle pointerStyle );
// Set Pointer Style to style desired by platform
void ResetPointerStyle();
PointerStyle CurrentPointerStyle();

bool IsPointerVisible();
void ShowPointer();
void HidePointer();

} // namespace Platform

