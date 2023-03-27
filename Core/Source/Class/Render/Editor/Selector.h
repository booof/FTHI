#pragma once
#ifndef SELECTOR_H
#define SELECTOR_H

#include "Class/Render/Editor/EditorWindow.h"

namespace Render::Camera
{
	class Camera;
}

namespace Render::Objects
{
	class Level;
	class UnsavedLevel;
}

namespace Object::Physics::Hinge
{
	class Hinge;
}

typedef void (Editor::Selector::* func)(float(&endpoints)[8], Render::Objects::UnsavedLevel* unsaved_level, int Type, int i, int extraValue);


namespace Editor
{
	class ObjectInfo;

	// List of Editing Shapes
	enum EditingShapes : unsigned char
	{
		RECTANGLE,
		TRAPEZOID,
		TRIANGLE,
		CIRCLE,
		LINE,
		HORIZONTAL_LINE,
		VERTICAL_LINE,
		SPRINGMASS_OBJECT,
		SPRINGMASS_NODE,
		SPRINGMASS_SPRING,
		HINGE
	};

	// List of Movement Directions
	enum EditingDirections : uint8_t
	{
		NORTH,
		EAST,
		SOUTH,
		WEST
	};

	// The Return Values of Selected Objects
	enum SelectedReturnValues : uint8_t
	{
		DESELECTED,
		ABSOLUTLY_NOTHING,
		MOUSED_OVER,
		MOVING,
		RESIZING
	};

	// A Struct for a Pointer to Limb Attatched to Currently Selected Object
	struct ConnectedLimb
	{
		// Pointer to Limb
		Object::Physics::Soft::Spring* limb_ptr = nullptr;

		// Connected Object Index
		bool connected_first = true;
	};

	// Selector Class
	class Selector : public EditorWindow
	{
		struct Group_Selector
		{
			// Vertex Array Object for Outline
			GLuint outlineVAO = 0;
			GLuint outlineVBO = 0;

			// Model Loc
			glm::mat4 model = glm::mat4(1.0f);

			// Extreme Values
			// Equal to the Farthest Vertex From Any Shape in Any Direction
			float extreme_value_north = 0.0f;
			float extreme_value_south = 0.0f;
			float extreme_value_east = 0.0f;
			float extreme_value_west = 0.0f;

			// The Origin of the Group
			// (Mean of All Extreme Values)
			glm::vec2 position = glm::vec2(0.0f);

			// Offsets of Mouse to Object
			float offset_x = 0.0f;
			float offset_y = 0.0f;

			// Determines if Objects Are Being Resized
			bool resizing = false;

			// Resize Offsets
			float change_horizontal = 0.0f;
			float change_vertical = 0.0f;
		};

		struct Selected_VertexObjects
		{
			// Vertex Array Object for Selected Object
			GLuint objectVAO = 0;
			GLuint objectVBO = 0;

			// Vertex Array Object for Outline
			GLuint outlineVAO = 0;
			GLuint outlineVBO = 0;

			// Model Loc
			glm::mat4 model = glm::mat4(1.0f);

			// Number of Object Vertices
			short object_vertex_count = 0;

			// Number of Outline Vertices
			short outline_vertex_count = 0;

			// Determines if Selected is an Object with Color and Texture
			bool visualize_object = false;

			// Determines if Selected Object is Composed of Lines Rather Than Triangles
			bool visualize_lines = false;

			// Determines if Selected Object Only Uses a Texture
			bool visualize_texture = false;

			// Determines if Selected Object is a Lighting Object
			bool lighting_object = false;
		};

		typedef void (*gen_vertices)(DataClass::Data_Object* data_object, Editor::Selector::Selected_VertexObjects* group_selector);

		class Selected_Object
		{

		public:

			// Pointer to Data Object
			DataClass::Data_Object* data_object = nullptr;

			// Position of the Object
			float* object_x = nullptr;
			float* object_y = nullptr;

			// Vertex Objects and Rendering Information
			Selected_VertexObjects vertex_objects;

			// Offsets of Mouse to Object
			float offset_x = 0.0f;
			float offset_y = 0.0f;

			// Resize Offsets
			float change_horizontal = 0.0f;
			float change_vertical = 0.0f;

			// Shape of Editing Object
			unsigned char editing_shape;

			// Determines if an Object is Moving
			static bool moving;

			// Determines if Object is in Resize Mode
			static bool resizing;

			// Determines if Mouse Intersects Object
			static bool mouse_intersects_object;

