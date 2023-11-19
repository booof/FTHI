#pragma once
#ifndef OBJECT_INFO_H
#define OBJECT_INFO_H

#include "ExternalLibs.h"

// This Object Displays the Info of the Currently Selected Object in the Top Right Corner

// Idea: The size of Background Vertex Object Changes as More or Less Text is Used

// Declaration for Level Object
namespace Render::Objects
{
	class Level;
}

namespace Editor
{
	// The Object Info Class
	class ObjectInfo
	{
		// The Different Types of Text Objects
		enum class TEXT_OBJECTS : uint8_t
		{
			TEXT_STRING = 1,
			TEXT_SINGLE_VALUE,
			TEXT_DOUBLE_VALUE,
			TEXT_BOOL,
			TEXT_COLOR,
			TEXT_POSITION
		};

		// Master Class for Text Objects
		class TextMaster
		{

		protected:

			// Identifier
			std::string identifier = "";

			// Identifier Color
			glm::vec4 identifier_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

			// Size of Text
			float text_size = 0;

			// Scale of Text
			float text_scale = 0.145f;

			// Determins if Value Should be Interpreted as an Int
			bool interpret_as_int = false;

		public:

			// Returns the Type of Text Object
			virtual TEXT_OBJECTS getTextType() = 0;

			// Blitz Text Function
			virtual void blitzText(float x, float& y, float max_height) = 0;

			// Get Size of Text
			virtual float getTextSize(float max_width) = 0;

			// Set Size and Scale
			void setSizeScale(float size, float scale);

			// Returns the Size (Width) of the Text
			float returnSize();

			// Returns the Scale (Size) of the Text
			float returnScale();

			// Set Scale of Text
			void setScale(float new_scale);

			// Returns the Height Offset of the Text
			float returnHeightOffset(float max_height);
		};

		// Class for String Text Objects
		class TextString : public TextMaster
		{
			// Value
			std::string* value_string;

			// Value Color
			glm::vec4 color;

		public:

			// Constructor
			TextString(std::string identifier_, glm::vec4 identifier_color_, std::string* value_, glm::vec4 value_color_);

			// Returns the Type of Text Object
			TEXT_OBJECTS getTextType();

			// Blitz Text
			void blitzText(float x, float& y, float max_height);

			// Get Size of Text
			float getTextSize(float max_width);

			// Set Value
			void setValue(std::string* value_);
		};

		// Class for Single Value Text Objects
		class TextSingleValue : public TextMaster
		{
			// Value 
			void* value;

			// Value String
			std::string value_string;

			// Value Color
			glm::vec4 color;

		public:

			// Constructor
			TextSingleValue(std::string identifier_, glm::vec4 identifier_color_, void* value_, glm::vec4 value_color_, bool is_int);

			// Returns the Type of Text Object
			TEXT_OBJECTS getTextType();

			// Blitz Text
			void blitzText(float x, float& y, float max_height);

			// Get Size of Text
			float getTextSize(float max_width);

			// Set Value
			void setValue(void* value_);
		};

		// Class for Double Value Text Objects
		class TextDoubleValue : public TextMaster
		{
			// First Secondary Identifier
			std::string secondary_identifier_1 = "";

			// Second  Secondary Identifier
			std::string secondary_identifier_2 = "";

			// Value 1
			void* value1;

			// Value String 1
			std::string value_string_1;

			// Value 2
			void* value2;

			// Value String 2
			std::string value_string_2;

			// Secondary Identifier Color 1
			glm::vec4 secondary_identifier_color_1;

			// Secondary Identifier Color 2
			glm::vec4 secondary_identifier_color_2;

			// Value Color
			glm::vec4 value_color;

		public:

			// Constructor
			TextDoubleValue(std::string identifier_, glm::vec4 identifier_color_, std::string secondary_identifier_1_, glm::vec4 secondary_color_1_,
				std::string secondary_identifier_2_, glm::vec4 secondary_color_2_, void* value1_, void* value2_, glm::vec4 value_color_, bool is_int);

			// Returns the Type of Text Object
			TEXT_OBJECTS getTextType();

			// Blitz Text
			void blitzText(float x, float& y, float max_height);

			// Get Size of Text
			float getTextSize(float max_width);

			// Set Values
			void setValues(void* value1_, void* value2_);
		};

		// Class for Boolean Text Objects
		class TextBoolean : public TextMaster
		{
			// Value String
			std::string value_string;

			// Value
			bool* value;

			// Value Color
			glm::vec4 color;

		public:

			// Constructor
			TextBoolean(std::string identifier_, glm::vec4 identifier_color_, bool* value_, glm::vec4 value_color_);

			// Returns the Type of Text Object
			TEXT_OBJECTS getTextType();

			// Blitz Text
			void blitzText(float x, float& y, float max_height);

			// Get Size of Text
			float getTextSize(float max_width);

			// Set Value
			void setValue(bool* value_);
		};

