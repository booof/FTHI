#ifndef GLOBALS_H
#define GLOBALS_H

#include "ExternalLibs.h"
#include "Macros.h"
#include "Constants.h"

class ScrollBar;

namespace GUI
{
	class Box;
}

namespace Editor
{
	class EditorOptions;
}

namespace Shader
{
	class Shader;
}

namespace Struct
{
	//class Character;
}

namespace Render::Camera
{
	class Camera;
}

namespace Object
{
	class Object;
}

#include "Class/Render/Struct/Texture.h"

typedef void(__stdcall* bindFunctionPointerPointer)(int, Object::Object*);
typedef void(__stdcall* updateGlobalScriptsPointer)();

namespace Global
{
	// The Path to the Engine
	extern std::string engine_path;

	// The Path to the Project Directory
	extern std::string project_directory_path;

	// Script Functions
	extern bindFunctionPointerPointer bindFunctionPointer;
	extern updateGlobalScriptsPointer updateGlobalScripts;

	// Path to Level Data
	extern std::string project_file_path;
	extern std::string level_data_path;
	extern std::string editor_level_data_path;
	extern std::string project_map_path;
	extern std::string project_scripts_path;
	extern std::string script_folder_path;

	// RNG
	extern uint64_t rng;

	// Font Variables
	extern FT_Library FREETYPE_LIBRARY;
	extern FT_Face Arial;
	extern FT_Face CambriaMath;
	extern FT_Face SEGOEUI;
	extern FT_Face TEST;

	// The Types of Cursors
	enum class CURSORS : uint8_t
	{
		ARROW,
		HORIZONTAL_RESIZE,
		VERTICAL_RESIZE,
		HAND,
		TYPE,
		POSITIVE_SLOPE_RESIZE,
		NEGATIVE_SLOPE_RESIZE,
		POINT,
		LOCK
	};

	// Cursor Variables
	extern std::map<std::string, GLFWcursor*> Mouse_Textures;
	extern CURSORS Selected_Cursor;
	extern CURSORS Default_Cursor;

	// List of Keys and Their States
	extern bool Keys[1024];

	// The Text Object Used for Typing
	extern std::string* text;
	extern std::string initial_text;
	extern int textModifier;
	extern unsigned int textIndex;
	extern GUI::Box* text_box;
	extern bool texting;
	extern bool stoped_texting;

	// The ScrollBar Object Used for Scrolling
	extern ScrollBar* scroll_bar;

	// Values of Current Screen Width and Height
	extern unsigned int screenWidth;
	extern unsigned int screenHeight;

	// Number that Scales Numbers Based on Window Size
	extern float screenScalar; // Default is 1 when screenHeight is 720

	// Half of Projection Width to Ensure Projection Width and Height Ratio Remains the Same
	extern float halfScalarX;

	// Determines if Static Objects Should Resize
	extern bool framebufferResize;

	// Shows if Cursor Moved
	extern bool cursor_Move;

	// Cursor Locations
	extern double mouseX;
	extern double mouseY;

	// Change in Mouse Positions
	extern glm::vec2 deltaMouse;

	// Relative Cursor Locations in World Space
	extern float mouseRelativeX;
	extern float mouseRelativeY;

	// Mouse Button Variables
	extern bool LeftClick;
	extern bool RightClick;

	// Boolean to Visualize Level Boarder
	extern bool level_border;

	// Boolean to Visualize Normals
	extern bool normals;

	// Boolean to Lock an Object
	extern bool lock_object;

	// Determines How Far the Projection Matrix Should be Zoomed
	extern float zoom_scale;

	// If True, Zoom Scale Changed
	extern bool zoom;

	// Pointer to Camera
	extern Render::Camera::Camera* camera_pointer;

#ifdef EDITOR

	// Determine is Currently in Editing Mode
	extern bool editing;

	// Editor Options
	extern Editor::EditorOptions* editor_options;

	// Object Index Counter
	extern uint32_t object_index_counter;

	// UUID Counter
	extern uint32_t uuid_counter;

	// Light Textures
	extern std::map<std::string, Struct::SingleTexture> Visual_Textures;

#endif

	// Window Object
	extern GLFWwindow* window;

	// Shader Locations for Object Draw Shader
	extern GLuint matrixLocObject;
	extern GLuint textureLocObject; 
	extern GLuint materialLocObject;

	// View Matrix
	extern glm::mat4 view;

	// Projection Matrix
	extern glm::mat4 projection;
	extern glm::mat4 projectionStatic;

	// Shader Locations

	// Model
	extern GLuint modelLocColor;
	extern GLuint modelLocColorStatic;
	extern GLuint modelLocTexture;
	extern GLuint modelLocTextureStatic;
	extern GLuint modelLocSingleColor;
	extern GLuint modelLocNormal;
	extern GLuint modelLocParticleColor;
	extern GLuint modelLocParticleTexture;
	extern GLuint modelLocObject;
	extern GLuint modelLocObjectStatic;

	// Static
	extern GLuint staticLocColor;
	extern GLuint staticLocTexture;
	extern GLuint staticLocParticleColor;
	extern GLuint staticLocParticleTexture;

	// Direction
	extern GLuint directionLoc;

	// Brightness
	extern GLuint brightnessLoc;

	// Alpha
	extern GLuint alphaLocColor;
	extern GLuint alphaLocTexture;
	extern GLuint alphaLocParticleColor;
	extern GLuint alphaLocParticleTexture;

	// Textures
	extern GLuint textureLayerLocTexture;
	extern GLuint textureLayerLocTextureStatic;

