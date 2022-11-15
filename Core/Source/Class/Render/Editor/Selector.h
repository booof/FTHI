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
		// Determines if Object is in Resize Mode
		bool resizing = false;

		// Determines if Mouse Intersects Object
		bool mouse_intersects_object = true;

		std::function<void()> update_functions[11] = {
			[this]()->void { updateRectangle(); },
			[this]()->void { updateTrapezoid(); },
			[this]()->void { updateTriangle(); },
			[this]()->void { updateCircle(); },
			[this]()->void { updateLine(); },
			[this]()->void { updateHorizontalLine(); },
			[this]()->void { updateVerticalLine(); },
			[this]()->void { updateSpringMassObject(); },
			[this]()->void { updateSpringMassNode(); },
			[this]()->void { updateSpringMassSpring(); },
			[this]()->void { updateHinge(); }
		};

		// Vertex Array Object for Selected Object
		GLuint objectVAO, objectVBO;

		// Vertex Array Object for Outline
		GLuint outlineVAO, outlineVBO;

		// Vertex Array Object for Pivot
		GLuint pivotVAO, pivotVBO;

		// Copy Buffer for Lights
		GLuint copyBuffer;

		// Model Loc
		glm::mat4 model;

		// Number of Object Vertices
		short object_vertex_count;

		// Number of Outline Vertices
		short outline_vertex_count;

		// Determines if Selected is an Object with Color and Texture
		bool visualize_object = false;

		// Determines if Selected Object is Composed of Lines Rather Than Triangles
		bool visualize_lines = false;

		// Determines if Selected Object Only Uses a Texture
		bool visualize_texture = false;

		// Determines if Selected Object is a Lighting Object
		bool lighting_object = false;

		// Offsets of Mouse to Object
		float offset_x;
		float offset_y;

		// Position Values of Object
		float* object_x;
		float* object_y;

		// Size Values of Object
		float* object_width;
		float* object_height;

		// Size Modifiers of Object
		float* object_width_modifier;
		float* object_height_modifier;

		// Radius Values of Object
		float* object_radius;
		float* object_inner_radius;

		// Opposite Position Value for Lines
		float* object_opposite_x;
		float* object_opposite_y;

		// Resize Horizontal Condition
		int8_t change_horizontal;

		// Resize Vertical Condition
		int8_t change_vertical;

		// Determines if Object is Activly Being Moved or Resized
		bool moving = false;

		// Triangle Collision Values
		glm::vec2 coords1 = glm::vec2(0.0f);
		glm::vec2 coords2 = glm::vec2(0.0f);
		glm::vec2 coords3 = glm::vec2(0.0f);
		int8_t selected_vertex = 0;
		bool should_sort = false;

		// Determines Which Coords are Origin, Angle1, and Angle2 for Triangles in That Order
		int order[3] = { 1, 2 ,3 };

		// Angle of Mouse to Center of Circle
		float mouse_angle = 0.0f;

		// Values for Line
		float slope;
		float intercept;

		// Offset of Light Object in Buffer
		GLuint light_buffer_offset = 0;

		// List of Nodes for SpringMass
		Object::Physics::Soft::NodeData* node_list;
		int node_count = 0;

		// List of Connected Libs for Complex Physics Objects
		ConnectedLimb* connected_limbs = nullptr;
		int connected_limbs_count = 0;

		// Initialize Selector for Object
		void initializeSelector();


		// The Following Functions Store Object Vertices and Data


		// Allocate Memory for Vertices
		void allocateSelectorVertices();

		// Generate Vertices
		void genSelectorVertices();

		// Store Selector Data
		void storeSelectorData();

		// Allocate Memory for Horizontal Mask Vertices
		void allocateSelectorVerticesHorizontalMasks();

		// Generate Selector Vertices for Horizontal Masks
		void genSelectorVerticesHorizontalMasks();

		// Store Data for Horizontal Masks
		void storeSelectorDataHorizontalMasks();

		// Allocate Memory for Vertical Mask Vertices
		void allocateSelectorVerticesVerticalMasks();

		// Generate Selector Vertices for Vertical Masks
		void genSelectorVerticesVerticalMasks();

		// Store Data for Vertical Masks
		void storeSelectorDataVerticalMasks();

		// Allocate Memory for Trigger Mask Vertices
		void allocateSelectorVerticesTriggerMasks();

		// Generate Selector Vertices for Trigger Masks
		void genSelectorVerticesTriggerMasks();

		// Store Data for Trigger Masks
		void storeSelectorDataTriggerMasks();

		// Allocate Memory for General Shape Vertices
		void allocateSelectorVerticesShapes(int index);

		// Generate Selector Vertices for General Shapes
		void genSelectorVerticesShapes(int index);

		// Store Data for General Shapes
		void storeSelectorDataShapes(int index);

		// Allocate Memory for Light Object Vertices
		void allocateSelectorVerticesLights();

		// Generate Selector Vertices for Light Objects
		void genSelectorVerticesLights();

		// Store Data for Light Objects
		void storeSelectorDataLights();

		// Store Shader Data of Light
		void storeSelectorShaderDataLights();

		// Allocate Memory for Soft Bodies
		void allocateSelectorVerticesSoftBody();

		// Generate Selector Vertices for Soft Bodies
		void genSelectorVerticesSoftBody();

		// Store Data for Soft Bodies
		void storeSelectorDataSoftBody();

		// Allocate Memory for Hinges
		void allocateSelectorVerticesHinge();

		// Generate Selector Vertices for Hinges
		void genSelectorVerticesHinge();

		// Store Data for Hinges
		void storeSelectorDataHinge();

		// Allocate Memory for Entity Vertices
		void alocateSelectorVerticesEntity();

		// Generate Selector Vertices for Entities
		void genSelectorVerticesEntity();

		// Store Data for Entities
		void storeSelectorDataEntity();

		// Object Info for Shapes
		void getShapeInfo(Shape::Shape* shape);

		
		// End of Storing Data Functions


		// Uninitialize Selector
		void uninitializeSelector();

		// Change Outline Color for Resize Mode
		void outlineForResize();

		// Change Outline Color for Move Mode
		void outlineForMove();

		// Change Outline Color for When Mouse is Not in Object
		void outlineForNotSelected();

		// Change Color of Object
		void outlineChangeColor(float* colors);

		// Generate the Vertex Object for Rotation Pivot Visualizer
		void genPivotVertices();

		// Edit Object
		void editObject();

		// Test Resize of Rectangle
		void testResizeRectangle(bool enable_horizontal, bool enable_vertical);

		// Move Rectangle
		void moveRectangle(bool enable_negative);

		// Update Rectangle
		void updateRectangle();

		// Test Resize of Trapezoid
		void testResizeTrapezoid();

		// Move Trapezoid
		void moveTrapezoid();

		// Update Trapezoid
		void updateTrapezoid();

		// Test Resize of Triangle
		void testResizeTriangle();

		// Move Triangle
		void moveTriangle();

		// Update Triangle
		void updateTriangle();

		// Test Resize of Circle
		void testResizeCircle(float& distance, float& delta_w, float& delta_h);

		// Move Circle
		void moveCircle();

		// Update Circle
		void updateCircle();

		// Test Resize of Line
		void testResizeLine();

		// Move Line
		void moveLine();

		// Update Line
		void updateLine();

		// Update Horizontal Line
		void updateHorizontalLine();

		// Update Vertical Line
		void updateVerticalLine();

		// Update SpringMass Object
		void updateSpringMassObject();

		// Update SpringMass Node
		void updateSpringMassNode();

		// Update SpringMass Spring
		void updateSpringMassSpring();

		// Update Hinge
		void updateHinge();

		// Sorts Vertices of Objects and Applies Rotations
		void sortVertices(bool enable_rotation);

		// Add a Child Object
		void addChild();

		// Add a SpringMass Node
		void addSpringMassNode();

		// Add a SpringMass Spring
		void addSpringMassSpring();

		// Add a Hinge Object
		void addHingeObject();

		// Base Function to Clamp Objects
		void clampBase();

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
		void clampTerrain(int Shape, int Object);

		// Clamp Terrain Objects
		void clampTerrainHelper(float(&endpoints)[8], Render::Objects::UnsavedLevel* unsaved_level, int Type, int i, int extraValue);

		// Abstracted Clamp Terrain
		void abstractedClampTerrain(float(&endpoints)[8], float(&midpoints)[8], int Type, int i, int max, Object::Terrain::TerrainBase** data);

		// Function Pointers
		func c_floor = &Selector::clampFloorMasks;
		func c_left = &Selector::clampWallMasksLeft;
		func c_right = &Selector::clampWallMasksRight;
		func c_ceiling = &Selector::clampCeilingMasks;
		func c_terrain = &Selector::clampTerrainHelper;

	public:

		// Determines if Selector is Active
		bool active = false;

		// Determine if Selector is Being Used
		//bool initialized = false;

		// Allows Object to be Resized
		bool enable_resize = false;

		// If False and Active is True, Initialize Selector
		bool editing = false;

		// If True, Selector is Not Selecting but Highlighting an Object to be Selected
		bool highlighting = false;

		// Determines if Object is Being Rotated
		bool rotating = false;

		// Pivot of Object Rotations
		glm::vec2 pivot;

		// Pointer to Level Object
		Render::Objects::Level* level = nullptr;

		// Pointer to the Object Info Object
		ObjectInfo* info = nullptr;

		// Pointer to Unsaved Level The Object Originates From
		Render::Objects::UnsavedLevel* level_of_origin;
		bool originated_from_level = false;

		// Pointer to the Hinge Object Originates From
		Object::Physics::Hinge::Hinge* hinge_of_origin;
		bool originated_from_hinge = false;

		// Initial Positions for Connection Objects
		glm::vec2 connection_pos_left;
		glm::vec2 connection_pos_right;

		// Pointer to Node Currently Being Edited
		Object::Physics::Soft::Node* node_pointer = nullptr;

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
		void deselectNode();

		// Deselect Spring
		void deselectSpring();

		// Read SpringMass File
		void readSpringMassFile();

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

		// Return Position of Object
		glm::vec2 getObjectPosition();

		// Store Pointers to Limbs
		void storeLimbPointers(int index, Object::Physics::Soft::Spring* limbs, int limbs_size);
	};
}

#endif
