#pragma once

#include "pblpch.h"

#include "Base.h"

// Forward decl for friend definition
int main(int argc, char** argv);

namespace Parable {

class Application{

    public:
        static Application* s_instance; /// Singleton application pointer 
        static Application& GetInstance() { return *s_instance; }

    public:
        Application();
        virtual ~Application(){};

    private:
        // Runs the main loop of the application
        void Run();

    private:
        bool m_Running = true;
        bool m_Minimised = false;
        friend int ::main(int argc, char** argv);

};

///
/// Create the singleton application object
///
/// To be defined in the CLIENT
///
Application* CreateApplication();

}