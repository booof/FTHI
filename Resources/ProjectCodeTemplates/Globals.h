#ifndef DLL_HEADER 
namespace glm {
	class vec2;
}
namespace Render::Camera{
	class Camera;
}
#endif

// List of Global Variables to be Used for Scripting
namespace Global
{
	// List of Keys and Their States
	extern bool* Keys;

	// Cursor Locations
	extern double* mouseX;
	extern double* mouseY;

	// Change in Mouse Positions
	extern glm::vec2* deltaMouse;

	// Relative Cursor Locations in World Space
	extern float* mouseRelativeX;
	extern float* mouseRelativeY;

	// Mouse Button Variables
	extern bool* LeftClick;
	extern bool* RightClick;

	// Determines How Far the Projection Matrix Should be Zoomed
	extern float* zoom_scale;

	// Pointer to Camera
	extern Render::Camera::Camera* camera;

	// Time Between Current and Last Frames
	extern float* deltaTime;
}
