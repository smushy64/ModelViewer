#pragma once
#include "alias.hpp"
#include "debug.hpp"
#include "keycode.hpp"
#include "mousecode.hpp"
#include <glm/vec2.hpp>
#include <queue>

namespace Core {

class Event {
public:
    enum class Type {
        UNKNOWN, //
        FOCUS_GAINED,  // done
        FOCUS_LOST,    // done
        WINDOW_MOVE,   // done
        WINDOW_RESIZE, // done
        QUIT,          // done

        KEY_DOWN, // done
        KEY_UP,   // done

        MOUSE_DOWN, // done
        MOUSE_UP,   // done

        MOUSE_POSITION, // done
        MOUSE_SCROLL,   // done
    };

    Type GetType()   const { return m_type; }
    bool IsHandled() const { return m_handled; }
    void EventHandled() { m_handled = true; }

public: // NOTE: virtual
    virtual void PrintEvent() = 0;
    virtual ~Event() = default;

protected:
    Type m_type    = Type::UNKNOWN;
    bool m_handled = false;

}; // class Event

class EventQueue {
public:
    EventQueue() {}
    ~EventQueue();

    // Push new event
    void   Push( Event* event );
    // Get front event, it must be deleted after use
    Event* Pop();
    // Get number of events in queue
    usize Count() const { return m_events.size(); }
    // Test if queue is empty
    bool IsEmpty() const { return m_events.empty() && m_resizeEvent == nullptr && m_moveEvent == nullptr; }
private:
    Event* m_resizeEvent;
    Event* m_moveEvent;
    std::queue<Event*> m_events;
}; // class Event Queue

// NOTE: KEYDOWN

class KeyDown : public Event {
public:
    KeyCode GetKeycode() const { return m_keycode; }
    KeyDown( KeyCode keycode ) : m_keycode(keycode) { m_type = Type::KEY_DOWN; }
    virtual void PrintEvent() override { LOG_DEBUG("KeyDown Event: %s", Core::KeyCodeToString(m_keycode)); }
private:
    KeyCode m_keycode;
}; // Class KeyDown

// NOTE: KEYUP

class KeyUp : public Event {
public:
    KeyCode GetKeycode() const { return m_keycode; }
    KeyUp( KeyCode keycode ) : m_keycode(keycode) { m_type = Type::KEY_UP; }
    virtual void PrintEvent() override { LOG_DEBUG("KeyUp Event: %s", Core::KeyCodeToString(m_keycode)); }
private:
    KeyCode m_keycode;
}; // Class KeyUp

// NOTE: MOUSE DOWN

class MouseDown : public Event {
public:
    MouseCode GetMouseCode() const { return m_mouseCode; }
    MouseDown( MouseCode mouseCode ) : m_mouseCode(mouseCode) { m_type = Type::MOUSE_DOWN; }
    virtual void PrintEvent() override { LOG_DEBUG("Mouse Down Event: %s", Core::MouseCodeToString(m_mouseCode)); }
private:
    MouseCode m_mouseCode;
}; // Class MouseDown

// NOTE: MOUSE UP

class MouseUp : public Event {
public:
    MouseCode GetMouseCode() const { return m_mouseCode; }
    MouseUp( MouseCode mouseCode ) : m_mouseCode(mouseCode) { m_type = Type::MOUSE_UP; }
    virtual void PrintEvent() override { LOG_DEBUG("Mouse Up Event: %s", Core::MouseCodeToString(m_mouseCode)); }
private:
    MouseCode m_mouseCode;
}; // Class MouseUp

// NOTE: MOUSE POSITION

class MousePositionEvent : public Event {
public:
    glm::vec2  GetScreenPosition() const { return m_mouseScreen; }
    glm::ivec2 GetPixelPosition()  const { return m_mousePixel; }
    MousePositionEvent( const glm::ivec2& pixelPosition, const glm::vec2& screenPosition )
        : m_mouseScreen( screenPosition ), m_mousePixel( pixelPosition ) {
        m_type = Type::MOUSE_POSITION;
    }
    virtual void PrintEvent() override {
        LOG_DEBUG("Mouse Position Event: (%.1f, %.1f) (%i, %i)",
            m_mouseScreen.x, m_mouseScreen.y, m_mousePixel.x, m_mousePixel.y
        );
    }
private:
    glm::vec2  m_mouseScreen;
    glm::ivec2 m_mousePixel;
};

// NOTE: MOUSE SCROLL

class MouseScroll : public Event {
public:
    i32 GetMouseScrollDirection() const { return m_mouseScrollDirection; }
    MouseScroll( i32 mouseScroll ) : m_mouseScrollDirection(mouseScroll) { m_type = Type::MOUSE_SCROLL; }
    virtual void PrintEvent() override { LOG_DEBUG( "Mouse Scroll Event: %i", m_mouseScrollDirection ); }
private:
    i32 m_mouseScrollDirection;
};

// NOTE: QUIT

class QuitEvent : public Event {
public:
    QuitEvent() { m_type = Type::QUIT; }
    virtual void PrintEvent() override { LOG_DEBUG("Quit Event"); }
}; // Class Quit

// NOTE: FOCUS GAINED

class FocusGainedEvent : public Event {
public:
    FocusGainedEvent() { m_type = Type::FOCUS_GAINED; }
    virtual void PrintEvent() override { LOG_DEBUG("Focus Gained Event"); }
}; // Class Focus Gained

// NOTE: FOCUS LOST

class FocusLostEvent : public Event {
public:
    FocusLostEvent() { m_type = Type::FOCUS_LOST; }
    virtual void PrintEvent() override { LOG_DEBUG("Focus Lost Event"); }
}; // Class Focus Lost

// NOTE: RESIZE

class WindowResizeEvent : public Event {
public:
    const glm::ivec2& GetResolution() const { return m_resolution; }
    i32 GetWidth() const { return m_resolution.x; }
    i32 GetHeight() const { return m_resolution.y; }
    WindowResizeEvent( glm::ivec2 resolution ) : m_resolution(resolution) { m_type = Type::WINDOW_RESIZE; }
    WindowResizeEvent( i32 w, i32 h ) : WindowResizeEvent(glm::ivec2(w, h)) {}
    virtual void PrintEvent() override { LOG_DEBUG("Window Resize Event: (%i, %i)", m_resolution.x, m_resolution.y); }
private:
    glm::ivec2 m_resolution;
};

// NOTE: MOVE

class WindowMoveEvent : public Event {
public:
    WindowMoveEvent( i32 x, i32 y ) : m_x(x), m_y(y) { m_type = Type::WINDOW_MOVE; }
    virtual void PrintEvent() override { LOG_DEBUG("Window Move Event: (%i, %i)", m_x, m_y); }
private:
    i32 m_x, m_y;
};

} // namespace Core

