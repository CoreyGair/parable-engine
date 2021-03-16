#pragma once

#include "Application.h"
#include "Base.h"

extern Parable::Application* Parable::CreateApplication();

int main(int argc, char** argv){

    Parable::Log::Init();

    // Create app
    auto app = Parable::CreateApplication();

    // Start the app
    app->Run();

    // Delete app
    delete app;
}