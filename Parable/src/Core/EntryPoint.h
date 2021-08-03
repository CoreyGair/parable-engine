#pragma once

#include "Core/Base.h"
#include "Core/Application.h"

extern Parable::Application* Parable::create_application();

/**
 * @file EntryPoint.h
 * 
 * Defines the engine main function.
 */

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