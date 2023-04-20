#pragma once
#ifndef UNSAVED_COMPLEX_H
#define UNSAVED_COMPLEX_H

#include "UnsavedCollection.h"
#include "Render/Struct/DataClasses.h"

// This File Will Handle the Storage of Changes Made to SpringMass
// and Hinge Physics Objects. Since These Object Types Utilize a
// Seperate File for Storing Information, They Will be Handled
// Sperately from Generalized Group Objects. The File Process Will
// Try to Stay Unchanged from the Current SpringMass File Structure,
// Where Objects Are Read First Before Connectors. The Proccess of
// Editing These Objects Should Also Remain the Same.
//
// As of Now, It is Currently Unkown if Objects in a Hinge or SpringMass
// Will be Able to Utilize a General Parent-Child Relationship With Other
// Objects That Are Not Physics Objects. Although it Could Lead to Interesting
// Creations, This May Cause Unneccessary Bloat, and Enough Bloat Might Casue
// This Idea to be Killed Off. Before it is Entirely Killed Off, However, the
// Object Identifier in these Files Will be Expanded to 4-bytes Like the Object
// Identifier for General Group Objects.

// New Ideas:
 
// Reading and Writing to Files Will Use the Same Code as Used in the Unsaved Level,
// But Updated to Include Hinge and Soft-Body Objects. When Creating Groups, Most Objects
// Will Utilize the Default Unsaved Group Object, However, Hinge Cores, Soft-Body Cores,
// and Group Objects Will Instead Utilize This Unsaved Complex Object. This is to Allow
// These Types of Objects to Write Their Contents to a File. These Files Allow For Storage of
// These Object as Well as Reusability

// Hinge Objects Will Only be Able to Store Physics Objects and Hinge Connectors, Soft Body
// Objects Will Only be Able to Store Nodes and Springs, and Group Objects Will Contain All 
// Objects That Can be Created From the Editor Window

// Hinge Cores, Soft Body Cores, and Group Object Cores Cannot be Included in a Group and,
// Under No Circumstances, Will Ever Have a Parent. Instead, To Allow For More Complex Objects,
// Such as Soft Bodies Attached Via a Hinge, There Should be an Object Identifier That Exists in
// Parrallel to These Objects That Generate These Objects, As Well As Contains a List of Dynamic
// Size Containing the Incides of Connection Points For Limbs

// Core Objects Won't Have Unsaved Group Objects As That Pointer Will Instead be an Unsaved
// Complex Object. However, Children of This Unsaved Complex Object Will Have an Optional
// Unsaved Group Object

// The Layer of Core Objects Will Essentially be -1, as All Children Will Have a Layer of 0
// Due to Layers Being Directly Tied to Unsaved Group Objects. There Will Also be a Custom 
// Color for Parent-Child Connections When a Core is a Parent. This Mostly Applies to Group
// Objects

// Another Idea:
// Unlike the Unsaved Group, Objects From the Unsaved Complex Objects Will Not be Stored
// Directly in the Corrisponding Unsaved Level of Origin. Instead, The Objects Will be Stored
// Within the Unsaved Complex Object, and Unsaved Levels Will be Given the Ability to Call
// Complex Objects to Generate Their Objects When Generating Objects for the Level. In Addition,
// The Rendering Of Complex Objects Will be Done Through the Unsaved Complex Object, So The
// Editing of an Object in One Complex Object Instance Will Affect All Other Complex Object
// Instances. This Behavior Will be Similar to That of Dynamic Entities.

namespace Render::Objects
{
	class UnsavedComplex : public UnsavedCollection
	{

	private:

		// Path to Complex Object Files
		std::string file_path;

		// The Root Parent of This Complex Object
		DataClass::Data_ComplexParent complex_parent;

		// The Instance Array of Group Objects Loaded in the World
		// Used for Drawing Currently Made Changes Via Selection
		// Also Used to Visualize the Save-State of the Object on Each Instance
		std::vector<Object::Object*> instance_vector;

		// Construct Unmodified Data Helper
		void constructUnmodifiedDataHelper(ObjectsInstance& instance);

		// Function to Add Objects While Transversing
		void addWhileTraversing(DataClass::Data_Object* data_object, bool move_with_parent);

		// Function to Remove Objects While Traversing
		void removeWhileTraversing(DataClass::Data_Object* data_object);

		// Function to Perform Object-Specific Changes After an Undo/Redo
		void updatePostTraverse();

		// Remove Instance in Change List
		void removeChainListInstance();

		// Tests if an Object can be Successfully Added to the Object
		bool testValidSelection(DataClass::Data_Object* parent, DataClass::Data_Object* test_child);

		// Update the Vertices of the Visualizer
		void updateVisualizer();

	public:

		// Unsaved Complex Constructor
		UnsavedComplex();

		// Write Current Instance to File
		void write(bool& save);

		// Construct Unmodified Data
		void constructUnmodifiedData(std::string file_path_);

		// Recursively Set the Group Layer
		void recursiveSetGroupLayer(uint8_t layer);

		// Returns the File Name of the Object
		std::string& getFilePath();

		// Determines What the Collection Type is
		UNSAVED_COLLECTIONS getCollectionType();

		// Returns the Complex Parent
		DataClass::Data_Object* getComplexParent();

		// Add a Instance of a Complex Object to Instance Array
		void addComplexInstance(Object::Object* new_instance);

		// Remove Instance From Instance Array
		void removeComplexInstance(Object::Object* pointer_to_be_removed);

		// Tests if Complex Instance is In Instance Array
		bool testForInstance(uint32_t test_index);

		// Tests if a Complex Instance is Being Selected
		bool testForSelectedInstance();

		// Returns the Selected Position of an Instance
		glm::vec2* getSelectedPosition();

		// Returns the Instance Array
		std::vector<Object::Object*> getInstances();

		// Draw Object State Visualizer
		void drawVisualizer();

		// Draw Currently Edited Object for Other Instances
		void drawSelected(int object_vertex_count, int mode, GLuint model_loc, glm::vec3 delta_pos, Object::Object* instance_to_skip);

		// Draw Connection to Currently Edited Object for All Instances
		void drawSelectedConnection(DataClass::Data_Object* selected_object, glm::vec2 position_offset);
	};
}

#endif
