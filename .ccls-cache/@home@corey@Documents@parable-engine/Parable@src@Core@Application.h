#pragma once

#include "pblpch.h"

// Forward decl for friend definition
int main(int argc, char** argv);

namespace Parable {

class Application{

    public:
        static Application* s_instance;
        static Application& GetInstance() { return *s_instance; }

    public:
        Application();
        virtual ~Application(){};

    private:
        void Run();

    private:
        bool m_Running = true;
        bool m_Minimised = false;
        friend int ::main(int argc, char** argv);

};

// To be defined in the CLIENT
Application* CreateApplication();

}