#include <windows.h>
#include "platform/time.hpp"
#include "alias.hpp"

f32 performanceFrequency;
u64 performanceCounterStart;

bool Platform::InitializeTimer() {
    LARGE_INTEGER lpFrequency;
    if( QueryPerformanceFrequency(&lpFrequency) == FALSE ) { return false; }
    LARGE_INTEGER lpPerformanceCount;
    if( QueryPerformanceCounter( &lpPerformanceCount ) == FALSE ) { return false; }

    performanceFrequency    = f32( f64( lpFrequency.QuadPart ) );
    performanceCounterStart = lpPerformanceCount.QuadPart;

    return true;
}

f32 Platform::ElapsedTime() {
    LARGE_INTEGER lpPerformanceCount;
    if( QueryPerformanceCounter( &lpPerformanceCount ) == FALSE ) {
        return 0.0f;
    }
    return f32( lpPerformanceCount.QuadPart - performanceCounterStart ) / performanceFrequency;
}
