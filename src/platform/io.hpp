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

/// @brief Get system time 
u64 GetSystemTime();

/// @brief Allocate memory in the heap
/// @param size amount to alloc
/// @return Pointer to memory
void* Alloc( usize size );

/// @brief Free memory in the heap
/// @param mem pointer to memory
void Free( void* mem );

// NOTE(alicia): MemCopy isn't really platform specific but it makes sense to put it in the same
// header as Alloc and Free

/// @brief Copy memory from source to destination
/// @param size size of source buffer, destination buffer must be greater than or equals to size of source buffer
/// @param src source buffer
/// @param dst destination buffer
inline void MemCopy( usize size, const void* src, void* dst ) {
    // size_t aligned
    if( size % sizeof(usize) == 0 ) {
        usize* srcBytes  = (usize*)src;
        usize* dstBytes  = (usize*)dst;
        usize iterations = size / sizeof(usize);
        ucycles( iterations ) {
            dstBytes[i] = srcBytes[i];
        }
        return;
    }

    // not-size_t aligned
    u8* srcBytes = (u8*)src;
    u8* dstBytes = (u8*)dst;
    ucycles( size ) {
        dstBytes[i] = srcBytes[i];
    }
}

// TODO(alicia): optimize memcopy range

inline void MemCopyRanges(
    usize srcRangeBegin, usize srcRangeEnd,
    void* src,
    usize dstRangeBegin, usize dstRangeEnd,
    void* dst
) {
    usize srcRangeLen = srcRangeEnd - srcRangeBegin;
    usize dstRangeLen = dstRangeEnd - dstRangeBegin;
    DEBUG_ASSERT_LOG( srcRangeLen == dstRangeLen,
        "MemCopyRanges > srcRangeLen must equal dstRangeLen! src: %llu dst: %llu",
        srcRangeLen, dstRangeLen
    );

    u8* srcBytes = (u8*)src + srcRangeBegin;
    u8* dstBytes = (u8*)dst + dstRangeBegin;
    ucycles( dstRangeLen ) {
        dstBytes[i] = srcBytes[i];
    }
}

struct File {
    void* data;
    usize size;
    char* filePath;
    usize filePathLen;
};

/// @brief Load file from disk
/// @param filePath path to file
/// @param result file
/// @return true if successful
bool LoadFile( const char* filePath, File* result );

/// @brief Load file from popup menu
/// @param result file
/// @return true if successful
bool UserLoadFile( const char* dialogTitle, File* result );

/// @brief Free file memory
/// @param file file to free
void FreeFile( File* file );

enum class WriteFileType {
    // Fail if file does not already exist
    NOT_CREATED_FAIL,
    // Create file if does not already exist
    CREATE,
};

/// @brief Write file to disk
/// @param filePath path to file
/// @param buffer buffer to write from
/// @param bufferSize size of buffer
/// @param writeType type of write
/// @return true if successful
bool WriteFile( const char* filePath, void* buffer, usize bufferSize, WriteFileType writeType );

/// @brief Cursor styles
enum class CursorStyle {
    ARROW,
    HAND,
    WAIT,
    BEAM,
    NOT_ALLOWED,
    RESIZE_RIGHT,
    RESIZE_LEFT,
    RESIZE_VERTICAL,
    RESIZE_HORIZONTAL
};

/// @brief Set cursor style
void SetCursorStyle( CursorStyle style );
/// @brief Set cursor style to whatever the platform layer thinks it should be
void ResetCursorStyle();
/// @brief Get current cursor style
CursorStyle GetCursorStyle();

/// @brief Set cursor visibility
void SetCursorVisibility( bool visible );
/// @brief Check if cursor is visible 
bool IsCursorVisible();

/// @brief Lock or unlock cursor position
void SetCursorLocked( bool lock );
/// @brief Check if cursor is locked 
bool IsCursorLocked();

/// @brief Check if app is active 
bool IsAppActive();

