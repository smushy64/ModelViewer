#include "mousecode.hpp"

const char* Core::MouseCodeToString( MouseCode mouseCode ) {
switch(mouseCode) {
    case MouseCode::LEFT  : return "LEFT";
    case MouseCode::RIGHT : return "RIGHT";
    case MouseCode::MIDDLE: return "MIDDLE";
    case MouseCode::EXTRA1: return "EXTRA1";
    case MouseCode::EXTRA2: return "EXTRA2";
    default: return "UNKNOWN";
}
}
