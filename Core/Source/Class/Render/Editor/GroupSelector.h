#ifndef GROUP_SELECTOR_H
#define GROUP_SELECTOR_H

namespace Editor
{
	// A Specialized Selector Object That Allows Multiple Objects to be Selected Simultaniously
	// Selected Objects Can Only be Moved and Dialated
	// Object Exists as a Rectangle with Position in Center
	// Drag Object to Move, Drag Edges to Resize
	// Similar to Selecting Multiple Objects With Right Click and Drag (Almost Exactly Similar)
	// Does Not Use Editor Window, Instead Uses an Array of Selected Objects
	// Right Click on Objects to Select and Deselect Them When This Selector Mode is Used
	// If All Selected Objects Share At Least 1 Common Value, That Value Can be Modified in Editor Window (Useful for Editing Nodes and Springs in Spring Mass)
	class GroupSelector
	{

	};
}

#endif