/// @brief Platform independent keycodes
enum class KeyCode {
    UNKNOWN   = 0,
    ESCAPE    ,
    BACKSPACE ,
    TAB       ,
    ENTER     ,
    SPACE     ,
    QUOTE     ,
    PLUS      ,
    COMMA     ,
    MINUS     ,
    PERIOD    ,
    KEY_0     ,
    KEY_1     ,
    KEY_2     ,
    KEY_3     ,
    KEY_4     ,
    KEY_5     ,
    KEY_6     ,
    KEY_7     ,
    KEY_8     ,
    KEY_9     ,
    COLON     ,
    ANGLE     ,
    QUESTION  ,
    LBRACKET  ,
    RBRACKET  ,
    TILDE     ,
    A         ,
    B         ,
    C         ,
    D         ,
    E         ,
    F         ,
    G         ,
    H         ,
    I         ,
    J         ,
    K         ,
    L         ,
    M         ,
    N         ,
    O         ,
    P         ,
    Q         ,
    R         ,
    S         ,
    T         ,
    U         ,
    V         ,
    W         ,
    X         ,
    Y         ,
    Z         ,
    KEY_DELETE,
    CAPSLOCK  ,
    F1        ,
    F2        ,
    F3        ,
    F4        ,
    F5        ,
    F6        ,
    F7        ,
    F8        ,
    F9        ,
    F10       ,
    F11       ,
    F12       ,
    F13       ,
    F14       ,
    F15       ,
    F16       ,
    F17       ,
    F18       ,
    F19       ,
    F20       ,
    F21       ,
    F22       ,
    F23       ,
    F24       ,
    PRINT     ,
    SCRLK     ,
    PAUSE     ,
    INSERT    ,
    HOME      ,
    PGUP      ,
    END       ,
    PGDWN     ,
    RIGHT     ,
    LEFT      ,
    DOWN      ,
    UP        ,
    NUMLOCK   ,
    DIVIDE    ,
    MULTIPLY  ,
    SUBTRACT  ,
    ADD       ,
    PAD_0     ,
    PAD_1     ,
    PAD_2     ,
    PAD_3     ,
    PAD_4     ,
    PAD_5     ,
    PAD_6     ,
    PAD_7     ,
    PAD_8     ,
    PAD_9     ,
    LSUPER    ,
    RSUPER    ,
    CTRL      ,
    SHIFT     ,
    ALT       ,

