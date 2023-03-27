#include "SortingAlgorithms.h"
#include "Render/Struct/DataClasses.h"

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

// Override for Equals for Object Identifiers
bool operator==(DataClass::Data_Object& l, DataClass::Data_Object& r)
{
	return (
		l.getObjectIdentifier()[0] == r.getObjectIdentifier()[0] &&
		l.getObjectIdentifier()[1] == r.getObjectIdentifier()[1] &&
		l.getObjectIdentifier()[2] == r.getObjectIdentifier()[2]
		);
}

// Override for Less Than for Object Identifiers
bool operator<(DataClass::Data_Object& l, DataClass::Data_Object& r)
{
	if (l.getObjectIdentifier()[0] != r.getObjectIdentifier()[0])
		return l.getObjectIdentifier()[0] < r.getObjectIdentifier()[0];
	if (l.getObjectIdentifier()[1] != r.getObjectIdentifier()[1])
		return l.getObjectIdentifier()[1] < r.getObjectIdentifier()[1];
	return l.getObjectIdentifier()[2] < r.getObjectIdentifier()[2];
}

// Override for Greater Than for Object Identifiers
bool operator>(DataClass::Data_Object& l, DataClass::Data_Object& r)
{
	if (l.getObjectIdentifier()[0] != r.getObjectIdentifier()[0])
		return l.getObjectIdentifier()[0] > r.getObjectIdentifier()[0];
	if (l.getObjectIdentifier()[1] != r.getObjectIdentifier()[1])
		return l.getObjectIdentifier()[1] > r.getObjectIdentifier()[1];
	return l.getObjectIdentifier()[2] > r.getObjectIdentifier()[2];
}

// Override for Less Than or Equal to for Object Identifiers
bool operator<=(DataClass::Data_Object& l, DataClass::Data_Object& r)
{
	return (l == r) || (l < r);
}

// Override for Greater Than or Equal to for Object Identifiers
bool operator>=(DataClass::Data_Object& l, DataClass::Data_Object& r)
{
	return (l == r) || (l > r);
}

int Algorithms::Sorting::quickIdentifierSortPartition(Object::Object** objects, int first, int last)
{
	// Pivot Index
	int pivot = first;

	// Get Midpoint of Section
	int midpoint = (int)floor((last - first) * 0.5f);

	// Get Three Arbitrary Points of Data
	DataClass::Data_Object& first_data = *objects[first]->data_object;
	DataClass::Data_Object& mid_data = *objects[midpoint]->data_object;
	DataClass::Data_Object& last_data = *objects[last]->data_object;

	// Mid is Median
	if ((first_data < mid_data && mid_data < last_data) || (first_data > mid_data && mid_data > last_data))
		pivot = midpoint;

	// Last is Median
	else if ((first_data < last_data && last_data < mid_data) || (first_data > last_data && last_data > mid_data))
		pivot = last;

	// Get Pivot Value
	Object::Object& pivot_object = *objects[pivot];
	DataClass::Data_Object& pivot_value = *pivot_object.data_object;

	// Swap Pivot With First Value
	objects[pivot] = objects[first];
	objects[first] = &pivot_object;

	// Left and Right Markers
	int left_mark = first + 1;
	int right_mark = last;

	// Iterate Until Markers Pass Eachother
	bool done = false;
	do
	{
		// Increment Left Marker
		while (left_mark <= right_mark && *objects[left_mark]->data_object <= pivot_value)
			left_mark++;

		// Decrement Right Marker
		while (right_mark >= left_mark && *objects[right_mark]->data_object >= pivot_value)
			right_mark--;

		// Test If Markers Pass Eachother
		if (right_mark < left_mark)
			done = true;

		// Else, Swap Marker Values
		else
		{
			Object::Object* temp = objects[left_mark];
			objects[left_mark] = objects[right_mark];
			objects[right_mark] = temp;
		}

	} while (!done);

	// Swap Pivot and Value at Right Marker
	objects[first] = objects[right_mark];
	objects[right_mark] = &pivot_object;

	// Return Splitpoint
	return right_mark;
}

void Algorithms::Sorting::quickIdentifierSortHelper(Object::Object** objects, int first, int last)
{
	if (first < last)
	{
		int splitpoint = quickIdentifierSortPartition(objects, first, last);
		quickIdentifierSortHelper(objects, first, splitpoint - 1);
		quickIdentifierSortHelper(objects, splitpoint + 1, last);
	}
}

void Algorithms::Sorting::quickIdentifierSort(Object::Object** objects, int list_size)
{
	quickIdentifierSortHelper(objects, 0, list_size - 1);
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
