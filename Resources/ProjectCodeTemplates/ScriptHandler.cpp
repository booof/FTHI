#define DLL_HEADER

#ifdef BUILD_DLL
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif

extern "C" EXPORT void __stdcall bindFunctionPointer(int index, Object::Object * object);
extern "C" EXPORT void __stdcall updateGlobalScripts();

// Script Function Declarations
#include "Scripts/ArraySizes.h"
#include "Scripts/GlobalScriptHeader.h"
#include "Scripts/ObjectScriptHeader.h"

namespace Scripts
{
	// The Null Function
	void null_function_global() {}
	void null_function_object(Object::Object* object_pointer) {}

	// Struct for Grouping a Global Function
	struct GlobalScript
	{
		void (*funct)();
		GlobalScript(void(*p1)())
		{
			funct = p1;
		}
	};

	// Struct for Grouping Object Scripts
	struct ObjectScriptGroup
	{
		void (*init_pointer)(Object::Object*);
		void (*update_pointer)(Object::Object*);
		void (*delete_pointer)(Object::Object*);
		ObjectScriptGroup(void(*p1)(Object::Object*), void(*p2)(Object::Object*), void(*p3)(Object::Object*))
		{
			init_pointer = p1;
			update_pointer = p2;
			delete_pointer = p3;
		}
	};

	// The Scripting Class
	class ScriptHandler
	{
	public:

		// The Global Script Array
		GlobalScript global_array[GLOBAL_SCRIPT_SIZE] =
		{
			// The Initial Null Script
			GlobalScript({&null_function_global})

			// The Dynamically Allocated Scripts
			#include "Scripts/GlobalScripts.h"
		};

		// The Object Script Array
		ObjectScriptGroup script_array[OBJECT_SCRIPT_SIZE] =
		{
			// The Initial Null Script Group
			ObjectScriptGroup({&null_function_object}, {&null_function_object}, {&null_function_object})

			// The Dynamically Allocated Scripts
			#include "Scripts/ObjectScripts.h"
		};

		// Default Constructor
		ScriptHandler() {}

	public:

	};
}

// The Script Instance
Scripts::ScriptHandler script_handler = Scripts::ScriptHandler();

// Bind Function Pointers
EXPORT void __stdcall bindFunctionPointer(int index, Object::Object* object)
{
	// If Index is Greater than the Array Size, Set to Null
	if (index > OBJECT_SCRIPT_SIZE)
		index = 0;

	Scripts::ObjectScriptGroup& group = script_handler.script_array[index];
	object->init = group.init_pointer;
	object->loop = group.update_pointer;
	object->del = group.delete_pointer;
}

// Update Global Functions
EXPORT void __stdcall updateGlobalScripts()
{
	for (int i = 0; i < GLOBAL_SCRIPT_SIZE; i++)
		script_handler.global_array[i].funct();
}