			// Determines if Object is Being Rotated
			static bool rotating;

			// Pointer to the Group Selector
			static Group_Selector* group_selector;

			// Pointer to the Function to Generate Vertices
			static std::function<void(DataClass::Data_Object*, Selected_VertexObjects&)> genSelectorVertices;

			// Pointer to the Function to Rotate Objects
			static std::function<void(bool, Selected_Object*)> sortVertices;

			// Pointer to the Function to Assign the Temp Connection Positions
			static std::function<void(glm::vec2&, glm::vec2&)> storeTempConnectionPos;

			// Function to Update the Object
			virtual uint8_t updateObject() = 0;

			// Function to Update the Group Selector
			virtual void updateGroup(Group_Selector& group_selector) = 0;

			// Function to Move the Object
			virtual void moveObject() = 0;

			// Function to Set Mouse Offset
			void setMouseOffset();

			// Function to Set Horizontal Group Resize
			virtual void setHorizontalGroupResize() = 0;

			// Function to Set Vertical Group Resize
			virtual void setVerticalGroupResize() = 0;

			// Change Outline Color for Resize Mode
			void outlineForResize();

			// Change Outline Color for Move Mode
			void outlineForMove();

			// Change Outline Color for When Mouse is Not in Object
			void outlineForNotSelected();

			// Change Color of Object
			void outlineChangeColor(float* colors);

			// Update Group Objects of a Selected Object
			void updateSelectedPositions(DataClass::Data_Object* data_object, float deltaX, float deltaY);
		};

		class Selected_Rectangle : public Selected_Object
		{

		public:

			// Size of the Object
			float* object_width = nullptr;
			float* object_height = nullptr;

			// Determines if Rectangle can Resize
			bool enable_resize = true;

			// Function to Update the Object
			uint8_t updateObject();

			// Function to Update the Group Selector
			void updateGroup(Group_Selector& group_selector);

			// Function to Move the Object
			void moveObject();

			// Function to Resize a Rectangle
			void testResizeRectangle(bool enable_horizontal, bool enable_vertical);

			// Function to Set Horizontal Group Resize
			void setHorizontalGroupResize();

			// Function to Set Vertical Group Resize
			void setVerticalGroupResize();
		};

		class Selected_Trapezoid : public Selected_Rectangle
		{

		public:

			// Size Offset of the Object
			float* object_width_modifier = nullptr;
			float* object_height_modifier = nullptr;

			// Function to Update the Object
			uint8_t updateObject();

			// Function to Update the Group Selector
			void updateGroup(Group_Selector& group_selector);

			// Function to Move the Object
			void moveObject();

			// Function to Resize a Trapezoid
			void testResizeTrapezoid();

			// Function to Set Horizontal Group Resize
			void setHorizontalGroupResize();

			// Function to Set Vertical Group Resize
			void setVerticalGroupResize();
		};

		class Selected_Triangle : public Selected_Object
		{
			
		public:

			// The Coordinates of the Object
			glm::vec2 coords1 = glm::vec2(0);
			glm::vec2 coords2 = glm::vec2(0);
			glm::vec2 coords3 = glm::vec2(0);

			// Triangle Collision Values
			int8_t selected_vertex = 0;

			bool should_sort = false;

			// Function to Update the Object
			uint8_t updateObject();

			// Function to Update the Group Selector
			void updateGroup(Group_Selector& group_selector);

			// Helper Function to Update the Group
			void updateGroupHelper(Group_Selector& group_selector, glm::vec2 test_pos);

			// Function to Move the Object
			void moveObject();

			// Function to Resize a Triangle
			void testResizeTriangle();

			// Function to Set Horizontal Group Resize
			void setHorizontalGroupResize();

			// Function to Set Vertical Group Resize
			void setVerticalGroupResize();
		};

		class Selected_Circle : public Selected_Object
		{

		public:

			// Radius of the Circle
			float* object_radius = nullptr;
			float* object_inner_radius = nullptr;

			// Circle Collision Values
			int8_t selected_vertex = 0;

			// Angle of Mouse to Center of Circle
			float mouse_angle = 0.0f;

			// Function to Update the Object
			uint8_t updateObject();

			// Function to Update the Group Selector
			void updateGroup(Group_Selector& group_selector);

			// Function to Move the Object
			void moveObject();

			// Function to Resize a Circle
			void testResizeCircle(float& distance, float& delta_w, float& delta_h);

			// Function to Set Horizontal Group Resize
			void setHorizontalGroupResize();

