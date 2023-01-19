#pragma once
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "Render/Struct/Struct.h"

// A Basic One Way Linked List Class
template <class Type> class Struct::LinkedList
{
	// Definition for the Node
	class Node
	{
	public:

		// Reference to Object
		Type* object;

		// Reference to Next Node
		Node* next_node;

		// Reference to Previous Node
		Node* previous_node;

		// Remove Node From List
		void popNode();
	};

	// Size of the List
	int size = 0;

	// Starter Node
	Node* first_node;

	// Insert Node in List
	void insertNode(Type* object);

	// Clear List
	void clearList();

public:

	// Traverse Through List
	void traverseUpdate();

	// Traverse Through List
	void traverseDraw()
	{
		// Only Execute if There Are Objects
		if (!size)
			return;

		first_node->object->drawObject();
	}
};

//#include "LinkedList.cpp"

#endif

