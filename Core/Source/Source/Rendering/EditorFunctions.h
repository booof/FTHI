#ifndef EDITOR_FUNCTIONS_H
#define EDITOR_FUNCTIONS_H

namespace Render::Objects
{
	class Level;
}

namespace Editor
{
	class Selector;
	class ObjectInfo;
}

namespace Render::Camera
{
	class Camera;
}

namespace Source::Rendering::Editing
{
	// Main Function for Editor
	void edit(Render::Objects::Level* level, Editor::Selector* selector, Editor::ObjectInfo* object_info);

	// Main Function for Drawing Editor Related Things
	void renderEditor(Render::Objects::Level* level, Editor::Selector* selector, Editor::ObjectInfo* object_info);

	// Draw Level Boarders
	void drawLevelBoarders(Render::Camera::Camera* camera);
}

#endif