    count
};
inline const usize KEY_COUNT = (usize)KeyCode::count;
inline const char* KeyCodeToString( KeyCode key ) {
    switch(key) {
        case KeyCode::ESCAPE    : return "ESCAPE";
        case KeyCode::BACKSPACE : return "BACKSPACE";
        case KeyCode::TAB       : return "TAB";
        case KeyCode::ENTER     : return "ENTER";
        case KeyCode::SPACE     : return "SPACE";
        case KeyCode::QUOTE     : return "QUOTE";
        case KeyCode::PLUS      : return "PLUS";
        case KeyCode::COMMA     : return "COMMA";
        case KeyCode::MINUS     : return "MINUS";
        case KeyCode::PERIOD    : return "PERIOD";
        case KeyCode::KEY_0     : return "KEY_0";
        case KeyCode::KEY_1     : return "KEY_1";
        case KeyCode::KEY_2     : return "KEY_2";
        case KeyCode::KEY_3     : return "KEY_3";
        case KeyCode::KEY_4     : return "KEY_4";
        case KeyCode::KEY_5     : return "KEY_5";
        case KeyCode::KEY_6     : return "KEY_6";
        case KeyCode::KEY_7     : return "KEY_7";
        case KeyCode::KEY_8     : return "KEY_8";
        case KeyCode::KEY_9     : return "KEY_9";
        case KeyCode::COLON     : return "COLON";
        case KeyCode::ANGLE     : return "ANGLE";
        case KeyCode::QUESTION  : return "QUESTION";
        case KeyCode::LBRACKET  : return "LBRACKET";
        case KeyCode::RBRACKET  : return "RBRACKET";
        case KeyCode::TILDE     : return "TILDE";
        case KeyCode::A         : return "A";
        case KeyCode::B         : return "B";
        case KeyCode::C         : return "C";
        case KeyCode::D         : return "D";
        case KeyCode::E         : return "E";
        case KeyCode::F         : return "F";
        case KeyCode::G         : return "G";
        case KeyCode::H         : return "H";
        case KeyCode::I         : return "I";
        case KeyCode::J         : return "J";
        case KeyCode::K         : return "K";
        case KeyCode::L         : return "L";
        case KeyCode::M         : return "M";
        case KeyCode::N         : return "N";
        case KeyCode::O         : return "O";
        case KeyCode::P         : return "P";
        case KeyCode::Q         : return "Q";
        case KeyCode::R         : return "R";
        case KeyCode::S         : return "S";
        case KeyCode::T         : return "T";
        case KeyCode::U         : return "U";
        case KeyCode::V         : return "V";
        case KeyCode::W         : return "W";
        case KeyCode::X         : return "X";
        case KeyCode::Y         : return "Y";
        case KeyCode::Z         : return "Z";
        case KeyCode::KEY_DELETE: return "DELETE";
        case KeyCode::CAPSLOCK  : return "CAPSLOCK";
        case KeyCode::F1        : return "F1";
        case KeyCode::F2        : return "F2";
        case KeyCode::F3        : return "F3";
        case KeyCode::F4        : return "F4";
        case KeyCode::F5        : return "F5";
        case KeyCode::F6        : return "F6";
        case KeyCode::F7        : return "F7";
        case KeyCode::F8        : return "F8";
        case KeyCode::F9        : return "F9";
        case KeyCode::F10       : return "F10";
        case KeyCode::F11       : return "F11";
        case KeyCode::F12       : return "F12";
        case KeyCode::F13       : return "F13";
        case KeyCode::F14       : return "F14";
        case KeyCode::F15       : return "F15";
        case KeyCode::F16       : return "F16";
        case KeyCode::F17       : return "F17";
        case KeyCode::F18       : return "F18";
        case KeyCode::F19       : return "F19";
        case KeyCode::F20       : return "F20";
        case KeyCode::F21       : return "F21";
        case KeyCode::F22       : return "F22";
        case KeyCode::F23       : return "F23";
        case KeyCode::F24       : return "F24";
        case KeyCode::PRINT     : return "PRINT";
        case KeyCode::SCRLK     : return "SCROLL_LOCK";
        case KeyCode::PAUSE     : return "PAUSE";
        case KeyCode::INSERT    : return "INSERT";
        case KeyCode::HOME      : return "HOME";
        case KeyCode::PGUP      : return "PAGE_UP";
        case KeyCode::END       : return "END";
        case KeyCode::PGDWN     : return "PAGE_DOWN";
        case KeyCode::RIGHT     : return "RIGHT";
        case KeyCode::LEFT      : return "LEFT";
        case KeyCode::DOWN      : return "DOWN";
        case KeyCode::UP        : return "UP";
        case KeyCode::NUMLOCK   : return "NUMLOCK";
        case KeyCode::DIVIDE    : return "DIVIDE";
        case KeyCode::MULTIPLY  : return "MULTIPLY";
        case KeyCode::SUBTRACT  : return "SUBTRACT";
        case KeyCode::ADD       : return "ADD";
        case KeyCode::PAD_0     : return "PAD_0";
        case KeyCode::PAD_1     : return "PAD_1";
        case KeyCode::PAD_2     : return "PAD_2";
        case KeyCode::PAD_3     : return "PAD_3";
        case KeyCode::PAD_4     : return "PAD_4";
        case KeyCode::PAD_5     : return "PAD_5";
        case KeyCode::PAD_6     : return "PAD_6";
        case KeyCode::PAD_7     : return "PAD_7";
        case KeyCode::PAD_8     : return "PAD_8";
        case KeyCode::PAD_9     : return "PAD_9";
        case KeyCode::LSUPER    : return "LSUPER";
        case KeyCode::RSUPER    : return "RSUPER";
        case KeyCode::CTRL      : return "CTRL";
        case KeyCode::SHIFT     : return "SHIFT";
        case KeyCode::ALT       : return "ALT";
        default: return "UNKNOWN";
    }
}

} // namespace Platform
