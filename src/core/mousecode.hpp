#pragma once

namespace Core {

enum class MouseCode {
    UNKNOWN = 0,
    LEFT  ,
    RIGHT ,
    MIDDLE,
    EXTRA1,
    EXTRA2,
    
    count
};

const char* MouseCodeToString( MouseCode mouseCode );

}; // namespace Core
