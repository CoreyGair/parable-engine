#include <gtest/gtest.h>

#include "test_component_manager.h"
#include <Util/Pointer.h>

#include <memory>

TEST_F(ComponentManagerSingleton, AddRemoveComponentsToEntity)
{
	component_manager->add_entity(0);

	A* a = (A*)component_manager->add_component(0, A::get_component_type());
	B* b = (B*)component_manager->add_component(0, B::get_component_type());

	// check for faults on access;
	a->val = 1;
	b->val = 0;

	// are the components actually attached to entity
	EXPECT_EQ(a, component_manager->get_component(0, A::get_component_type()));
	EXPECT_EQ(b, component_manager->get_component(0, B::get_component_type()));

	component_manager->remove_component(0, A::get_component_type());
	component_manager->remove_component(0, B::get_component_type());

	EXPECT_EQ(component_manager->get_component(0, A::get_component_type()), nullptr);
	EXPECT_EQ(component_manager->get_component(0, B::get_component_type()), nullptr);
}

TEST_F(ComponentManagerSingleton, HandleManyComponents)
{
	
	const size_t num_entities = 50;
	A* components[num_entities];


	for (size_t i = 0; i < num_entities; ++i)
	{
		component_manager->add_entity(i);
		components[i] = (A*)component_manager->add_component(i, A::get_component_type());
	}


	for (size_t i = 0; i < num_entities; ++i)
	{
		// are the components actually attached to entity
		EXPECT_EQ(components[i], component_manager->get_component(i, A::get_component_type()));

		// check for access faults
		components[i]->val = i;
	}


	for (size_t i = 0; i < num_entities; ++i)
	{
		component_manager->remove_component(i, A::get_component_type());
	}


	for (size_t i = 0; i < num_entities; ++i)
	{
		// are the components actually removed from entity
		EXPECT_EQ(component_manager->get_component(i, A::get_component_type()), nullptr);
	}

	for (size_t i = 0; i < num_entities; ++i)
	{
		component_manager->remove_entity(i);
	}

}