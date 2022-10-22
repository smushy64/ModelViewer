#include "keycode.hpp"

Core::KeyCode Core::WinKeyToKeyCode( u32 winKey ) {
// these are VK codes in hex
switch( winKey ) {
    case 0x1B: return KeyCode::ESCAPE;
    case 0x08: return KeyCode::BACKSPACE;
    case 0x09: return KeyCode::TAB;
    case 0x0D: return KeyCode::ENTER;
    case 0x20: return KeyCode::SPACE;
    case 0xDE: return KeyCode::QUOTE;
    case 0xBB: return KeyCode::PLUS;
    case 0xBC: return KeyCode::COMMA;
    case 0xBD: return KeyCode::MINUS;
    case 0xBE: return KeyCode::PERIOD;
    case 0x30: return KeyCode::KEY_0;
    case 0x31: return KeyCode::KEY_1;
    case 0x32: return KeyCode::KEY_2;
    case 0x33: return KeyCode::KEY_3;
    case 0x34: return KeyCode::KEY_4;
    case 0x35: return KeyCode::KEY_5;
    case 0x36: return KeyCode::KEY_6;
    case 0x37: return KeyCode::KEY_7;
    case 0x38: return KeyCode::KEY_8;
    case 0x39: return KeyCode::KEY_9;
    case 0xBA: return KeyCode::COLON;
    case 0xE2: return KeyCode::ANGLE;
    case 0xBF: return KeyCode::QUESTION;
    case 0xDB: return KeyCode::LBRACKET;
    case 0xDD: return KeyCode::RBRACKET;
    case 0xC0: return KeyCode::TILDE;
    case 0x41: return KeyCode::A;
    case 0x42: return KeyCode::B;
    case 0x43: return KeyCode::C;
    case 0x44: return KeyCode::D;
    case 0x45: return KeyCode::E;
    case 0x46: return KeyCode::F;
    case 0x47: return KeyCode::G;
    case 0x48: return KeyCode::H;
    case 0x49: return KeyCode::I;
    case 0x4A: return KeyCode::J;
    case 0x4B: return KeyCode::K;
    case 0x4C: return KeyCode::L;
    case 0x4D: return KeyCode::M;
    case 0x4E: return KeyCode::N;
    case 0x4F: return KeyCode::O;
    case 0x50: return KeyCode::P;
    case 0x51: return KeyCode::Q;
    case 0x52: return KeyCode::R;
    case 0x53: return KeyCode::S;
    case 0x54: return KeyCode::T;
    case 0x55: return KeyCode::U;
    case 0x56: return KeyCode::V;
    case 0x57: return KeyCode::W;
    case 0x58: return KeyCode::X;
    case 0x59: return KeyCode::Y;
    case 0x5A: return KeyCode::Z;
    case 0x2E: return KeyCode::KEY_DELETE;
    case 0x14: return KeyCode::CAPSLOCK;
    case 0x70: return KeyCode::F1;
    case 0x71: return KeyCode::F2;
    case 0x72: return KeyCode::F3;
    case 0x73: return KeyCode::F4;
    case 0x74: return KeyCode::F5;
    case 0x75: return KeyCode::F6;
    case 0x76: return KeyCode::F7;
    case 0x77: return KeyCode::F8;
    case 0x78: return KeyCode::F9;
    case 0x79: return KeyCode::F10;
    case 0x7A: return KeyCode::F11;
    case 0x7B: return KeyCode::F12;
    case 0x7C: return KeyCode::F13;
    case 0x7D: return KeyCode::F14;
    case 0x7E: return KeyCode::F15;
    case 0x7F: return KeyCode::F16;
    case 0x80: return KeyCode::F17;
    case 0x81: return KeyCode::F18;
    case 0x82: return KeyCode::F19;
    case 0x83: return KeyCode::F20;
    case 0x84: return KeyCode::F21;
    case 0x85: return KeyCode::F22;
    case 0x86: return KeyCode::F23;
    case 0x87: return KeyCode::F24;
    case 0x2A: return KeyCode::PRINT;
    case 0x91: return KeyCode::SCRLK;
    case 0x13: return KeyCode::PAUSE;
    case 0x2D: return KeyCode::INSERT;
    case 0x24: return KeyCode::HOME;
    case 0x21: return KeyCode::PGUP;
    case 0x23: return KeyCode::END;
    case 0x22: return KeyCode::PGDWN;
    case 0x27: return KeyCode::RIGHT;
    case 0x25: return KeyCode::LEFT;
    case 0x28: return KeyCode::DOWN;
    case 0x26: return KeyCode::UP;
    case 0x90: return KeyCode::NUMLOCK;
    case 0x6F: return KeyCode::DIVIDE;
    case 0x6A: return KeyCode::MULTIPLY;
    case 0x6D: return KeyCode::SUBTRACT;
    case 0x6B: return KeyCode::ADD;
    case 0x60: return KeyCode::PAD_0;
    case 0x61: return KeyCode::PAD_1;
    case 0x62: return KeyCode::PAD_2;
    case 0x63: return KeyCode::PAD_3;
    case 0x64: return KeyCode::PAD_4;
    case 0x65: return KeyCode::PAD_5;
    case 0x66: return KeyCode::PAD_6;
    case 0x67: return KeyCode::PAD_7;
    case 0x68: return KeyCode::PAD_8;
    case 0x69: return KeyCode::PAD_9;
    case 0x5B: return KeyCode::LWIN;
    case 0x5C: return KeyCode::RWIN;
    case 0x11: return KeyCode::CTRL;
    case 0x10: return KeyCode::SHIFT;
    case 0x12: return KeyCode::ALT;
    default: return KeyCode::UNKNOWN;
}
}