			// Function to Set Vertical Group Resize
			void setVerticalGroupResize();
		};

		class Selected_Horizontal_Line : public Selected_Rectangle
		{

		public:

			// Function to Update the Object
			uint8_t updateObject();

			// Function to Update the Group Selector
			void updateGroup(Group_Selector& group_selector);

			// Function to Set Vertical Group Resize
			void setVerticalGroupResize();
		};

		class Selected_Vertical_Line : public Selected_Rectangle
		{
		
		public:

			// Function to Update the Object
			uint8_t updateObject();

			// Function to Update the Group Selector
			void updateGroup(Group_Selector& group_selector);

			// Function to Set Horizontal Group Resize
			void setHorizontalGroupResize();
		};

		class Selected_Line : public Selected_Object
		{
		
		public:

			// Opposite Point of Line
			float* object_opposite_x = nullptr;
			float* object_opposite_y = nullptr;

			// Circle Collision Values
			int8_t selected_vertex = 0;

			// Values for Line
			float slope = 0.0f;
			float intercept = 0.0f;

			// Function to Update the Object
			uint8_t updateObject();

			// Function to Update the Group Selector
			void updateGroup(Group_Selector& group_selector);

			// Helper Function to Update a Group
			void updateGroupHelper(Group_Selector& group_selector, float& test_x, float& test_y);

			// Function to Move the Object
			void moveObject();

			// Function to Resize a Line
			void testResizeLine();

			// Function to Set Horizontal Group Resize
			void setHorizontalGroupResize();

			// Function to Set Vertical Group Resize
			void setVerticalGroupResize();
		};

		class Selected_Unsized : public Selected_Object
		{

		public:

			// Function to Move the Object
			void moveObject();

			// Helper Function to Update Group
			void updateGroupHelper(Group_Selector& group_selector, float half_width, float half_height);

			// Function to Set Horizontal Group Resize
			void setHorizontalGroupResize() {}

			// Function to Set Vertical Group Resize
			void setVerticalGroupResize() {}
		};

		class Selected_SpringMassObject : public Selected_Unsized
		{

		public:

			// Size of the Object
			const float object_half_width = 2.0f;
			const float object_half_height = 2.0f;

			// Function to Update the Object
			uint8_t updateObject();

			// Function to Update the Group Selector
			void updateGroup(Group_Selector& group_selector);
		};

		class Selected_SpringMassNode : public Selected_Unsized
		{

		public:	

			// Radius of the Object
			float* object_radius;

			// Function to Update the Object
			uint8_t updateObject();

			// Function to Update the Group Selector
			void updateGroup(Group_Selector& group_selector);
		};

		class Selected_SpringMassSpring : public Selected_Object
		{

		public:

			// Opposite Point of Spring
			float* object_opposite_x = nullptr;
			float* object_opposite_y = nullptr;

			// Circle Collision Values
			int8_t selected_vertex = 0;

			// Values for Line
			float slope = 0.0f;
			float intercept = 0.0f;

			// Initial Positions for Connection Objects
			glm::vec2 connection_pos_left;
			glm::vec2 connection_pos_right;

			// List of Nodes for SpringMass
			Object::Physics::Soft::NodeData* node_list;
			int node_count = 0;

			// Function to Update the Object
			uint8_t updateObject();

			// Function to Update the Group Selector
			void updateGroup(Group_Selector& group_selector);

			// Function to Move the Object
			void moveObject();

			// Function to Set Horizontal Group Resize
			void setHorizontalGroupResize() {}

			// Function to Set Vertical Group Resize
			void setVerticalGroupResize() {}
		};

		// The Group Selector
		Group_Selector group_selector;

		// Highlighted Vertex Objects
		Selected_VertexObjects highlighted_vertex_objects;

		// Vertex Array Object for Pivot
		GLuint pivotVAO, pivotVBO;

		// Copy Buffer for Lights
		GLuint copyBuffer;

		// Selector Data of Object
		std::vector<Selected_Object*> selected_objects;

		// Temp Position
		glm::vec2 temp_position;

		// Temp UUID
		uint32_t uuid;

		// Determines Which Coords are Origin, Angle1, and Angle2 for Triangles in That Order
		int order[3] = { 1, 2 ,3 };

		// Offset of Light Object in Buffer
		GLuint light_buffer_offset = 0;

		// List of Connected Libs for Complex Physics Objects
		ConnectedLimb* connected_limbs = nullptr;
		int connected_limbs_count = 0;

		// Initialize Selector for Object
		void initializeSelector();


