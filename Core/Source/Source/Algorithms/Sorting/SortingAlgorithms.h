#pragma once
#ifndef SORTING_ALGORITHMS_H
#define SORTING_ALGORITHMS_H

#include "Algorithms/Algorithms.h"

// Declaration for Named Nodes
namespace Object::Physics::Rigid
{
	struct Named_Node;
}

// Declaration for Data Classes
namespace Object
{
	class Object;
}

namespace Algorithms::Sorting
{
	// Bubble Sorting Algorithm With Offsets
	template <class Type> Type* bubbleSort(Type object[], int size, int offset);

	// Bubble Sorting Algorithm Without Offsets
	template <class Type> void bubbleSort(Type object[], int size);

	// Sort an Array of Named Nodes
	//void sort_Named_Nodes(Named_Node(&object)[4], int size);

	// Quick Z Sort Partition Function
	template <class Type> int quickZSortPartition(Type** objects, int first, int last)
	{
		// Pivot Index
		int pivot = first;

		// Get Midpoint of Section
		int midpoint = (int)floor((last - first) * 0.5f);

		// Get Three Arbitrary Points of Data
		float first_data = objects[first]->zpos;
		float mid_data = objects[midpoint]->zpos;
		float last_data = objects[last]->zpos;

		// Mid is Median
		if ((first_data < mid_data && mid_data < last_data) || (first_data > mid_data && mid_data > last_data))
			pivot = midpoint;

		// Last is Median
		else if ((first_data < last_data && last_data < mid_data) || (first_data > last_data && last_data > mid_data))
			pivot = last;

		// Get Pivot Value
		Type* pivot_object = objects[pivot];
		float pivot_value = pivot_object->zpos;

		// Swap Pivot With First Value
		objects[pivot] = objects[first];
		objects[first] = pivot_object;

		// Left and Right Markers
		int left_mark = first + 1;
		int right_mark = last;

		// Iterate Until Markers Pass Eachother
		bool done = false;
		do
		{
			// Increment Left Marker
			while (left_mark <= right_mark && objects[left_mark]->zpos <= pivot_value)
				left_mark++;

			// Decrement Right Marker
			while (right_mark >= left_mark && objects[right_mark]->zpos >= pivot_value)
				right_mark--;

			// Test If Markers Pass Eachother
			if (right_mark < left_mark)
				done = true;

			// Else, Swap Marker Values
			else
			{
				Type* temp = objects[left_mark];
				objects[left_mark] = objects[right_mark];
				objects[right_mark] = temp;
			}

		} while (!done);

		// Swap Pivot and Value at Right Marker
		objects[first] = objects[right_mark];
		objects[right_mark] = pivot_object;

		// Return Splitpoint
		return right_mark;
	}

	// Quick Z Sort Helper Function
	template <class Type> void quickZSortHelper(Type** objects, int first, int last)
	{
		if (first < last)
		{
			int splitpoint = quickZSortPartition(objects, first, last);
			quickZSortHelper(objects, first, splitpoint - 1);
			quickZSortHelper(objects, splitpoint + 1, last);
		}
	}

	// Quick Sort for Z-Positions of Objects
	// Sorts From Least to Greatest
	template <class Type> void quickZSort(Type** objects, int list_size)
	{
		quickZSortHelper(objects, 0, list_size - 1);
	}

	// Quick Identifier Sort Partition Function
	int quickIdentifierSortPartition(Object::Object** objects, int first, int last);

	// Quick Identifier Sort Helper Function
	void quickIdentifierSortHelper(Object::Object** objects, int first, int last);

	// Quick Identifier Sort
	// Sorts Based on Object Identifier from Lowest to Highest
	void quickIdentifierSort(Object::Object** objects, int list_size);

	// Sort an Array of Named Nodes
	void sortNamedNodes(Object::Physics::Rigid::Named_Node** list, int size);
}

#endif
