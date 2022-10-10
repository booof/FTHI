#include "Class/Object/Terrain/TerrainBase.h"
#include "Class/Object/Terrain/Foreground.h"
#include "Globals.h"

typedef void(__stdcall* funct_pointer3)(Object::Object*);

// These Scripts are a Test Scripting 

// Test Initialize Script
void test_terrain_init(Object::Object* object_pointer)
{

}

// Test Update Script
void test_terrain_update(Object::Object* object_pointer)
{
	HINSTANCE test_dll = LoadLibrary(TEXT("../Core/EngineLibs/TestProject/Build/Debug/testlib.dll"));
	funct_pointer3 funct3 = (funct_pointer3)GetProcAddress(test_dll, "update");
	funct3(object_pointer);
	FreeLibrary(test_dll);

	Object::Terrain::Foreground& object = *static_cast<Object::Terrain::Foreground*>(object_pointer);
	object.test_var++;
	std::cout << object.test_var << " : test var test 2\n";

	//Object::Terrain::TerrainBase& object = *static_cast<Object::Terrain::TerrainBase*>(object_pointer);

	//glm::vec2& pos = object.returnObjectData().position;
	//pos += glm::vec2(2.0f * Global::deltaTime, 0.0f);
	//object.updateModel();
}

// Test Uninitialize Script
void test_terrain_delete(Object::Object* object_pointer)
{

}