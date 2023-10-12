#include "Globals.h"

#include "ExternalLibs.h"
#include "Macros.h"
#include "Class/Render/Shader/Shader.h"
#include "Class/Render/Struct/Texture.h"

namespace GUI
{
	class Box;
}

namespace Editor
{
	class EditorOptions;
}

namespace Render::Camera
{
	class Camera;
}

namespace Global
{
	// The Path to the Engine
	std::string engine_path = "";

	// The Path to the Project Directory
	std::string default_project_directory_path = "";

	// Script Functions
	bindFunctionPointerPointer bindFunctionPointer;
	updateGlobalScriptsPointer updateGlobalScripts;

	// Path to Level Data
	std::string project_file_path = "";
	std::string project_map_path = "";
	std::string project_scripts_path = "";
	std::string script_folder_path = "";
	std::string project_solution_path = "";
	std::string project_symbols_path = "";
	std::string project_resources_path = "";
	std::string project_name = "";

	// RNG
	uint64_t rng;

	// Font Variables
	FT_Library FREETYPE_LIBRARY;
	FT_Face Arial;
	FT_Face CambriaMath;
	FT_Face SEGOEUI;
	FT_Face TEST;

	// Cursor Variables
	std::map<std::string, GLFWcursor*> Mouse_Textures;
	CURSORS Selected_Cursor = CURSORS::ARROW;
	CURSORS Default_Cursor = CURSORS::ARROW;

	// List of Keys and Their States
	bool Keys[1024];

	// The ScrollBar Object Used for Scrolling
	GUI::ScrollBar* scroll_bar = nullptr;

	// Values of Current Screen Width and Height
	unsigned int screenWidth = 1280;
	unsigned int screenHeight = 720;

	// Number that Scales Numbers Based on Window Size
	float screenScalar = 1.0; // Default is 1 when screenHeight is 720

	// Half of Projection Width to Ensure Projection Width and Height Ratio Remains the Same
	float halfScalarX = 90;

	// Determines if Static Objects Should Resize
	bool framebufferResize = false;

	// Shows if Cursor Moved
	bool cursor_Move = false;

	// Raw Cursor Locations
	double rawX = 0.0;
	double rawY = 0.0;

	// Cursor Locations
	double mouseX = 0.0;
	double mouseY = 0.0;

	// Change in Mouse Positions
	glm::vec2 deltaMouse = glm::vec2(0.0f, 0.0f);

	// Relative Cursor Locations in World Space
	float mouseRelativeX, mouseRelativeY;

	// Mouse Button Variables
	bool LeftClick = false;
	bool RightClick = false;
	bool DoubleClick = false;

	// Boolean to Visualize Level Boarder
	bool level_border = true;

	// Boolean to Visualize Normals
	bool normals = false;

	// Boolean to Lock an Object
	bool lock_object = false;

	// Determines How Far the Projection Matrix Should be Zoomed
	float zoom_scale = 0.2f;

	// If True, Zoom Scale Changed
	bool zoom = false;

	// Pointer to Camera
	Render::Camera::Camera* camera_pointer;

#ifdef EDITOR

	// Determine is Currently in Editing Mode
	bool editing = true;

	// Determine if Currently in Debugging Mode
	bool debugging = false;

	// The Connection Point to Dot Net
	HINSTANCE framework_handle = NULL;

	// Editor Options
	Editor::EditorOptions* editor_options;

	// Object Index Counter
	uint32_t object_index_counter = 1;

	// UUID Counter
	uint32_t uuid_counter = 1;

	// Light Textures
	std::map<std::string, Struct::SingleTexture> Visual_Textures;

	// Flag to Reload Lights
	bool reload_lights = false;

	// Dummy Vec2 to be used as a Safe Nullptr
	glm::vec2 dummy_vec2 = glm::vec2(0.0f, 0.0f);

#endif

	// Window Object
	GLFWwindow* window;

	// Shader Locations for Object Draw Shader
	GLuint matrixLocObject = 1;
	GLuint textureLocObject, materialLocObject;

	// View Matrix
	glm::mat4 view = glm::mat4(1.0f);

	// Projection Matrix
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 projectionStatic = glm::mat4(1.0f);

	// Shader Locations

	// Model
	GLuint modelLocColor, modelLocColorStatic, modelLocTexture, modelLocTextureStatic, modelLocSingleColor, modelLocNormal, modelLocParticleColor, modelLocParticleTexture, modelLocObject, modelLocObjectStatic;

	// Static
	GLuint staticLocColor, staticLocTexture, staticLocParticleColor, staticLocParticleTexture;

	// Direction
	GLuint directionLoc;

	// Brightness
	GLuint brightnessLoc;

	// Alpha
	GLuint alphaLocColor, alphaLocTexture, alphaLocParticleColor, alphaLocParticleTexture;

	// Textures
	GLuint textureLayerLocTexture, textureLayerLocTextureStatic;

	// Object Static Textures
	GLuint objectStaticColorTexture, objectStaticMaterialTexture, objectStaticParallaxTexture;

