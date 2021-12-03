#include <gtest/gtest.h>

#include "test_with_malloc.h"

#include <Memory/LinearAllocator.h>

#include <ECS/EntityManager.h>
#include <ECS/ComponentManager.h>

#define ALLOC_SIZE 4000

class ComponentManagerSingleton : public MallocWrapper<ALLOC_SIZE>
{
public:
	ComponentManagerSingleton() : alloc(ALLOC_SIZE, mem),
									component_manager(2000,100,alloc)
	{
		component_manager.register_component<A>();
		component_manager.register_component<B>();
		
		component_manager.init();
	}

	~ComponentManagerSingleton()
	{
		alloc.clear();
	}

protected:

	Parable::LinearAllocator alloc;

	Parable::ECS::ComponentManager component_manager;

	// test components
	struct A : public Parable::ECS::Component<A>
	{
		int val = 0;
	};
	struct B : public Parable::ECS::Component<B>
	{
		int val = 1;
	};
	
};
