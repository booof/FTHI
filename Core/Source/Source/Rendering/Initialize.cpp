#include "Initialize.h"
#include "ExternalLibs.h"
#include "Globals.h"
#include "Class/Render/Shader/Shader.h"
#include "Source/Events/EventListeners.h"
#include "Source/Loaders/Fonts.h"
#include "Class/Render/Editor/EditorOptions.h"
#include "Source/Loaders/Textures.h"
#include "Source/Rendering/PreRender.h"
#include "Class/Render/Objects/ChangeController.h"
#include "Class/Render/Editor/Notification.h"
#include "Render/GUI/SelectedText.h"
#include "Class/Render/Editor/ScriptWizard.h"
#include "Source/Loaders/Audio.h"
#include "Class/Render/Editor/ProjectSelector.h"
#include "Class/Render/Editor/Debugger.h"
#include "Render/Editor/SceneController.h"

void why_do_i_have_to_mak_a_function_to_call_this()
{
	// Generate Change Controller
	change_controller->initializeChangeController();
}

void ErrorExit(LPTSTR lpszFunction);

bool Source::Rendering::Initialize::initialize()
{
	//////////////////////////////////
	// Splash Screen Initialization //
	//////////////////////////////////

	std::string splash_path = "../x64/Debug/DisplaySplash.dll";
	HINSTANCE splash_dll = LoadLibrary(splash_path.c_str());
	GetProcAddress(splash_dll, "openSplash")();

	//////////////////////////
	// Initial Engine Setup //
	//////////////////////////

	// Get the Current Path of the Engine
	Global::engine_path = std::filesystem::current_path().string();
	Global::engine_path.resize(Global::engine_path.size() - 5);
	for (int i = 0; i < Global::engine_path.size(); i++)
		if (Global::engine_path[i] == '/')
			Global::engine_path[i] = '\\';
	std::cout << "Loading Engine From: " << Global::engine_path << "\n";

	// Get the Default Project Directory
	Global::default_project_directory_path = Global::engine_path;
	Global::default_project_directory_path.resize(Global::default_project_directory_path.size() - 5);
	Global::default_project_directory_path += "\\Projects";

	// Test if Project Directory Exists
	if (std::filesystem::exists(Global::default_project_directory_path))
		std::cout << "Default Project Path: " << Global::default_project_directory_path << "\n";
	else
	{
		std::cout << "Default Project Path Does Not Exist\nCreating New Directory At:" << Global::default_project_directory_path << "\n";
		std::filesystem::create_directory(Global::default_project_directory_path);
	}

	// Set Random Number Generator
	Global::rng = (uint64_t)time;

	///////////////////////
	// Initialize OpenGL //
	///////////////////////

	// Initialize GLFW
	glfwInit();

	// Initialize Window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

	// Create Window
	Global::window = glfwCreateWindow(Global::screenWidth, Global::screenHeight, "For The Hell Of It", NULL, NULL);

	if (Global::window == NULL)
	{
		std::cout << "Failed to Initialize Window" << std::endl;
		glfwTerminate();
		exit(-1);
	}	

	// Bind Graphics Context
	glfwMakeContextCurrent(Global::window);
	glfwSetFramebufferSizeCallback(Global::window, Source::Listeners::framebuffer_size_callback);
	glfwSwapInterval(1);

	// Bind Input Events to Window
	glfwSetKeyCallback(Global::window, Source::Listeners::KeyCallback);
	glfwSetCursorPosCallback(Global::window, Source::Listeners::CursorCallback);
	glfwSetMouseButtonCallback(Global::window, Source::Listeners::MouseCallback);
	glfwSetScrollCallback(Global::window, Source::Listeners::ScrollCallback);

	// Move Window to Center of Screen
	MONITORINFO monitor_info = { 0 };
	monitor_info.cbSize = sizeof(monitor_info);
	GetMonitorInfo(MonitorFromPoint(POINT{0}, MONITOR_DEFAULTTOPRIMARY), &monitor_info);
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(Global::window,
		monitor_info.rcWork.left + (monitor_info.rcWork.right - monitor_info.rcWork.left - Global::screenWidth) / 2,
		monitor_info.rcWork.top + (monitor_info.rcWork.bottom - monitor_info.rcWork.top - Global::screenHeight) / 2);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		exit(-1);
	}

	printf("Using OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);

	// Enable Several Graphical Options
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D_ARRAY);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilOp(GL_ZERO, GL_REPLACE, GL_REPLACE);

	// Set View Port
	glViewport(0, 0, Global::screenWidth, Global::screenHeight);

#define KHRONOS
#ifdef KHRONOS

	PFNWGLSWAPINTERVALEXTPROC       wglSwapIntervalEXT = NULL;
	PFNWGLGETSWAPINTERVALEXTPROC    wglGetSwapIntervalEXT = NULL;

	if (WGLExtensionSupported("WGL_EXT_swap_control"))
	{
		// Extension is supported, init pointers.
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

		// this is another function from WGL_EXT_swap_control extension
		wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
	}

	// Enable Vsync
	//glSwapIntervalEXT(1);