	// Object Static Values
	GLuint objectStaticViewPos, objectStaticLayer;
	
	// General Matrix for Objects
	GLuint objectMatrixLoc, objectStaticMatrixLoc;

	// Ambient
	GLuint ambientLocColor, ambientLocTexture;

	// Specular
	GLuint specularLocColor, specularLocTexture;

	// Shininess
	GLuint shininessLocColor, shininessLocTexture;

	// Fonts
	GLuint projectionLocFont, textureLocFont, texcolorLocFont;

	// Relative Fonts
	GLuint projectionLocRelativeFont, modelLocRelativeFont, textureLocRelativeFont, texcolorLocRelativeFont;

	// Global Fonts
	GLuint projectionLocGlobalFont, viewLocGlobalFont, textureLocGlobalFont, textcolorLocGlobalFont;

	// Bloom
	GLuint bloomLoc;

	// Particle Generator
	GLuint ParticleOffsetLocColor, ParticleOffsetLocTexture;

	// Level Border Objects
	GLuint level_border_camera_pos_loc;
	GLuint level_border_screen_width_loc;
	GLuint level_border_screen_height_loc;
	GLuint level_border_level_width_loc;
	GLuint level_border_level_height_loc;
	GLuint level_border_render_distance_loc;

	// Shaders List
	Shader::Shader FramebufferShader_Core;
	Shader::Shader FramebufferShader_HDR;
	Shader::Shader FramebufferShader_Bloom;
	Shader::Shader StencilbufferShader;
	Shader::Shader NormalShader;
	Shader::Shader objectShader;
	Shader::Shader objectShaderStatic;
	Shader::Shader colorShader;
	Shader::Shader colorShaderStatic;
	Shader::Shader colorShaderStaticLine;
	Shader::Shader texShader;
	Shader::Shader texShaderStatic;
	Shader::Shader fontShader;
	Shader::Shader fontOffsetShader;
	Shader::Shader fontGlobalShader;
	Shader::Shader particleShaderColor;
	Shader::Shader particleShaderTexture;
	Shader::Shader borderShader;

	// Friction Coefficients
	const int MATERIALS_COUNT = 25;
	const int MATERIALS_PERMUTATIONS = 351; // (MATERIALS_COUNT * (MATERIALS_COUNT + 1)) / 2
	float static_friction[MATERIALS_PERMUTATIONS]; // Index: (MATERIALS_COUNT * MATERIAL_INDEX1) + (MATERIALS_INDEX2 - MATERIALS_INDEX1) 
	float kinetic_friction[MATERIALS_PERMUTATIONS]; // MATERIAL_INDEX1 will always be the minimum between the two indicies

	// Maximum Number of Particles to be Used Each Frame
	int Global_Max_Particles = 20000;
	int Global_Particles_Loaded = 0;
	glm::vec4 Particle_Positions[20000];

	// Framebuffer
	GLuint Frame_Buffer_Object_Core, Frame_Buffer_Object_HDR, Frame_Buffer_Object_Multisample, Frame_Buffer_Object_Bloom[2];
	GLuint Frame_Buffer_Texture_Core, Frame_Buffer_Texture_HDR, Frame_Buffer_Texture_Bloom, Frame_Buffer_Texture_Multisample, Frame_Buffer_Texture_PingPong[2];
	GLuint Render_Buffer_Multisample, screenVAO, screenVBO, windowTexture;

	// Uniform Buffers
	GLuint MatricesBlock, DirectionalBuffer, PointBuffer, SpotBuffer, BeamBuffer, InstanceBuffer, PhysicsInstanceBuffer, ParticleBuffer;

	// Font Buffer Objects
	GLuint fontVAO, fontVBO;

	// Fonts
	std::map<std::string, int> FontList;
	std::map<GLchar, Struct::Character> Current_Font;

	// Audio Device
	ALCdevice* audio_device = nullptr;

	// Audio Context
	ALCcontext* audio_context = nullptr;

	// Time Variables
	float deltaTime = 0.0f; // Time Between Current and Last Frames
	double lastFrame = 0.0; // Current Time of Most Recent Frame
	double framebyframeStart = 0.0f; // At What Time The Last Frame By Frame Pause
	double framebyframeOffset = 0.0f; // Offset Caused by Frame By Frame Pausing
	const float SPEED = 15.0f; // Universal Speed Constant
	const float TERMINALVELOCITY = 50.0f; // Terminal Velocity
	double lastTime = 0.0; // Time Since Last Second
	int frameCount = 0; // Number of Frames
	int last_frameCount = 0; // Last Frame Count in a Second

	// Frame by Frame Debugging
	bool frame_step = false;
	bool frame_run = false;
	bool frame_by_frame = false;

	// Camera Speed Multiplier
	float camera_speed_multiplier = 1.0f;

	// Shift Speed Multiplier
	float shift_speed_multiplier = 1.0f;

	// Flag to Determine if Level Should Reload All
	bool reload_all = false;

	// Determines if the Engine is Paused or Not
	bool paused = false;

	// The Elusive Breakpoint
	bool activate_elusive_breakpoint = false;
}