	// Object Static Textures
	extern GLuint objectStaticColorTexture;
	extern GLuint objectStaticMaterialTexture;
	extern GLuint objectStaticParallaxTexture;

	// Object Static Values
	extern GLuint objectStaticViewPos;
	extern GLuint objectStaticLayer;
	
	// General Matrix for Objects
	extern GLuint objectMatrixLoc;
	extern GLuint objectStaticMatrixLoc;

	// Ambient
	extern GLuint ambientLocColor;
	extern GLuint ambientLocTexture;

	// Specular
	extern GLuint specularLocColor;
	extern GLuint specularLocTexture;

	// Shininess
	extern GLuint shininessLocColor;
	extern GLuint shininessLocTexture;

	// Fonts
	extern GLuint projectionLocFont;
	extern GLuint textureLocFont;
	extern GLuint texcolorLocFont;

	// Relative Fonts
	extern GLuint projectionLocRelativeFont;
	extern GLuint modelLocRelativeFont;
	extern GLuint textureLocRelativeFont;
	extern GLuint texcolorLocRelativeFont;

	// Global Fonts
	extern GLuint projectionLocGlobalFont;
	extern GLuint viewLocGlobalFont;
	extern GLuint textureLocGlobalFont;
	extern GLuint textcolorLocGlobalFont;

	// Bloom
	extern GLuint bloomLoc;

	// Particle Generator
	extern GLuint ParticleOffsetLocColor;
	extern GLuint ParticleOffsetLocTexture;

	// Level Border Objects
	extern GLuint level_border_VAO;
	extern GLuint level_border_camera_pos_loc;
	extern GLuint level_border_screen_width_loc;
	extern GLuint level_border_screen_height_loc;

	// Shaders List
	extern Shader::Shader FramebufferShader_Core;
	extern Shader::Shader FramebufferShader_HDR;
	extern Shader::Shader FramebufferShader_Bloom;
	extern Shader::Shader StencilbufferShader;
	extern Shader::Shader NormalShader;
	extern Shader::Shader objectShader;
	extern Shader::Shader objectShaderStatic;
	extern Shader::Shader colorShader;
	extern Shader::Shader colorShaderStatic;
	extern Shader::Shader colorShaderStaticLine;
	extern Shader::Shader texShader;
	extern Shader::Shader texShaderStatic;
	extern Shader::Shader fontShader;
	extern Shader::Shader fontOffsetShader;
	extern Shader::Shader fontGlobalShader;
	extern Shader::Shader particleShaderColor;
	extern Shader::Shader particleShaderTexture;
	extern Shader::Shader borderShader;

	// Friction Coefficients
	extern float static_friction[Constant::MATERIALS_PERMUTATIONS]; // Index: (MATERIALS_COUNT * MATERIAL_INDEX1) + (MATERIALS_INDEX2 - MATERIALS_INDEX1) 
	extern float kinetic_friction[Constant::MATERIALS_PERMUTATIONS]; // MATERIAL_INDEX1 will always be the minimum between the two indicies

	// Maximum Number of Particles to be Used Each Frame
	extern int Global_Max_Particles;
	extern int Global_Particles_Loaded;
	extern glm::vec4 Particle_Positions[20000];

	// Framebuffers
	extern GLuint Frame_Buffer_Object_Core;
	extern GLuint Frame_Buffer_Object_HDR; 
	extern GLuint Frame_Buffer_Object_Multisample;
	extern GLuint Frame_Buffer_Object_Bloom[2];

	// Framebuffer Textures
	extern GLuint Frame_Buffer_Texture_Core;
	extern GLuint Frame_Buffer_Texture_HDR;
	extern GLuint Frame_Buffer_Texture_Bloom;
	extern GLuint Frame_Buffer_Texture_Multisample;
	extern GLuint Frame_Buffer_Texture_PingPong[2];

	// Screen Objects
	extern GLuint Render_Buffer_Multisample;
	extern GLuint screenVAO;
	extern GLuint screenVBO;
	extern GLuint windowTexture;

	// Uniform Buffers
	extern GLuint MatricesBlock;
	extern GLuint DirectionalBuffer;
	extern GLuint PointBuffer;
	extern GLuint SpotBuffer;
	extern GLuint BeamBuffer;
	extern GLuint InstanceBuffer;
	extern GLuint PhysicsInstanceBuffer;
	extern GLuint ParticleBuffer;

	// Font Buffer Objects
	extern GLuint fontVAO;
	extern GLuint fontVBO;

	// Fonts
	extern std::map<std::string, int> FontList;
	extern std::map<GLchar, Struct::Character> Current_Font;

	// Audio Device
	extern ALCdevice* audio_device;

	// Audio Context
	extern ALCcontext* audio_context;

	// Time Variables
	extern float deltaTime; // Time Between Current and Last Frames
	extern double lastFrame; // Current Time of Most Recent Frame
	extern double framebyframeStart; // At What Time The Last Frame By Frame Pause
	extern double framebyframeOffset; // Offset Caused by Frame By Frame Pausing
	extern double lastTime; // Time Since Last Second
	extern int frameCount; // Number of Frames
	extern int last_frameCount; // Last Frame Count in a Second

	// Frame by Frame Debugging
	extern bool frame_step;
	extern bool frame_run;
	extern bool frame_by_frame;

	// Camera Speed Multiplier
	extern float camera_speed_multiplier;

	// Shift Speed Multiplier
	extern float shift_speed_multiplier;

	// Flag to Determine if Level Should Reload All
	extern bool reload_all;

	// Determines if the Engine is Paused or Not
	extern bool paused;

	// The Elusive Breakpoint
	extern bool activate_elusive_breakpoint;
}

#endif
