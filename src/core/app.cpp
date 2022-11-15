/**
 * Description:  Application Definition
 * Author:       Alicia Amarilla (smushy) 
 * File Created: November 14, 2022 
*/
#include "app.hpp"
using namespace Core;

void Core::OnUpdate( AppContext* app ) {
    
}

AppContext Core::CreateContext() {
    AppContext app = {};
    app.isRunning  = true;
    return app;
}
void Core::OnClose( AppContext* app ) {
    app->isRunning = false;
}
