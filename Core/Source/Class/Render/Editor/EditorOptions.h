#pragma once
#ifndef EDITOR_OPTIONS_H
#define EDITOR_OPTIONS_H

#include "ExternalLibs.h"
#include "Macros.h"

// GUI Objects
#include "Class/Render/GUI/VerticalScrollBar.h"
#include "Class/Render/GUI/HorizontalScrollBar.h"
#include "Class/Render/GUI/Box.h"
#include "Class/Render/GUI/ToggleGroup.h"
#include "Class/Render/GUI/MasterElement.h"

namespace Render::Objects
{
	class Level;
}

namespace Editor
{
	// Function Pointers to Text Render Functions
	//typedef void (Editor::EditorOptions::*initialize_vertices)(void);
	//typedef void (Editor:: EditorOptions::*text_func)(void);

	class EditorOptions
	{
	public:

		// Determines if Window is Active
		bool Active = false;

		// List of Options and Default Values

		// Common Options
		uint8_t option_double_click = true; // Click Once to Select, Click Again to Move/Resize
		uint8_t option_default_clamp = true; // New Objects Automatically Have Clamp Enabled
		uint8_t option_default_lock = false; // New Objects Are Automatically Locked
		uint8_t option_visualize_masks = true; // Determines if Collision Masks Should be Visualized
		uint8_t option_visualize_lights = true; // Determines if Light Identifiers Should be Visualized
		uint8_t option_clamp_to_clamp = false; // Objects Only Clamp to Objects That Have Clamped Enabled
		uint8_t option_enable_lighting = true; // Determines if Lighting Should be Calculated or Not (Use Static Shader if False)
		uint8_t option_disable_pass_through = false; // Disables Side Opposite of Selected Side From Moving if Mouse Passes Through 
		uint8_t option_resize = true; // Determines if Objects Are Able to be Resized
		uint8_t option_fps; // 0 = FPS is Not Dispalyed, 1 = FPS is displayed on Debug Window, 2 = FPS is Displayed as Text in Window
		uint8_t option_default_to_window = false; // If true, creating a new object reopens editor window for object
		uint8_t option_display_springmass_components = true; // If true, SpringMass Objects Will be Displayed Through Nodes and Springs

		// Object Options
		uint8_t option_interactable[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // 0 = Visable and Interactable, 1 = Visable but Not Interactable, 2 = Invisible and Uninteractable

		// Extra Options
		float option_camera_speed = 1.0f;
		float option_alt_camera_speed = 0.2f;
		float option_shift_speed = 1.0f;
		float option_scroll_speed = 3.0f;
		float option_object_info_max_width_percent = 1.0f;
		float option_object_info_text_size_percent = 1.0f;

		// Pointer to Level Object
		Render::Objects::Level* level;

		// Generate Object
		EditorOptions();

		// Initialize Object
		void initialize();

		// Read From File
		void read();

		// Store Data in File
		void write();

		// Creates the GUI
		void initialize_gui();

		// Draws Window
		void Blitz();

		// Get Pointer to Main Level Object
		void getLevelPointer(Render::Objects::Level& level_);

	private:
		
		// Render Text
		void DisplayText();

		// Common Text
		void Render_Common_Text();

		// Object Text
		void Render_Object_Text();

		// Function Text
		void Render_Function_Text();

		// Extra Text
		void Render_Extra_Text();

		// Update Window
		void Update();

		// Common Options
		void updateBoxes(glm::vec2 cursor);

		// Common Vertices
		void Common_Vertices();

		// Object Vertices
		void Object_Vertices();

		// Function Vertices
		void Function_Vertices();

		// Extra Vertices
		void Extra_Vertices();

		// Generate Two Grouped Boxes With Boolean Data
		void generateBooleanBoxes(float* x_offsets, float& y_index, float y_difference, float temp_width, float temp_height, int begin, int end, int& box_index, int& group_index);

		// Generate Multiple Grouped Boxes With Custom Text
		void generateMultipleBoxes(uint8_t stride, float* x_offsets, float& y_index, float y_difference, float temp_width, float temp_height, int begin, int end, int& box_index, int& group_index, std::string* text);

		// Waits Until a User Types and Enters a Response
		std::string& querry(std::string message, std::string input, int typing_mode);

		// Lock All
		void lockAll();

		// Lock Select
		void lockSelect();

		// Unlock All
		void unlockAll();

		// Unlock Select
		void unlockSelect();

		// Lock/Unlock All Function
		void lockUnlockFunction(bool lock, bool specific);

		// Lock/Unlock Level
		void lockUnlockLevel(glm::vec2 level_coords, bool lock);

		// Delete Level
		void deleteLevel();

		// Cull Levels
		void cullEmptyLevels();

		// Resets All Values to the Default Value
		void resetToDefault();

		// Background Vertex Objectes
		GLuint backgroundVAO, backgroundVBO;

		// Lines Vertex Objects
		GLuint linesVAO, linesVBO;

		// Selected Options Vertex Objects
		GLuint selectedVAO, selectedVBO;

		// Highlighter Vertex Object
		GLuint highlighterVAO, highlighterVBO;

		// Popup Vertex Object
		GLuint popupVAO, popupVBO;

		// Model Matrix
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 secondary_model = glm::mat4(1.0f);

		// Position Of Window
		glm::vec2 position = glm::vec2(0.0f, 0.0f);

		// Scale of All Widnow Objects
		float Scale = 1.0f;

		// Size of Object
		float width = 0.0f, height = 0.0f;

		// Height of Full Options Part of Window
		float options_height = 0.0f;

		// Difference Between Body Height of Window and Options Height
		float height_difference = 0.0f;

		// Offset of Y Position
		float y_offset = 0.0f;

		// Scrollbar
		GUI::VerticalScrollBar bar;

		// Offset of Scroll Bar
		float bar_offset = 0.0f;

		// Number of Triangles to Draw
		float select_triangles = 0, lines_triangles = 0;

		// Current Mode
		int mode = 0;

		// Determines if Mode is Different
		bool new_mode = false;

		// Determines if an Option is Being Highlighted
		bool Highlight = false;

		// Array of Functions That Draw Mode-Specific Vertices
		//initialize_vertices draw_vertices[4] = { &EditorOptions::Common_Vertices, &EditorOptions::Object_Vertices, &EditorOptions::Function_Vertices, &EditorOptions::Extra_Vertices };

		std::function<void()> draw_vertices[4] = { [this]()->void {this->Common_Vertices(); }, [this]()->void {this->Object_Vertices(); }, [this]()->void {this->Function_Vertices(); }, [this]()->void {this->Extra_Vertices(); } };

		// Array of Text Rendering Functions
		std::function<void()> text_renderers[4] = { [this]()->void {this->Render_Common_Text(); }, [this]()->void {this->Render_Object_Text(); }, [this]()->void {this->Render_Function_Text(); }, [this]()->void {this->Render_Extra_Text(); } };

		// Arrays of Objects
		GUI::Box* box_array;
		GUI::ToggleGroup* group_array;

		// Size of Object Arrays
		uint8_t box_count;
		uint8_t group_count;

		// Test Horizontal Scroll Bar
		GUI::HorizontalScrollBar test_bar;
		GUI::HorizontalScrollBar test_bar2;
		GUI::MasterElement master;
	};
}

#endif
