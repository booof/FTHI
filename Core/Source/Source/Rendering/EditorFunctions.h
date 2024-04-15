#pragma once
#ifndef EDITOR_FUNCTIONS_H
#define EDITOR_FUNCTIONS_H

namespace Render
{
	class Container;
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
	void edit(Render::Container* container, Editor::Selector* selector, Editor::ObjectInfo* object_info);

	// Main Function for Drawing Editor Related Things
	void renderEditor(Render::Container* container, Editor::Selector* selector, Editor::ObjectInfo* object_info);
}

#endif