#endif

	// Enable Debug Message Callback
	glDebugMessageCallback(Source::Listeners::MessageCallback, 0);

	///////////////////////
	// Initialize OpenAL //
	///////////////////////

	// Store Pointer to Initial Audio Device
	Source::Audio::getCurrentAudioDevice();

	// Generate Primary Audio Context
	Global::audio_context = alcCreateContext(Global::audio_device, nullptr);

	// Store Current Audio Context
	alcMakeContextCurrent(Global::audio_context);

	///////////////////////////
	// Generate Framebuffers //
	///////////////////////////


	// Generate Framebuffer
	GLuint test;
	glGenVertexArrays(1, &test);

	glGenFramebuffers(1, &Global::Frame_Buffer_Object_Core);
	glGenFramebuffers(1, &Global::Frame_Buffer_Object_HDR);
	glGenFramebuffers(1, &Global::Frame_Buffer_Object_Multisample);
	glGenFramebuffers(2, Global::Frame_Buffer_Object_Bloom);
	glGenTextures(1, &Global::Frame_Buffer_Texture_Core);
	glGenTextures(1, &Global::Frame_Buffer_Texture_HDR);
	glGenTextures(1, &Global::Frame_Buffer_Texture_Multisample);
	glGenTextures(2, Global::Frame_Buffer_Texture_PingPong);
	glGenRenderbuffers(1, &Global::Render_Buffer_Multisample);

	// Bind Core Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Global::Frame_Buffer_Object_Core);
	glBindTexture(GL_TEXTURE_2D, Global::Frame_Buffer_Texture_Core);

	// Create Framebuffer Core Texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Global::screenWidth, Global::screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind Core Framebuffer Texture to Core Framebuffer Object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Global::Frame_Buffer_Texture_Core, 0);

	// Check if Core Framebuffer is Complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER: Core Framebuffer is not Complete" << std::endl;
	}

	// Bind HDR Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Global::Frame_Buffer_Object_HDR);

	// Create HDR Framebuffer Texture
	glBindTexture(GL_TEXTURE_2D, Global::Frame_Buffer_Texture_HDR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Global::screenWidth, Global::screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind HDR Framebuffer Texture to HDR Framebuffer Object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Global::Frame_Buffer_Texture_HDR, 0);

	// Create Bloom Framebuffer Texture
	glBindTexture(GL_TEXTURE_2D, Global::Frame_Buffer_Texture_Bloom);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Global::screenWidth, Global::screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Bind Bloom Framebuffer Texture to HDR Framebuffer Object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, Global::Frame_Buffer_Texture_Bloom, 0);

	// Test if HDR Framebuffer is Complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER: Framebuffer is not Complete" << std::endl;
	}

	// Bind Multisample Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, Global::Frame_Buffer_Object_Multisample);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, Global::Frame_Buffer_Texture_Multisample);
	glBindRenderbuffer(GL_RENDERBUFFER, Global::Render_Buffer_Multisample);

	// Create Multisampled Framebuffer Texture
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGBA16F, Global::screenWidth, Global::screenHeight, GL_TRUE);

	// Bind Multisampled Framebuffer Texture to Multisampled Framebuffer Object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, Global::Frame_Buffer_Texture_Multisample, 0);

	// Create Renderbuffer Object
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH24_STENCIL8, Global::screenWidth, Global::screenHeight);

	// Bind Renderbuffer Object to HDR Framebuffer Object
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, Global::Render_Buffer_Multisample);

	// Test if Multisampled Framebuffer is Complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER: Multisampled Framebuffer is not Complete" << std::endl;
	}

	for (int i = 0; i < 2; i++)
	{
		// Bind Bloom Framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, Global::Frame_Buffer_Object_Bloom[i]);

		// Create PingPong Texture
		glBindTexture(GL_TEXTURE_2D, Global::Frame_Buffer_Texture_PingPong[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Global::screenWidth, Global::screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Bind PingPong Texture to Bloom Framebuffer Object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Global::Frame_Buffer_Texture_PingPong[i], 0);

		// Test if Bloom Framebuffer is Complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ERROR::FRAMEBUFFER: Bloom Framebuffer is not Complete" << std::endl;
		}
	}


	// Unbind Framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Generate Screen Object
	glGenVertexArrays(1, &Global::screenVAO);
	glGenBuffers(1, &Global::screenVBO);

	// Bind Screen Object
	glBindVertexArray(Global::screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, Global::screenVBO);

	// Create and Bind Normalized Device Coordinates
	float vertices[24] =
	{
		-1.0, -1.0, 0.0, 0.0,
		1.0, -1.0, 1.0, 0.0,
		1.0, 1.0, 1.0, 1.0,
		1.0, 1.0, 1.0, 1.0,
		-1.0, 1.0, 0.0, 1.0,
		-1.0, -1.0, 0.0, 0.0
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Enable Position Vertices
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)(0));
	glEnableVertexAttribArray(0);

	// Enable Texture Vertices
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (void*)(2 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	// Unbind Screen Object
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	////////////////
	// Load Fonts //
	////////////////


	// Create Free Type Library
	if (FT_Init_FreeType(&Global::FREETYPE_LIBRARY))
	{
		std::cout << "Failed to Initialize FREETYPE" << std::endl;
		glfwTerminate();
		exit(-1);
	}

	// Load Arial Glyph Textures
	Global::FontList.insert({ "Arial", 0 });
	if (FT_New_Face(Global::FREETYPE_LIBRARY, "../Resources/Fonts/arial.ttf", 0, &Global::Arial))
	{
		std::cout << "ERROR::FREETYPE::FAILED_TO_LOAD_FONT: ARIAL" << std::endl;
	}

	// Load Cambria Math Glyph Textures
	Global::FontList.insert({ "CambriaMath", 1 });
	if (FT_New_Face(Global::FREETYPE_LIBRARY, "../Resources/Fonts/cambria.ttc", 0, &Global::CambriaMath))
	{
		std::cout << "ERROR::FREETYPE::FAILED_TO_LOAD_FONT: CAMBRIA MATH" << std::endl;
	}

	// Load SEGOEUI Glyph Textures
	Global::FontList.insert({ "SEGOEUI", 2 });
	if (FT_New_Face(Global::FREETYPE_LIBRARY, "../Resources/Fonts/SEGOEUI.ttf", 0, &Global::SEGOEUI))
	{
		std::cout << "ERROR::FREETYPE::FAILED_TO_LOAD_FONT: SEGOEUI" << std::endl;
	}

	// Load test Glyph Textures
	Global::FontList.insert({ "test", 3 });
	if (FT_New_Face(Global::FREETYPE_LIBRARY, "../Resources/Fonts/test.ttf", 0, &Global::TEST))
	{
		std::cout << "ERROR::FREETYPE::FAILED_TO_LOAD_FONT: test" << std::endl;
	}

	// Disable Byte Allignment Restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Initialize Default Font
	Source::Fonts::changeFont("CambriaMath", 0, 48);

	// Generate Font Buffer Objects
	glGenVertexArrays(1, &Global::fontVAO);
	glGenBuffers(1, &Global::fontVBO);

	// Enable font VAO
	glBindVertexArray(Global::fontVAO);

	// Enable font VBO and Leave it Unbinded
	glBindBuffer(GL_ARRAY_BUFFER, Global::fontVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	// Bind Data with Shaders
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);

	// Unbind font VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind font VAO
	glBindVertexArray(0);


	/////////////////
	// Load Images //
	/////////////////

	// Load Mouse Images
	const char* image_Mouse[12] =
	{
		"../Resources/Textures/Mouse/Cursor.png",
		"../Resources/Textures/Mouse/Hand.png",
		"../Resources/Textures/Mouse/Type.png",
		"../Resources/Textures/Mouse/ReSize.png",
		"../Resources/Textures/Mouse/ReSizeHorizontal.png",
		"../Resources/Textures/Mouse/ReSizeVertical.png",
		"../Resources/Textures/Mouse/ReSizePositive.png",
		"../Resources/Textures/Mouse/ReSizeNegative.png",
		"../Resources/Textures/Mouse/Wait.png",
		"../Resources/Textures/Mouse/Help.png",
		"../Resources/Textures/Mouse/Lock.png",
		"../Resources/Textures/Mouse/Forbiden.png"
	};

	// Width and Height of Images
	int imageWidth, imageHeight;

	// Image of Cursors
	GLFWimage image;

	// Set Image Parameters and Assign to Cursor Variable
	image.pixels = SOIL_load_image(image_Mouse[0], &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);
	image.width = imageWidth;
	image.height = imageHeight;
	Global::Mouse_Textures.insert({ "Cursor", glfwCreateCursor(&image, 0, 0) });

	// Set Image Parameters and Assign to Hand Variable
	image.pixels = SOIL_load_image(image_Mouse[1], &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);
	image.width = imageWidth;
	image.height = imageHeight;
	Global::Mouse_Textures.insert({ "Hand", glfwCreateCursor(&image, 5, 0) });

	// Set Image Parameters and Assign to Type Variable
	image.pixels = SOIL_load_image(image_Mouse[2], &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);
	image.width = imageWidth;
	image.height = imageHeight;
	Global::Mouse_Textures.insert({ "Type", glfwCreateCursor(&image, 0, 11) });

	// Set Image Parameters and Assign to BiDirectional Resize Variable
	image.pixels = SOIL_load_image(image_Mouse[3], &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);
	image.width = imageWidth;
	image.height = imageHeight;
	Global::Mouse_Textures.insert({ "ReSize", glfwCreateCursor(&image, 0, 11) });

	// Set Image Parameters and Assign to Horizontal Resize Variable
	image.pixels = SOIL_load_image(image_Mouse[4], &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);
	image.width = imageWidth;
	image.height = imageHeight;
	Global::Mouse_Textures.insert({ "ReSizeHorizontal", glfwCreateCursor(&image, 12, 0) });

	// Set Image Parameters and Assign to Horizontal Resize Variable
	image.pixels = SOIL_load_image(image_Mouse[5], &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);
	image.width = imageWidth;
	image.height = imageHeight;
	Global::Mouse_Textures.insert({ "ReSizeVertical", glfwCreateCursor(&image, 0, 12) });

	// Set Image Parameters and Assign to Positive Resize Variable
	image.pixels = SOIL_load_image(image_Mouse[6], &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);
	image.width = imageWidth;
	image.height = imageHeight;
	Global::Mouse_Textures.insert({ "ReSizePositive", glfwCreateCursor(&image, 12, 0) });

	// Set Image Parameters and Assign to Negative Resize Variable
	image.pixels = SOIL_load_image(image_Mouse[7], &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);
	image.width = imageWidth;
	image.height = imageHeight;
	Global::Mouse_Textures.insert({ "ReSizeNegative", glfwCreateCursor(&image, 0, 12) });

	// Set Image Parameters and Assign to Wait Variable
	image.pixels = SOIL_load_image(image_Mouse[8], &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);
	image.width = imageWidth;
	image.height = imageHeight;
	Global::Mouse_Textures.insert({ "Wait", glfwCreateCursor(&image, 0, 11) });

	// Set Image Parameters and Assign to Help Variable
	image.pixels = SOIL_load_image(image_Mouse[9], &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);
	image.width = imageWidth;
	image.height = imageHeight;
	Global::Mouse_Textures.insert({ "Help", glfwCreateCursor(&image, 0, 11) });

	// Set Image Parameters and Assign to Lock Variable
	image.pixels = SOIL_load_image(image_Mouse[10], &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);
	image.width = imageWidth;
	image.height = imageHeight;
	Global::Mouse_Textures.insert({ "Lock", glfwCreateCursor(&image, 0, 11) });

	// Set Image Parameters and Assign to Forbiden Variable
	image.pixels = SOIL_load_image(image_Mouse[11], &imageWidth, &imageHeight, 0, SOIL_LOAD_RGBA);
	image.width = imageWidth;
	image.height = imageHeight;
	Global::Mouse_Textures.insert({ "Forbiden", glfwCreateCursor(&image, 0, 11) });

	// Set Cursor to Default
	glfwSetCursor(Global::window, Global::Mouse_Textures.find("Cursor")->second);

	// Return a Nullified Value
	//texture_null.texture = NULL;
	//texture_null.data[0] = NULL;
	//texture_null.index = NULL;
	//texture_null.size = NULL;
	//Foreground_Textures.insert({ "NULL", texture_null });
	//Formerground_Textures.insert({ "NULL", texture_null });
	//Background_Textures.insert({ "NULL", texture_null });
	//Backdrop_Textures.insert({ "NULL", texture_null });
	//Particle_Textures.insert({ "NULL", texture_null });
	//Object_Textures.insert({ "NULL", texture_null });

	// Load Textures
	//Load_Textures("../Resources/Textures/Objects/Foreground", Foreground_Textures, GL_SRGB_ALPHA);
	//Load_Textures("../Resources/Textures/Objects/Formerground", Formerground_Textures, GL_SRGB_ALPHA);
	//Load_Textures("../Resources/Textures/Objects/Background", Background_Textures, GL_SRGB_ALPHA);
	//Load_Textures("../Resources/Textures/Objects/Backdrop", Backdrop_Textures, GL_SRGB_ALPHA);
	//Load_Textures("../Resources/Textures/HUD", HUD_Textures, GL_RGBA);
	//Load_Textures("../Resources/Textures/Objects/Weapons", Weapon_Textures, GL_RGBA);
	Source::Textures::Load_Textures("../Resources/Textures/Objects/Visual_Indicators", Global::Visual_Textures, GL_RGBA);
	//Load_Textures("../Resources/Textures/Entity/Player", Player_Textures, GL_RGBA);
	//Load_Textures("../Resources/Textures/Objects/Physics", Object_Textures, GL_RGBA);
	//Load_Textures("../Resources/Textures/Objects/Particles", Particle_Textures, GL_RGBA);

	// Load Materials
	//Load_Materials_Lighting("../Resources/Materials/Lighting.txt", materials);
	//Load_Materials_Friction("../Resources/Materials/Friction.txt");


	///////////////////////////////////
	// Initialize some Basic Objects //
	///////////////////////////////////


	// The Shader Classes
	Global::FramebufferShader_Core = Shader::Shader("Source/Class/Render/Shader/vert/Framebuffer.vert", "Source/Class/Render/Shader/geom/TextureShaderStatic.geom", "Source/Class/Render/Shader/frag/Framebuffer_Core.frag");
	Global::FramebufferShader_HDR = Shader::Shader("Source/Class/Render/Shader/vert/Framebuffer.vert", "Source/Class/Render/Shader/geom/TextureShaderStatic.geom", "Source/Class/Render/Shader/frag/Framebuffer_HDR.frag");
	Global::FramebufferShader_Bloom = Shader::Shader("Source/Class/Render/Shader/vert/Framebuffer.vert", "Source/Class/Render/Shader/geom/TextureShaderStatic.geom", "Source/Class/Render/Shader/frag/Framebuffer_Bloom.frag");
	Global::StencilbufferShader = Shader::Shader("Source/Class/Render/Shader/vert/SingleColor.vert", "Source/Class/Render/Shader/geom/SingleColor.geom", "Source/Class/Render/Shader/frag/SingleColor.frag");
	Global::NormalShader = Shader::Shader("Source/Class/Render/Shader/vert/Normals.vert", "Source/Class/Render/Shader/geom/Normals.geom", "Source/Class/Render/Shader/frag/SingleColor.frag");


	Global::objectShader = Shader::Shader("Source/Class/Render/Shader/vert/ObjectShader.vert", "Source/Class/Render/Shader/frag/ObjectShader.frag");
	//Global::objectShader = Shader::Shader("Source/Class/Render/Shader/vert/ObjectShader.vert", "Source/Class/Render/Shader/frag/test.frag");
	
	
	Global::objectShaderStatic = Shader::Shader("Source/Class/Render/Shader/vert/ObjectShaderStatic.vert", "Source/Class/Render/Shader/frag/ObjectShaderStatic.frag");
	Global::colorShader = Shader::Shader("Source/Class/Render/Shader/vert/ColorShader.vert", "Source/Class/Render/Shader/geom/ColorShader.geom", "Source/Class/Render/Shader/frag/ColorShader.frag");
	Global::colorShaderStatic = Shader::Shader("Source/Class/Render/Shader/vert/ColorShaderStatic.vert", "Source/Class/Render/Shader/frag/ColorShaderStatic.frag");
	//Global::colorShaderStaticLine = Shader::Shader("Source/Class/Render/Shader/vert/ColorShaderStatic.vert", "Source/Class/Render/Shader/geom/ColorShaderStaticLine.geom", "Source/Class/Render/Shader/frag/ColorShaderStatic.frag");
	Global::texShader = Shader::Shader("Source/Class/Render/Shader/vert/TextureShader.vert", "Source/Class/Render/Shader/geom/TextureShader.geom", "Source/Class/Render/Shader/frag/TextureShader.frag");
	Global::texShaderStatic = Shader::Shader("Source/Class/Render/Shader/vert/TextureShaderStatic.vert", "Source/Class/Render/Shader/frag/TextureShaderStatic.frag");
	Global::fontShader = Shader::Shader("Source/Class/Render/Shader/vert/FontShader.vert", "Source/Class/Render/Shader/geom/TextureShaderStatic.geom", "Source/Class/Render/Shader/frag/FontShader.frag");
	Global::fontOffsetShader = Shader::Shader("Source/Class/Render/Shader/vert/OffsetFontShader.vert", "Source/Class/Render/Shader/geom/TextureShaderStatic.geom", "Source/Class/Render/Shader/frag/FontShader.frag");
	Global::fontGlobalShader = Shader::Shader("Source/Class/Render/Shader/vert/GlobalFontShader.vert", "Source/Class/Render/Shader/geom/TextureShaderStatic.geom", "Source/Class/Render/Shader/frag/FontShader.frag");
	Global::particleShaderColor = Shader::Shader("Source/Class/Render/Shader/vert/ParticleShaderColor.vert", "Source/Class/Render/Shader/geom/ParticleShaderColor.geom", "Source/Class/Render/Shader/frag/ParticleShaderColor.frag");
	Global::particleShaderTexture = Shader::Shader("Source/Class/Render/Shader/vert/ParticleShaderTexture.vert", "Source/Class/Render/Shader/geom/ParticleShaderTexture.geom", "Source/Class/Render/Shader/frag/ParticleShaderTexture.frag");
	Global::borderShader = Shader::Shader("Source/Class/Render/Shader/vert/LevelBorderShader.vert", "Source/Class/Render/Shader/geom/LevelBorderShader.geom", "Source/Class/Render/Shader/frag/LevelBorderShader.frag");

	// Camera Used to Calculate View Matrix
	//camera = Camera(0.0f, 0.0f, false);
	//view = glm::translate(glm::mat4(1.0f), camera.Position);

	// Set up Projection Matrix
	Global::projection = glm::ortho(-Global::halfScalarX * Global::zoom_scale, Global::halfScalarX * Global::zoom_scale, -50.0f * Global::zoom_scale, 50.0f * Global::zoom_scale, 0.1f, 100.0f);
	Global::projectionStatic = glm::ortho(-Global::halfScalarX, Global::halfScalarX, -50.0f, 50.0f, 0.1f, 100.0f);

	// Matrices Block Locations
	glUniformBlockBinding(Global::colorShader.Program, glGetUniformBlockIndex(Global::colorShader.Program, "Matrices"), 0);
	glUniformBlockBinding(Global::colorShaderStatic.Program, glGetUniformBlockIndex(Global::colorShaderStatic.Program, "Matrices"), 0);
	glUniformBlockBinding(Global::texShader.Program, glGetUniformBlockIndex(Global::texShader.Program, "Matrices"), 0);
	glUniformBlockBinding(Global::texShaderStatic.Program, glGetUniformBlockIndex(Global::texShaderStatic.Program, "Matrices"), 0);
	glUniformBlockBinding(Global::particleShaderColor.Program, glGetUniformBlockIndex(Global::particleShaderColor.Program, "Matrices"), 0);
	glUniformBlockBinding(Global::particleShaderTexture.Program, glGetUniformBlockIndex(Global::particleShaderTexture.Program, "Matrices"), 0);
	glUniformBlockBinding(Global::objectShaderStatic.Program, glGetUniformBlockIndex(Global::objectShader.Program, "Matrices"), 0);
	glUniformBlockBinding(Global::objectShaderStatic.Program, glGetUniformBlockIndex(Global::objectShaderStatic.Program, "Matrices"), 0);

	// Generate Matrices Block
	glGenBuffers(1, &Global::MatricesBlock);
	glBindBuffer(GL_UNIFORM_BUFFER, Global::MatricesBlock);
	glBufferData(GL_UNIFORM_BUFFER, 192, NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(glm::mat4(1.0f)));
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(Global::projection));
	glBufferSubData(GL_UNIFORM_BUFFER, 128, 64, glm::value_ptr(Global::projectionStatic));
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, Global::MatricesBlock, 0, 192);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Matrices Block Default Data
	glBindBuffer(GL_UNIFORM_BUFFER, Global::MatricesBlock);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(Global::view));
	glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(Global::projection));
	glBufferSubData(GL_UNIFORM_BUFFER, 128, 64, glm::value_ptr(Global::projectionStatic));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Generate Object Instance Buffers
	glGenBuffers(1, &Global::InstanceBuffer);
	glGenBuffers(1, &Global::PhysicsInstanceBuffer);

	// Generate Light Buffers
	glGenBuffers(1, &Global::DirectionalBuffer);
	glGenBuffers(1, &Global::PointBuffer);
	glGenBuffers(1, &Global::SpotBuffer);
	glGenBuffers(1, &Global::BeamBuffer);

	// Generate Particles Buffer and Nullify Data
	glGenBuffers(1, &Global::ParticleBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, Global::ParticleBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Global::Particle_Positions), NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Object Shader Locations
	Global::objectShader.Use();
	Global::matrixLocObject = glGetUniformLocation(Global::objectShader.Program, "matrix");

	// Static Object Shader Locations
	Global::objectShaderStatic.Use();
	Global::modelLocObjectStatic = glGetUniformLocation(Global::objectShaderStatic.Program, "model");
	Global::objectStaticMatrixLoc = glGetUniformLocation(Global::objectShaderStatic.Program, "matrix");
	Global::objectStaticColorTexture = glGetUniformLocation(Global::objectShaderStatic.Program, "color_texture");
	Global::objectStaticMaterialTexture = glGetUniformLocation(Global::objectShaderStatic.Program, "material_texture");
	Global::objectStaticParallaxTexture = glGetUniformLocation(Global::objectShaderStatic.Program, "parallax_texture");
	Global::objectStaticViewPos = glGetUniformLocation(Global::objectShaderStatic.Program, "vew_pos");
	Global::objectStaticLayer = glGetUniformLocation(Global::objectShaderStatic.Program, "layer");

	// Color Shader Locations
	Global::colorShader.Use();
	Global::modelLocColor = glGetUniformLocation(Global::colorShader.Program, "model");
	Global::alphaLocColor = glGetUniformLocation(Global::colorShader.Program, "Alpha");
	Global::ambientLocColor = glGetUniformLocation(Global::colorShader.Program, "material.ambient");
	Global::specularLocColor = glGetUniformLocation(Global::colorShader.Program, "material.specular");
	Global::shininessLocColor = glGetUniformLocation(Global::colorShader.Program, "material.shininess");
	glUniform1f(Global::alphaLocColor, 1.0f);

	// Static Color Locations
	Global::colorShaderStatic.Use();
	Global::modelLocColorStatic = glGetUniformLocation(Global::colorShaderStatic.Program, "model");
	Global::brightnessLoc = glGetUniformLocation(Global::colorShaderStatic.Program, "brightness");
	glUniform1i(glGetUniformLocation(Global::colorShaderStatic.Program, "Static"), 0);

	// Texture Shader Locations
	Global::texShader.Use();
	Global::modelLocTexture = glGetUniformLocation(Global::texShader.Program, "model");
	Global::alphaLocTexture = glGetUniformLocation(Global::texShader.Program, "Alpha");
	Global::ambientLocTexture = glGetUniformLocation(Global::texShader.Program, "material.ambient");
	Global::specularLocTexture = glGetUniformLocation(Global::texShader.Program, "material.specular");
	Global::shininessLocTexture = glGetUniformLocation(Global::texShader.Program, "material.shininess");
	Global::textureLayerLocTexture = glGetUniformLocation(Global::texShader.Program, "material.texture_layer");
	glUniform1f(Global::alphaLocTexture, 1.0f);
	glUniform1i(glGetUniformLocation(Global::texShader.Program, "material.texture1"), 0);
	glUniform1i(glGetUniformLocation(Global::texShader.Program, "material.texture2"), 1);
	glUniform1i(glGetUniformLocation(Global::texShader.Program, "material.texture3"), 2);

	// Static Texture Shader Locations
	Global::texShaderStatic.Use();
	Global::modelLocTextureStatic = glGetUniformLocation(Global::texShaderStatic.Program, "model");
	Global::directionLoc = glGetUniformLocation(Global::texShaderStatic.Program, "direction");
	Global::textureLayerLocTextureStatic = glGetUniformLocation(Global::texShaderStatic.Program, "texture_layer");
	Global::staticLocTexture = glGetUniformLocation(Global::texShaderStatic.Program, "Static");
	glUniform1i(glGetUniformLocation(Global::texShaderStatic.Program, "Static"), 0);
	glUniform1i(glGetUniformLocation(Global::texShaderStatic.Program, "texture1"), 3);
	glUniform1i(glGetUniformLocation(Global::texShaderStatic.Program, "texture2"), 4);

	// Font Shader Locations
	Global::fontShader.Use();
	Global::projectionLocFont = glGetUniformLocation(Global::fontShader.Program, "project");
	Global::textureLocFont = glGetUniformLocation(Global::fontShader.Program, "texture1");
	glUniformMatrix4fv(Global::projectionLocFont, 1, GL_FALSE, glm::value_ptr(Global::projectionStatic));
	Global::texcolorLocFont = glGetUniformLocation(Global::fontShader.Program, "texColor");

	// Offset Font Shader Locations
	Global::fontOffsetShader.Use();
	Global::projectionLocRelativeFont = glGetUniformLocation(Global::fontOffsetShader.Program, "project");
	glUniformMatrix4fv(Global::projectionLocGlobalFont, 1, GL_FALSE, glm::value_ptr(Global::projection));
	Global::modelLocRelativeFont = glGetUniformLocation(Global::fontOffsetShader.Program, "model");
	Global::textureLocRelativeFont = glGetUniformLocation(Global::fontOffsetShader.Program, "texture1");
	Global::texcolorLocRelativeFont = glGetUniformLocation(Global::fontOffsetShader.Program, "texColor");

	// Global Font Shader Locations
	Global::fontGlobalShader.Use();
	Global::projectionLocGlobalFont = glGetUniformLocation(Global::fontGlobalShader.Program, "project");
	glUniformMatrix4fv(Global::projectionLocGlobalFont, 1, GL_FALSE, glm::value_ptr(Global::projection));
	Global::viewLocGlobalFont = glGetUniformLocation(Global::fontGlobalShader.Program, "view");
	Global::textureLocGlobalFont = glGetUniformLocation(Global::fontGlobalShader.Program, "texture1");
	Global::textcolorLocGlobalFont = glGetUniformLocation(Global::fontGlobalShader.Program, "texColor");

	// Particle Shader Color Locations
	Global::particleShaderColor.Use();
	Global::modelLocParticleColor = glGetUniformLocation(Global::particleShaderColor.Program, "model");
	Global::staticLocParticleColor = glGetUniformLocation(Global::particleShaderColor.Program, "Static");
	Global::alphaLocParticleColor = glGetUniformLocation(Global::particleShaderColor.Program, "alpha");
	glUniform1i(Global::staticLocParticleColor, true);
	Global::ParticleOffsetLocColor = glGetUniformLocation(Global::particleShaderColor.Program, "Particles_Offset");

	// Particle Shader Texture Locations
	Global::particleShaderTexture.Use();
	Global::modelLocParticleTexture = glGetUniformLocation(Global::particleShaderTexture.Program, "model");
	Global::staticLocParticleTexture = glGetUniformLocation(Global::particleShaderTexture.Program, "Static");
	Global::alphaLocParticleTexture = glGetUniformLocation(Global::particleShaderTexture.Program, "alpha");
	glUniform1i(Global::staticLocParticleTexture, true);
	Global::ParticleOffsetLocTexture = glGetUniformLocation(Global::particleShaderTexture.Program, "Particles_Offset");
	glUniform1i(glGetUniformLocation(Global::particleShaderTexture.Program, "texture1"), 0);
	glUniform1i(glGetUniformLocation(Global::particleShaderTexture.Program, "texture2"), 1);

	// Single Color Shader Locations
	Global::StencilbufferShader.Use();
	Global::modelLocSingleColor = glGetUniformLocation(Global::StencilbufferShader.Program, "model");
	glUniform4f(glGetUniformLocation(Global::StencilbufferShader.Program, "color"), 1.0, 0.0, 0.0, 1.0);

	// Normal Shader Locations
	Global::NormalShader.Use();
	Global::modelLocNormal = glGetUniformLocation(Global::NormalShader.Program, "model");
	glUniform4f(glGetUniformLocation(Global::NormalShader.Program, "color"), 0.0, 0.0, 0.0, 1.0);

	// Bloom Shader Location
	Global::FramebufferShader_Bloom.Use();
	Global::bloomLoc = glGetUniformLocation(Global::FramebufferShader_Bloom.Program, "iteration");

	// Core Framebuffer Textures
	Global::FramebufferShader_Core.Use();
	glUniform1i(glGetUniformLocation(Global::FramebufferShader_Core.Program, "screenTexture"), 0);
	glUniform1i(glGetUniformLocation(Global::FramebufferShader_Core.Program, "bloomTexture"), 1);

	// Level Border Shader Locations
	Global::borderShader.Use();
	Global::level_border_camera_pos_loc = glGetUniformLocation(Global::borderShader.Program, "view_pos");
	Global::level_border_screen_width_loc = glGetUniformLocation(Global::borderShader.Program, "screen_width");
	Global::level_border_screen_height_loc = glGetUniformLocation(Global::borderShader.Program, "screen_height");
	Global::level_border_level_width_loc = glGetUniformLocation(Global::borderShader.Program, "level_width");
	Global::level_border_level_height_loc = glGetUniformLocation(Global::borderShader.Program, "level_height");
	Global::level_border_render_distance_loc = glGetUniformLocation(Global::borderShader.Program, "render_distance");
	glUniform2f(Global::level_border_camera_pos_loc, 0.0f, 0.0f);
	glUniform1f(Global::level_border_screen_width_loc, -Global::halfScalarX * Global::zoom_scale);
	glUniform1f(Global::level_border_screen_height_loc, -50.0f * Global::zoom_scale);
	glUniform1f(Global::level_border_screen_width_loc, 128.0f);
	glUniform1f(Global::level_border_screen_height_loc, 64.0f);
	glUniform1i(Global::level_border_render_distance_loc, 2);

	// Enable Depth and Scissor Tests
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);

	// Initialization Error Log
	//Source::Rendering::Pre::Error_Log();

