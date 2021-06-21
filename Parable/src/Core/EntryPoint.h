#pragma once

#include "Application.h"
#include "Base.h"

extern Parable::Application* Parable::create_application();

// Engine-defined main entrypoint
int main(int argc, char** argv){

    Parable::Log::init();

    // Create app
    auto app = Parable::create_application();

    // Start the app
    app->run();

    // Delete app
    delete app;
}