		// The Following Functions Store Object Vertices and Data


		// Allocate Memory for Vertices
		void allocateSelectorVertices(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects);

		// Generate Vertices
		void genSelectorVertices(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects);

		// Store Selector Data
		Selected_Object* storeSelectorData(DataClass::Data_Object* data_object);

		// Allocate Memory for Horizontal Mask Vertices
		void allocateSelectorVerticesHorizontalMasks(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects);

		// Generate Selector Vertices for Horizontal Masks
		void genSelectorVerticesHorizontalMasks(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects);

		// Store Data for Horizontal Masks
		Selected_Object* storeSelectorDataHorizontalMasks(DataClass::Data_Object* data_object);

		// Allocate Memory for Vertical Mask Vertices
		void allocateSelectorVerticesVerticalMasks(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects);

		// Generate Selector Vertices for Vertical Masks
		void genSelectorVerticesVerticalMasks(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects);

		// Store Data for Vertical Masks
		Selected_Object* storeSelectorDataVerticalMasks(DataClass::Data_Object* data_object);

		// Allocate Memory for Trigger Mask Vertices
		void allocateSelectorVerticesTriggerMasks(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects);

		// Generate Selector Vertices for Trigger Masks
		void genSelectorVerticesTriggerMasks(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects);

		// Store Data for Trigger Masks
		Selected_Object* storeSelectorDataTriggerMasks(DataClass::Data_Object* data_object);

		// Allocate Memory for General Shape Vertices
		void allocateSelectorVerticesShapes(int index, DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects);

		// Generate Selector Vertices for General Shapes
		void genSelectorVerticesShapes(int index, DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects);

		// Store Data for General Shapes
		Selected_Object* storeSelectorDataShapes(int index, DataClass::Data_Object* data_object);

		// Allocate Memory for Light Object Vertices
		void allocateSelectorVerticesLights(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects);

		// Generate Selector Vertices for Light Objects
		void genSelectorVerticesLights(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects);

		// Store Data for Light Objects
		Selected_Object* storeSelectorDataLights(DataClass::Data_Object* data_object);

		// Store Shader Data of Light
		void storeSelectorShaderDataLights(DataClass::Data_Object* data_object);

		// Allocate Memory for Soft Bodies
		void allocateSelectorVerticesSoftBody(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects);

		// Generate Selector Vertices for Soft Bodies
		void genSelectorVerticesSoftBody(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects);

		// Store Data for Soft Bodies
		Selected_Object* storeSelectorDataSoftBody(DataClass::Data_Object* data_object);

		// Allocate Memory for Hinges
		void allocateSelectorVerticesHinge(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects);

		// Generate Selector Vertices for Hinges
		void genSelectorVerticesHinge(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects);

		// Store Data for Hinges
		Selected_Object* storeSelectorDataHinge(DataClass::Data_Object* data_object);

		// Allocate Memory for Entity Vertices
		void alocateSelectorVerticesEntity(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects);

		// Generate Selector Vertices for Entities
		void genSelectorVerticesEntity(DataClass::Data_Object* data_object, Selected_VertexObjects& vertex_objects);

		// Store Data for Entities
		Selected_Object* storeSelectorDataEntity(DataClass::Data_Object* data_object);

		
		// End of Storing Data Functions


		// Uninitialize Selector
		void uninitializeSelector();

		// Generate the Vertex Object for Rotation Pivot Visualizer
		void genPivotVertices();

		// Update the Group Selector
		void updateGroupSelector();

		// Edit Object
		void editObject();

		// Update Hinge
		uint8_t updateHinge(Selected_Object* selected_object);

		// Sorts Vertices of Objects and Applies Rotations
		void sortVertices(bool enable_rotation, Selected_Object* selected_object);

		// Sets the Values of the Temporary Connection Positions
		void setTempConnectionPos(glm::vec2& left, glm::vec2& right);

		// Add a Child Object
		void addChild();

		// Add a SpringMass Node
		void addSpringMassNode(Selected_SpringMassObject* selected_object);

		// Add a SpringMass Spring
		void addSpringMassSpring(Selected_SpringMassObject* selected_object);

		// Add a Hinge Object
		void addHingeObject();

		// Base Function to Clamp Objects
		void clampBase(Selected_Object* selected_object);

		// Clamp Objects
		void clampObjects(bool enabled, float(&endpoints)[8], int Type, int endpoint_count, int extraValue, func function);