#ifdef EDITOR

	// Editor Specific Initialization

	// Bridge the Connection to Dot Net
	Global::framework_handle = LoadLibrary("..\\x64\\Debug\\NetFrameworkWrangler.dll");
	GetProcAddress(Global::framework_handle, "echo")();

	// Generate Editor Options GUI
	Global::editor_options = new EditorOptions();
	Global::editor_options->initialize();

	// Generate Change Controller
	why_do_i_have_to_mak_a_function_to_call_this();

	// Generate Notification GUI
	notification_->initializeNotification();

	// Generate Selected Text Object
	selected_text->initializeSelectedText();

	// Generate ScriptWizard
	script_wizard->initializeScriptWizard();

	// Generate Debugger
	debugger->initializeDebugger();

	// Generate Scene Controller
	scene_controller->initializeSceneController();

#endif

	// Allow the Splash Screen to Fade
	FARPROC fade_function = GetProcAddress(splash_dll, "fadeSplash");
	while (fade_function())
		Sleep(4);

	// Make the Window Visible
	glfwShowWindow(Global::window);

	// Disable Splash Screen
	GetProcAddress(splash_dll, "closeSplash")();
	FreeLibrary(splash_dll);

	// Prompt User to Load a Project
	//project_selector->readProjectListFile();
	project_selector->initializeProjectSelector();

	// Read UUID to Start At
	if (!Global::project_file_path.empty())
	{
		std::ifstream uuid_file;
		uuid_file.open(Global::project_resources_path + "/Data/CommonData/UUID.dat", std::ios::binary);
		uuid_file.read((char*)&Global::uuid_counter, sizeof(uint32_t));
		uuid_file.close();
	}

	// Returns True if a Project was not Selected
	return Global::project_file_path.empty();
}

