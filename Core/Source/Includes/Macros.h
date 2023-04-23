#pragma once
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

// Macro for Accessing the Selected Text Singleton
#define selected_text GUI::SelectedText::get()

// Macro for Accessing the Project Selector Singleton
#define project_selector Editor::ProjectSelector::get()

// Macro for Accessing the Script Wizard Singleton
#define script_wizard Editor::ScriptWizard::get()

// Macro for Accessing the Debugger Singleton
#define debugger Editor::Debugger::get()

// Macro that Determines How to Build and Where to Get Project Dlls
#ifdef _DEBUG
#define CONFIG_DIR "Debug"
#else
#define CONFIG_DIR "Release"
#endif

// Macro to Disable Console Output
//#define DISABLE_OUTPUT
#ifdef DISABLE_OUTPUT
#define std::cout //
#endif

// Macro to Disable the Console Entirely
#define SHOW_CONSOLE
#ifdef SHOW_CONSOLE
#pragma comment(linker, "/SUBSYSTEM:console /ENTRY:mainCRTStartup")
#else
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#endif
