/**
 * Description:  Application Definition
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 14, 2022 
*/
#pragma once
#include "pch.hpp"
#include "time.hpp"

namespace Core {

struct AppContext {
    bool isRunning;
    Time time;
};

AppContext CreateContext();
void OnClose( AppContext* app );
void OnUpdate( AppContext* app );

} // namespace Core

