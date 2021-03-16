#pragma once

#include "Application.h"
extern Parable::Application* Parable::CreateApplication();

int main(int argc, char** argv){

    // Create app
    auto app = Parable::CreateApplication();

    // Start the app
    app->Run();

    // Delete app
    delete app;
}