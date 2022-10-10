#ifndef MACROS_H
#define MACROS_H

// If Defined, Editor Code is Enabled
// Includes Editor Loop, Editor GUIs, and Gives Objects
// the Ability to be Modified through Editing
#define EDITOR

// If Defined, Levels Will be Written and Read in Text
// Text Files Allows Debugging of Levels Through Files
// Else, Objects Will be Written and Read in Binary Files
#define READ_WRITE_TEXT

// Macro for Accessing the Change Controller Singleton
#define change_controller Render::Objects::ChangeController::get()

// Marco for Accessing the Notification Singleton
#define notification_ Editor::Notification::get()

// Macro for Accessing the Project Selector Singleton
#define project_selector Editor::ProjectSelector::get()

// Macro for Accessing the Script Wizard Singleton
#define script_wizard Editor::ScriptWizard::get()

// Macro for Accessing the Debugger Singleton
#define debugger Editor::Debugger::get()

#endif