bool Source::Rendering::Initialize::WGLExtensionSupported(const char* extension_name)
{
	// this is pointer to function which returns pointer to string with list of all wgl extensions
	PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

	// determine pointer to wglGetExtensionsStringEXT function
	_wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");

	if (strstr(_wglGetExtensionsStringEXT(), extension_name) == NULL)
	{
		// string was not found
		return false;
	}

	// extension is supported
	return true;
}

Render::GUI::BoxDataBundle Source::Rendering::Initialize::constrtuctBox(uint8_t mode, float xPos, float yPos, float zPos, float width, float height, bool centered, std::string text, glm::vec4 background_color, glm::vec4 outline_color, glm::vec4 highlight_color, glm::vec4 text_color)
{
	Render::GUI::BoxDataBundle data;
	data.data1.position = glm::vec2(xPos, yPos);
	data.data1.is_static = false;
	data.data1.script = 0;
	data.data2.mode = mode;
	data.data2.zpos = zPos;
	data.data2.width = width;
	data.data2.height = height;
	data.data2.centered = centered;
	data.data2.button_text = text;
	data.data2.background_color = background_color;
	data.data2.outline_color = outline_color;
	data.data2.highlight_color = highlight_color;
	data.data2.text_color = text_color;
	return data;
}

Render::GUI::ToggleGroupData Source::Rendering::Initialize::constructGrouper(uint8_t group_count, uint8_t initial_value)
{
	Render::GUI::ToggleGroupData data;
	data.group_count = group_count;
	data.initial_value = initial_value;
	return data;
}

Render::GUI::TextDataBundle Source::Rendering::Initialize::constructText(float xPos, float yPos, float scale, glm::vec4 color, bool static_, std::string text)
{
	Render::GUI::TextDataBundle data;
	data.data1.position = glm::vec2(xPos, yPos);
	data.data1.is_static = static_;
	data.data1.script = 0;
	data.data2.scale = scale;
	data.data2.color = color;
	data.data2.text = text;
	return data;
}
