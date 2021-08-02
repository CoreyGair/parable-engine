#include <Parable.h>

#include <Core/EntryPoint.h>


#include "Memory/LinearAllocator.h"
#include "Util/DynamicBitset.h"


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