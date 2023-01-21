#include "Class/Object/Object.h"
#include "Globals.h"
#include "Class/Render/Editor/Selector.h"
//#include "Class/Render/Objects/ScriptHandler.h"

extern DataClass::Data_Object* lllll;

void Object::Object::initializeScript(int script)
{
	//script_handler->bindFunctionPointer(script, this);
	Global::bindFunctionPointer(script, this);
}

void Object::Object::select2(Editor::Selector& selector)
{
	// Store Data Object in Selector
	selector.data_object = data_object;

	// Set Selector to Active Highlight
	selector.activateHighlighter();
}

void Object::Object::debug_funct()
{
	std::cout << object_index << " i\n";
}