const char* Core::KeyCodeToString( KeyCode keycode ) {
switch( keycode ) {
    case KeyCode::ESCAPE:    return "ESCAPE";
    case KeyCode::BACKSPACE: return "BACKSPACE";
    case KeyCode::TAB:       return "TAB";
    case KeyCode::ENTER:     return "ENTER";
    case KeyCode::SPACE:     return "SPACE";
    case KeyCode::QUOTE:     return "QUOTE";
    case KeyCode::PLUS:      return "PLUS";
    case KeyCode::COMMA:     return "COMMA";
    case KeyCode::MINUS:     return "MINUS";
    case KeyCode::PERIOD:    return "PERIOD";
    case KeyCode::KEY_0:     return "KEY_0";
    case KeyCode::KEY_1:     return "KEY_1";
    case KeyCode::KEY_2:     return "KEY_2";
    case KeyCode::KEY_3:     return "KEY_3";
    case KeyCode::KEY_4:     return "KEY_4";
    case KeyCode::KEY_5:     return "KEY_5";
    case KeyCode::KEY_6:     return "KEY_6";
    case KeyCode::KEY_7:     return "KEY_7";
    case KeyCode::KEY_8:     return "KEY_8";
    case KeyCode::KEY_9:     return "KEY_9";
    case KeyCode::COLON:     return "COLON";
    case KeyCode::ANGLE:     return "ANGLE";
    case KeyCode::QUESTION:  return "QUESTION";
    case KeyCode::LBRACKET:  return "LBRACKET";
    case KeyCode::RBRACKET:  return "RBRACKET";
    case KeyCode::TILDE:     return "TILDE";
    case KeyCode::A:         return "A";
    case KeyCode::B:         return "B";
    case KeyCode::C:         return "C";
    case KeyCode::D:         return "D";
    case KeyCode::E:         return "E";
    case KeyCode::F:         return "F";
    case KeyCode::G:         return "G";
    case KeyCode::H:         return "H";
    case KeyCode::I:         return "I";
    case KeyCode::J:         return "J";
    case KeyCode::K:         return "K";
    case KeyCode::L:         return "L";
    case KeyCode::M:         return "M";
    case KeyCode::N:         return "N";
    case KeyCode::O:         return "O";
    case KeyCode::P:         return "P";
    case KeyCode::Q:         return "Q";
    case KeyCode::R:         return "R";
    case KeyCode::S:         return "S";
    case KeyCode::T:         return "T";
    case KeyCode::U:         return "U";
    case KeyCode::V:         return "V";
    case KeyCode::W:         return "W";
    case KeyCode::X:         return "X";
    case KeyCode::Y:         return "Y";
    case KeyCode::Z:         return "Z";
    case KeyCode::KEY_DELETE:return "DELETE";
    case KeyCode::CAPSLOCK:  return "CAPSLOCK";
    case KeyCode::F1:        return "F1";
    case KeyCode::F2:        return "F2";
    case KeyCode::F3:        return "F3";
    case KeyCode::F4:        return "F4";
    case KeyCode::F5:        return "F5";
    case KeyCode::F6:        return "F6";
    case KeyCode::F7:        return "F7";
    case KeyCode::F8:        return "F8";
    case KeyCode::F9:        return "F9";
    case KeyCode::F10:       return "F10";
    case KeyCode::F11:       return "F11";
    case KeyCode::F12:       return "F12";
    case KeyCode::F13:       return "F13";
    case KeyCode::F14:       return "F14";
    case KeyCode::F15:       return "F15";
    case KeyCode::F16:       return "F16";
    case KeyCode::F17:       return "F17";
    case KeyCode::F18:       return "F18";
    case KeyCode::F19:       return "F19";
    case KeyCode::F20:       return "F20";
    case KeyCode::F21:       return "F21";
    case KeyCode::F22:       return "F22";
    case KeyCode::F23:       return "F23";
    case KeyCode::F24:       return "F24";
    case KeyCode::PRINT:     return "PRINT";
    case KeyCode::SCRLK:     return "SCRLK";
    case KeyCode::PAUSE:     return "PAUSE";
    case KeyCode::INSERT:    return "INSERT";
    case KeyCode::HOME:      return "HOME";
    case KeyCode::PGUP:      return "PGUP";
    case KeyCode::END:       return "END";
    case KeyCode::PGDWN:     return "PGDWN";
    case KeyCode::RIGHT:     return "RIGHT";
    case KeyCode::LEFT:      return "LEFT";
    case KeyCode::DOWN:      return "DOWN";
    case KeyCode::UP:        return "UP";
    case KeyCode::NUMLOCK:   return "NUMLOCK";
    case KeyCode::DIVIDE:    return "DIVIDE";
    case KeyCode::MULTIPLY:  return "MULTIPLY";
    case KeyCode::SUBTRACT:  return "SUBTRACT";
    case KeyCode::ADD:       return "ADD";
    case KeyCode::PAD_0:     return "PAD_0";
    case KeyCode::PAD_1:     return "PAD_1";
    case KeyCode::PAD_2:     return "PAD_2";
    case KeyCode::PAD_3:     return "PAD_3";
    case KeyCode::PAD_4:     return "PAD_4";
    case KeyCode::PAD_5:     return "PAD_5";
    case KeyCode::PAD_6:     return "PAD_6";
    case KeyCode::PAD_7:     return "PAD_7";
    case KeyCode::PAD_8:     return "PAD_8";
    case KeyCode::PAD_9:     return "PAD_9";
    case KeyCode::LWIN:      return "LWIN";
    case KeyCode::RWIN:      return "RWIN";
    case KeyCode::CTRL:      return "CTRL";
    case KeyCode::SHIFT:     return "SHIFT";
    case KeyCode::ALT:       return "ALT";
    default: return "UNKNOWN";
}
}