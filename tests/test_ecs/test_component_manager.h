#include <gtest/gtest.h>

#include "test_with_malloc.h"

#include <Memory/LinearAllocator.h>

#include <ECS/EntityManager.h>
#include <ECS/ComponentManager.h>

#define ALLOC_SIZE 4000

class ComponentManagerSingleton : public MallocWrapper<ALLOC_SIZE>
{
public:
	ComponentManagerSingleton() : alloc(ALLOC_SIZE, mem)
	{
		Parable::ECS::ComponentRegistry reg;

		reg.register_component<A>();
		reg.register_component<B>();

		component_manager = std::make_unique<Parable::ECS::ComponentManager>(reg, 2000, 100, 1000, alloc);
	}

	~ComponentManagerSingleton()
	{
		alloc.clear();
	}

protected:
	Parable::LinearAllocator alloc;

	UPtr<Parable::ECS::ComponentManager> component_manager;

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