		// Clamp Floor Masks
		void clampFloorMasks(float(&endpoints)[8], Render::Objects::UnsavedLevel* unsaved_level, int Type, int i, int extraValue);

		// Clamp Left Wall Masks
		void clampWallMasksLeft(float(&endpoints)[8], Render::Objects::UnsavedLevel* unsaved_level, int Type, int i, int extraValue);

		// Clamp Right Wall Masks
		void clampWallMasksRight(float(&endpoints)[8], Render::Objects::UnsavedLevel* unsaved_level, int Type, int i, int extraValue);

		// Clamp Ceiling Masks
		void clampCeilingMasks(float(&endpoints)[8], Render::Objects::UnsavedLevel* unsaved_level, int Type, int i, int extraValue);

		// Clamp Terrain Objects
		void clampTerrain(int Shape, int Object, Selected_Object* selected_object);

		// Clamp Terrain Objects
		void clampTerrainHelper(float(&endpoints)[8], Render::Objects::UnsavedLevel* unsaved_level, int Type, int i, int extraValue);

		// Abstracted Clamp Terrain
		void abstractedClampTerrain(float(&endpoints)[8], float(&midpoints)[8], int Type, int i, int max, DataClass::Data_Terrain** data);

		// Function Pointers
		func c_floor = &Selector::clampFloorMasks;
		func c_left = &Selector::clampWallMasksLeft;
		func c_right = &Selector::clampWallMasksRight;
		func c_ceiling = &Selector::clampCeilingMasks;
		func c_terrain = &Selector::clampTerrainHelper;

	public:

		// Determines if Selector is Active
		bool active = false;

		// Allows Object to be Resized
		bool enable_resize = false;

		// If False and Active is True, Initialize Selector
		bool editing = false;

		// If True, Selector is Not Selecting but Highlighting an Object to be Selected
		bool highlighting = false;

		// Pivot of Object Rotations
		glm::vec2 pivot;

		// Pointer to Level Object
		Render::Objects::Level* level = nullptr;

		// Pointer to the Object Info Object
		ObjectInfo* info = nullptr;

		// Temporary Connection Values for Connector Objects
		glm::vec2 temp_connection_pos_left = glm::vec2(0.0f);
		glm::vec2 temp_connection_pos_right = glm::vec2(0.0f);

		// Pointer to Unsaved Level The Object Originates From
		Render::Objects::UnsavedLevel* level_of_origin;
		bool originated_from_level = false;

		// Pointer to the Hinge Object Originates From
		Object::Physics::Hinge::Hinge* hinge_of_origin;
		bool originated_from_hinge = false;

		// Pointer to Node Currently Being Edited
		Object::Physics::Soft::Node* node_pointer = nullptr;

		// Currently Highlighted Data Object
		// This Object is Not to be Selected.
		// Only Shows What the User is Moused Over
		DataClass::Data_Object* highlighted_object;

		// Initialize Object
		Selector();

		// Activate the Highlighter for Selector
		void activateHighlighter();

		// Update Object
		void updateSelector();

		// Blitz Object
		void blitzSelector();

		// Blitz Highlighter
		void blitzHighlighter();

		// Deselect Object
		void deselectObject();

		// Deselect Node
		void deselectNode(Selected_SpringMassNode* selected_object);

		// Deselect Spring
		void deselectSpring(Selected_SpringMassSpring* selected_object);

		// Read SpringMass File
		void readSpringMassFile(Selected_SpringMassSpring* selected_object);

		// Read Hinge File
		void readHingeFile();

		// Move Selector With Camera
		void moveWithCamera(Render::Camera::Camera& camera, uint8_t direction);

		// Move Selector With Arrow Keys
		void moveWithArrowKeys(uint8_t direction);

		// Stop Resizing Object
		void stopResizing();

		// Clear Object
		void clear();

		// Store Pointers to Limbs
		void storeLimbPointers(int index, Object::Physics::Soft::Spring* limbs, int limbs_size);

		// Retrieves the Rotation Flag
		bool& retrieveRotation();

		// Returns True if There is Only One Object Selected. No More, No Less
		bool selectedOnlyOne();

		// Adds a Child Object to the Only Selected Object
		void addChildToOnlyOne(DataClass::Data_Object* data_object);

		// Returns the Data Object of the Only Selected Object
		DataClass::Data_Object* getOnlyOne();

		// Make a Copy of the Selected Only One
		void copyOnlyOne();

		// Update Parent of Selected Objects
		void updateParentofSelected(DataClass::Data_Object* new_parent);
	};
}

#endif
