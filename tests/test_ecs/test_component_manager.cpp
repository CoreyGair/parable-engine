#include <gtest/gtest.h>

#include "test_component_manager.h"
#include <Util/Pointer.h>

#include <memory>

TEST_F(ComponentManagerSingleton, AddRemoveComponentsToEntity)
{
	component_manager.add_entity(0);

	A* a = component_manager.add_component<A>(0);
	B* b = component_manager.add_component<B>(0);

	// did the constructors run correctly
	EXPECT_EQ(a->val, 0);
	EXPECT_EQ(b->val, 1);

	// are the components actually attached to entity
	EXPECT_EQ(a, component_manager.get_component<A>(0));
	EXPECT_EQ(b, component_manager.get_component<B>(0));

	component_manager.remove_component<A>(0);
	component_manager.remove_component<B>(0);

	EXPECT_EQ(component_manager.get_component<A>(0), nullptr);
	EXPECT_EQ(component_manager.get_component<B>(0), nullptr);
}

TEST_F(ComponentManagerSingleton, HandleManyComponents)
{
	
	const size_t num_entities = 100;
	A* components[num_entities];


	for (size_t i = 0; i < num_entities; ++i)
	{
		component_manager.add_entity(i);
		components[i] = component_manager.add_component<A>(i);
	}


	for (size_t i = 0; i < num_entities; ++i)
	{
		// are the components actually attached to entity
		EXPECT_EQ(components[i], component_manager.get_component<A>(i));
	}


	for (size_t i = 0; i < num_entities; ++i)
	{
		component_manager.remove_component<A>(i);
	}


	for (size_t i = 0; i < num_entities; ++i)
	{
		// are the components actually removed from entity
		EXPECT_EQ(component_manager.get_component<A>(i), nullptr);
	}

	for (size_t i = 0; i < num_entities; ++i)
	{
		component_manager.remove_entity(i);
	}

}