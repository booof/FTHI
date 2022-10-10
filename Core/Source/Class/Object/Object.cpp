#include "Class/Object/Object.h"
#include "Globals.h"
//#include "Class/Render/Objects/ScriptHandler.h"

void Object::Object::initializeScript(int script)
{
	//script_handler->bindFunctionPointer(script, this);
	Global::bindFunctionPointer(script, this);
}

void Object::Object::debug_funct()
{
	std::cout << object_index << " i\n";
}
