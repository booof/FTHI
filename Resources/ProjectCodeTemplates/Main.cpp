#ifdef BUILD_DLL
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif

namespace Render::Camera
{
	class Camera;
}

extern "C" EXPORT void __stdcall initialize_dll(bool* Keys, double* mouseX, double* mouseY, glm::vec2 * deltaMouse, float* mouseRelativeX, float* mouseRelativeY, bool* LeftClick, bool* RightClick, float* zoom_scale, Render::Camera::Camera * camera, float* delta_time);

// List of Global Variables to be Used for Scripting
namespace Global
{
	// List of Keys and Their States
	bool* Keys;

	// Cursor Locations
	double* mouseX;
	double* mouseY;

	// Change in Mouse Positions
	glm::vec2* deltaMouse;

	// Relative Cursor Locations in World Space
	float* mouseRelativeX;
	float* mouseRelativeY;

	// Mouse Button Variables
	bool* LeftClick;
	bool* RightClick;

	// Determines How Far the Projection Matrix Should be Zoomed
	float* zoom_scale;

	// Pointer to Camera
	Render::Camera::Camera* camera;

	// Time Between Current and Last Frames
	float* deltaTime;
}

// DLL Initializer Function
EXPORT void __stdcall initialize_dll(bool* Keys, double* mouseX, double* mouseY, glm::vec2* deltaMouse, float* mouseRelativeX, float* mouseRelativeY, bool* LeftClick, bool* RightClick, float* zoom_scale, Render::Camera::Camera* camera, float* delta_time)
{
	// Store All Pointers to EXE Vars
	Global::Keys = Keys;
	Global::mouseX = mouseX;
	Global::mouseY = mouseY;
	Global::deltaMouse = deltaMouse;
	Global::mouseRelativeX = mouseRelativeX;
	Global::mouseRelativeY = mouseRelativeY;
	Global::LeftClick = LeftClick;
	Global::RightClick = RightClick;
	Global::camera = camera;
	Global::zoom_scale = zoom_scale;
	Global::deltaTime = delta_time;
}
