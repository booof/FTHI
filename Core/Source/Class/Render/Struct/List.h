#ifndef LIST_H
#define LIST_H

#include "ExternalLibs.h"

//#include "Class/Object/Physics/PhysicsBase.h"

// The Primary Container for Physics and Entity Objects

// Prerequisites: Give Each Physics Object and Entity Their Own UUID Similar to the Object Index for Editing
// This UUID Should be Given Upon Creation Of Object From Editor Window
// Dynamic Objects and Objects Spawned From Scripting Should Not Have a UUID (Should be 0) as They Are Not Created From the Editor
// The Current UUID Index to Implement Should be Stored in a File
// Physics Objects Should Use Different UUIDs From Physics Objects

// This is a List (Similar to std::vector) used to contian Physics Objects and Entities
// The Contents of This List Should be Sorted From Lowest UUID to Greatest UUID
// This is to Allow OlogN complexity When Checking UUIDs of Objects
// To Add an Object, Allocate Memory if Needed Then Perform Insertion Sort
// To Remove an Object, Shift All Proceding Objects in Array Down
// Both Editor-Window Generated Objects and Dynamic Objects Will Be Stored in Same Array (Might Change Later) (Probably Will be Changed)

// Idea for Dynamic and Non-Dynamic Objects: Use an Array for Each, Iterators are to be used to iterate through each array easily

// Unkown if These Will have Constant Array Sizes or be Periodically Dynamically Resized

namespace Struct
{
	// The Number of Static Objects in List
	const uint32_t static_object_count = 20;

	// The Number of Dynamically Created Objects in List
	const uint32_t dynamic_object_count = 10;

	// The List Object
	template <class Type> class List
	{
		// The Array of Objects
		Type** object_array = nullptr;

		// Full Size of Array
		uint32_t array_size = 0;

		// The Initial Starting Position Between Static and Dynamic Objects
		// This Index in Array is the Pointer to the First Static Object
		//This Index - 1 in Array is the Pointer to the First Dynamic Object
		uint32_t type_seperator = 0;

		// Lowest Index in Array
		uint32_t start_index = 0;

		// Highest Index in Array
		uint32_t end_index = 0;

		// Binary Search Helper
		bool binarySearch(uint32_t uuid, uint32_t left, uint32_t right);

		// Remove a Static Object
		void removeStatic(uint32_t index);

		// Remove a Dynamic Object
		void removeDynamic(uint32_t index);

	public:

		// List Iterator
		struct Iterator
		{
			// The Index of the Iterator
			uint32_t index = 0;

			// Object Array
			Type** object_array_ = nullptr;

			// Increment the Iterator
			void operator++(int) { index++; }

			// Move Iterator
			Iterator operator+(const int& i) { index += i; return *this; }

			// Dereference the Iterator
			Type& operator*() { return *object_array_[index]; }
			Type& operator->() { return *object_array_[index]; }
			Type* operator&() { return object_array_[index]; }

			// Comparison Operators
			friend bool operator==(const Iterator& it, const Iterator& it2) { return it.index == it2.index; }
			friend bool operator!=(const Iterator& it, const Iterator& it2) { return it.index != it2.index; }

			// Initialize Iterator
			Iterator(uint32_t index_, Type** object_array__) { index = index_; object_array_ = object_array__; }
			Iterator() {}
		};

		Iterator it;

		// Initialize Object
		List();

		// Delete Object
		~List();

		// Erase Object
		void erase();

		// Test if UUID is Already Stored in Array]
		bool testIn(uint32_t uuid);

		// Append a Static Object
		bool appendStatic(Type* object);

		// Append a Dynamic Object
		void appendDynamic(Type* object);

		// Remove an Object
		void removeObject(uint32_t index);

		// Remove an Object
		void removeObject(Iterator it);

		// Return Index of Start of Array
		Iterator begin();

		// Return Index of Start of Dynamic Array
		Iterator beginDynamic();

		// Return Index of End of Dynamic Array
		Iterator endDynamic();

		// Return Index of Seperator
		Iterator indexSeperator();

		// Return Index of Start of Static Array
		Iterator beginStatic();

		// Return Index of End of Static Array
		Iterator endStatic();

		// Return Index of End of Array
		Iterator end();

		Type* operator[](const int& i) { return object_array[i]; }
	};
}

#include "List.tpc"

#endif
