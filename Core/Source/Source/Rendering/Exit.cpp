#include "Exit.h"

#include "ExternalLibs.h"
#include "Globals.h"
#include "Class/Render/Editor/Notification.h"
#include "Class/Render/Editor/ProjectSelector.h"
#include "Class/Render/Editor/ScriptWizard.h"
#include "Class/Render/Editor/Debugger.h"
#include "Class/Render/Objects/ChangeController.h"

inline bool anoying_function1()
{
	return change_controller->returnIfUnsaved();
}

inline void anoying_function2()
{
	// Save Change Controller
	if (change_controller->returnIfUnsaved())
		change_controller->save();

	// Save Scripts
	if (script_wizard->modified)
		script_wizard->writeScriptData();
}

void Source::Render::Exit::exitProgram()
{
	// Break the Connection to Dot Net
	FreeLibrary(Global::framework_handle);

	// Delete Notification
	notification_->deleteNotification();

	// Delete Project Selector
	project_selector->uninitializeProjectSelector();

	// Delete Script Wizard
	//script_wizard->

	// Delete Debugger
	//debugger->

	// Terminate the Audio Context
	alcDestroyContext(Global::audio_context);

	// Terminate the Audio Device
	alcCloseDevice(Global::audio_device);

	// Terminate the Window
	glfwTerminate();
}

bool Source::Render::Exit::determineSafeToExit()
{
	// Test if Something is Unsaved
	if (script_wizard->modified || anoying_function1())
	{
		// Prompt User to Reconsider
		std::string message1 = "WARNING: UNSAVED PROJECT\n\nProceding Will Cause All Unsaved\nProgress to be Lost!\n\nClick Cancel to Go Back";
		if (notification_->notificationCancelOption(Editor::NOTIFICATION_MESSAGES::NOTIFICATION_WARNING, message1))
		{
			std::string message2 = "Are You Sure?";
			if (notification_->notificationCancelOption(Editor::NOTIFICATION_MESSAGES::NOTIFICATION_WARNING, message2))
			{
				return true;
			}
		}

		// Prompt User if They Would Like to Save Right Now
		std::string message3 = "Would You Like To Save Right Now?\n\nPress OK to Save and Exit\n\nPress Cancel to Go Back to Editing";
		if (notification_->notificationCancelOption(Editor::NOTIFICATION_MESSAGES::NOTIFICATION_MESSAGE, message3))
		{
			anoying_function2();
			return true;
		}

		return false;
	}

	return true;
}
