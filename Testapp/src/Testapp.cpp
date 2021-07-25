#include <Parable.h>

#include <Core/EntryPoint.h>


class Testapp : public Parable::Application
{
    public:
        Testapp()
        {

        }

        ~Testapp()
        {

        }
};

Parable::Application* Parable::create_application()
{
	return new Testapp();
}