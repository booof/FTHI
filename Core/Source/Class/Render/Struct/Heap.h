#ifndef HEAP_H
#define HEAP_H

// This File is for Definitions for Heaps

namespace Struct
{
	// Min Heap
	template <class Type> class MinHeap
	{
		// The Array of Objects
		Type* object_array = nullptr;

		// The Size of the Array
		int array_size = 0;

		// The Number of Objects in Array
		int object_count = 0;

		// Perc Up
		void percUp(int index)
		{
			// Move Object Up Heap Until Heap is Partially Sorted
			int parent = (int)floor(index * 0.5f);
			while (index > 1 && object_array[index] < object_array[parent])
			{
				// Swap Parent Item With Item to be Perced Up
				Type temp_object_holder = object_array[index];
				object_array[index] = object_array[parent];
				object_array[parent] = temp_object_holder;

				// Determine New Location of Item to be Perced Up and Corrisponding Parent
				index = parent;
				parent = (int)floor(index * 0.5f);
			}
		}

		// Perc Down
		void percDown(int index)
		{
			// Temp Variable for Swapping
			Type temp_swap_var;

			// Temp Indicies for Child Objects
			int child1 = 0;
			int child2 = 0;

			// Move Item at Given Index Down Until Heap is Partially Sorted
			bool move_down = true;
			while (move_down)
			{
				// Determine Indices for Children
				child1 = index * 2;
				child2 = child1 + 1;

				// If Both Indices Don't Exist, (Leaft Node) Do Nothing
				if (child1 > object_count)
					move_down = false;

				// If Child1 Exists and Child2 Does Not, Determine if Items Should Swap
				else if (child2 > object_count)
				{
					// If Parent is Lower Than Child, Do Nothing
					if (object_array[index] < object_array[child1])
						move_down = false;

					// Else, Swap Values and Continue Moving Down
					else
						percDownSwapHelper(index, child1, temp_swap_var);
				}

				// Both Children Exists, Swap With Smallest Child
				else
				{
					// If Parent is Less Than Both Children, Do Nothing
					if (object_array[index] < object_array[child1] && object_array[index] < object_array[child2])
						move_down = false;

					// Child1 is Less
					else if (object_array[child1] < object_array[child2])
						percDownSwapHelper(index, child1, temp_swap_var);

					// Child2 is Less
					else
						percDownSwapHelper(index, child2, temp_swap_var);
				}
			}
		}

		// Helper Function to Swap Values for Perc Down
		void percDownSwapHelper(int& index, int& child, Type& temp_swap_var)
		{
			temp_swap_var = object_array[index];
			object_array[index] = object_array[child];
			object_array[child] = temp_swap_var;
			index = child;
		}

	public:

		// Construct the Heap
		MinHeap(int heap_size)
		{
			// Store Heap Size + 1 (Index 0 is not Used)
			array_size = heap_size + 1;

			// Allocate Memory for Objects
			object_array = new Type[array_size];
		}

		// Deconstruct the Heap
		~MinHeap()
		{
			if (array_size)
				delete[] object_array;
		}

		// Enqueue an Item
		void enqueue(Type item)
		{
			// If Heap is Full, Throw Error
			if (object_count == array_size - 1)
				throw "HEAP FULL";

			// Insert Item at End of Array
			object_count++;
			object_array[object_count] = item;

			// Perc Up
			percUp(object_count);
		}

		// Dequeue an Item
		Type dequeue()
		{
			// If Heap is Empty, Throw Error
			if (object_count == 0)
				throw "HEAP EMPTY";

			// Store Item at Root, Promote Item at End of Array
			Type temp_item_holder = object_array[1];
			object_array[1] = object_array[object_count];
			object_count--;

			// Perc Down
			percDown(1);

			// Return Stored Item
			return temp_item_holder;
		}
		
		// Returns True if Heap is Full
		bool full()
		{
			return object_count == array_size - 1;
		}

		// Returns True if Heap is Empty
		bool empty()
		{
			return object_count == 0;
		}
	};
}

#endif