		// Class for Color Values
		class TextColor : public TextMaster
		{
			// The Amount of Color Values Loaded
			static uint8_t color_count;

			// Colors
			glm::vec4* value;

			// Offset in Vertex Array
			int vao_offset;

		public:

			// Vertex Object for All Instances Combined
			static GLuint VAO, VBO;

			// Constructor
			TextColor(std::string identifier_, glm::vec4 identifier_color_, glm::vec4* value_);

			// Returns the Type of Text Object
			TEXT_OBJECTS getTextType();

			// Blitz Text
			void blitzText(float x, float& y, float max_height);

			// Get Size of Text
			float getTextSize(float max_width);

			// Set Value
			void setValue(glm::vec4* value_);

			// Returns True if There is At Least 1 Color Value
			static bool containsColor();

			// Reset the Color Count
			static void resetColorCount();

			// Get the Color Count
			static uint8_t getColorCount();

			// Returns the Color
			glm::vec4& getColor();

			// Set the VAO Offset
			void setVAOOffset(int new_offset);
		};

		// Class for Position Values
		class TextPosition : public TextMaster
		{
			// Pointer to the Level Object
			Render::Objects::Level* level;

			// Label Colors
			glm::vec4 label_color_1;
			glm::vec4 label_color_2;

			// Value Color
			glm::vec4 value_color;

			// Position Data
			glm::vec2* position_data;

			// Position Strings
			std::string position_string_x = "";
			std::string position_string_y = "";

		public:

			// Constructor
			TextPosition(std::string identifier_, glm::vec4 identifier_color_, glm::vec4 label_color_1_, glm::vec4 label_color_2_, glm::vec4 value_color_, glm::vec2* position, bool is_int);

			// Returns the Type of Text Object
			TEXT_OBJECTS getTextType();

			// Blitz Text
			void blitzText(float x, float& y, float max_height);

			// Get Size of Text
			float getTextSize(float max_width);

			// Set Values
			void setValue(glm::vec2* position);
		};

		// Vertex Object
		GLuint VAO, VBO;

		// Type of the Object
		std::string type_text = "";

		// Scale of Object Type Text
		float type_scale = 0.25f;

		// Color of the Object Type Text
		glm::vec4 type_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		// Container for Text
		std::vector<TextMaster*> text_objects;

		// Determines if Object Should be Restructured (After Adding/Removing Text Object)
		bool should_restructure = false;

		// The Position of the Object Type Text
		glm::vec2 type_text_position;

		// The Position of the First Text Object
		glm::vec2 first_text_position;

		// The Current Maximum Size of Text
		float max_text_size = 0.0f;

		// The Current Height of the Object
		float current_height = 0.0f;

		// Model Matrix
		glm::mat4 model;

		// Max Width
		float max_width = 1.0f;

		// Max Height
		float max_height = 1.0f;

		// Max Scale
		float max_scale = 1.0;

		// Generate Color Vertices
		void generateColorVertices();

	public:

		// Determines if Object is Active
		bool active = false;

		// Constructor
		ObjectInfo();

		// Draw Info
		void drawInfo();

		// Set Object Type
		void setObjectType(std::string type, glm::vec4 color);

		// Add Text Value
		void addTextValue(std::string identifier, glm::vec4 identifier_color, std::string* value, glm::vec4 value_color);

		// Edit Single Value
		void editSingleValue(int index, std::string* value);

		// Add Single Value 
		void addSingleValue(std::string identifier, glm::vec4 identifier_color, void* value, glm::vec4 value_color, bool is_int);

		// Edit Single Value
		void editSingleValue(int index, void* value);

		// Add Double Value
		void addDoubleValue(std::string identifier, glm::vec4 identifier_color, std::string secondary_identifier_1, glm::vec4 secondary_color_1, 
			std::string secondary_identifier_2, glm::vec4 secondary_color_2, void* value1, void* value2, glm::vec4 value_color, bool is_int);

		// Edit Double Value
		void editDoubleValue(int index, void* value1, void* value2);

		// Add Boolean Value
		void addBooleanValue(std::string identifier, glm::vec4 identifier_color, bool* value, glm::vec4 value_color);

		// Edit Boolean Value
		void editBooleanValue(int index, bool* value);

		// Add Color Value
		void addColorValue(std::string identifier, glm::vec4 identifier_color, glm::vec4* value, bool generate);

		// Edit Color Value
		void editColorValue(int index, glm::vec4* value);

		// Add Position Value
		void addPositionValue(std::string identifier, glm::vec4 identifier_color, glm::vec4 label_color_1, glm::vec4 label_color_2, glm::vec4 value_color, glm::vec2* value, bool is_int);

		// Edit Position Data
		void editPositionValue(int index, glm::vec2* value);

		// Remove Value
		void removeValueAtIndex(int index);

		// Clear All Text
		void clearAll();

		// Force a Resize Operation (Useful When Selector is Modifing Variables)
		void forceResize();
	};
}

#endif