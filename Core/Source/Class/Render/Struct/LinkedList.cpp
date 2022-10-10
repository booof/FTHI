#include "LinkedList.h"
#include "ExternalLibs.h"
#include "Class/Object/Physics/PhysicsBase.h"

template<class Type>
inline void Struct::LinkedList<Type>::Node::popNode()
{
	// Test if Node Should be Popped
	if (!object->ShouldPop())
		return;

	// Perform Pointer Swaps
	previous_node->next_node = next_node;
	next_node->previous_node = previous_node;

	// Decrement Size
	size--;
}

template<class Type>
void Struct::LinkedList<Type>::traverseUpdate()
{
	first_node->object->Update();
}

template<class Type>
void Struct::LinkedList<Type>::insertNode(Type* object)
{
	// Create New Node
	Node* new_node = std::unique_ptr(std::make_unique(Node));
	
	// Insert Object into new node
	new_node->object = object;

	// Store Next Node
	new_node->next_node = first_node;

	// Set This Node to First Node
	first_node = new_node;

	// Increment Size
	size++;
}

template<class Type>
void Struct::LinkedList<Type>::clearList()
{
	first_node = NULL;
}
