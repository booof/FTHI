#include "SortingAlgorithms.h"

// Deffinition for Named Nodes
namespace Object::Physics::Rigid
{
	struct Named_Node
	{
		float value;
		int name;
		Named_Node(float Value, int Name) { value = Value; name = Name; }
		Named_Node() { value = 0.0f; name = 0; }
	};
}

template<class Type>
Type* Algorithms::Sorting::bubbleSort(Type object[], int size, int offset)
{
	// Iterate Through Size of Array Until All is Sorted
	for (int i = offset; i < size + offset; i++)
	{
		// Iterate Through Each Element in Array
		for (int j = offset; j < size - i + offset - 1; j++)
		{
			// Compare Current Element With Next Element
			// If Element is Greater than Next Element, Swap Places
			if (object[j] > object[j + 1])
			{
				Type intermediate_value = object[j];
				object[j] = object[j + 1];
				object[j + 1] = intermediate_value;
			}
		}
	}

	// Return Sorted Object
	return object;
}

template<class Type>
void Algorithms::Sorting::bubbleSort(Type object[], int size)
{
	// Iterate Through Size of Array Until All is Sorted
	for (int i = 0; i < size; i++)
	{
		// Iterate Through Each Element in Array
		for (int j = 0; j < size - i - 1; j++)
		{
			// Compare Current Element With Next Element
			// If Element is Greater than Next Element, Swap Places
			if (object[j] > object[j + 1])
			{
				Type intermediate_value = object[j];
				object[j] = object[j + 1];
				object[j + 1] = intermediate_value;
			}
		}
	}
}

void Algorithms::Sorting::sortNamedNodes(Object::Physics::Rigid::Named_Node** list, int size)
{
	// Iterate Through Size of Array Until All is Sorted
	for (int i = 0; i < size; i++)
	{
		// Iterate Through Each Element in Array
		for (int j = 0; j < size - i - 1; j++)
		{
			// Compare Current Element With Next Element
			// If Element is Greater than Next Element, Swap Places
			if ((*list)[j].value > (*list)[j + 1].value)
			{
				Object::Physics::Rigid::Named_Node intermediate_value = (*list)[j];
				(*list)[j] = (*list)[j + 1];
				(*list)[j + 1] = intermediate_value;
			}
		}
	}
}
