#include "event.hpp"

Core::EventQueue::~EventQueue() {
    while( !m_events.empty() ) {
        // delete all events
        delete( Pop() );
    }
    if( m_resizeEvent != nullptr ) {
        delete( m_resizeEvent );
    }
    if( m_moveEvent != nullptr ) {
        delete( m_moveEvent );
    }
}

void Core::EventQueue::Push( Event* event ) {
    switch( event->GetType() ) {
        case Event::Type::WINDOW_RESIZE: {
            m_resizeEvent = event;
        } break;
        case Event::Type::WINDOW_MOVE: {
            m_moveEvent = event;
        } break;
        default: m_events.push( event ); break;
    }
}

Core::Event* Core::EventQueue::Pop() {
    if( m_events.empty() ) {
        if( m_resizeEvent != nullptr ) {
            Event* result = m_resizeEvent;
            m_resizeEvent = nullptr;
            return result;
        }
        if( m_moveEvent != nullptr ) {
            Event* result = m_moveEvent;
            m_moveEvent = nullptr;
            return result;
        }
    } else {
        Event* result = m_events.front();
        m_events.pop();
        return result;
    }

    // return null if no valid condition is met
    return nullptr;
}
