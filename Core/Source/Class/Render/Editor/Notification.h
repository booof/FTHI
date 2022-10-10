#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include "ExternalLibs.h"
#include "Class/Render/GUI/Box.h"
#include "Class/Render/GUI/TextObject.h"
#include "Class/Render/Struct/Texture.h"

// This Class is a Simple GUI That Displays a Popup Message
// Similar to Windows Popup Notifications
// All Processes Will Halt Until Okay Button (Or Cancel Button if Enabled) is Pressed
// There Are Different Types of Notifications, Such as Warnings and Errors
// Each Type of Notification Will Have a Different Image Label and Header
// Each Time the Notification Is to be Called, A Function Will Take a Enum for Type of Error and String for Message as Arguments
// This Class Should Exist as a Singleton

namespace Editor
{
	// The Types of Messages
	enum class NOTIFICATION_MESSAGES : uint8_t
	{
		NOTIFICATION_ERROR = 1,
		NOTIFICATION_WARNING,
		NOTIFICATION_MESSAGE
	};

	// The Notification Class
	class Notification
	{
		// The Singleton Instance
		static Notification instance;

		// Window Vertex Object
		GLuint windowVAO = NULL, windowVBO = NULL;

		// Message Type Label Vertex Object
		GLuint labelVAO = NULL, labelVBO = NULL;

		// Label Text
		GUI::TextObject label_text;

		// Error Texture
		Struct::SingleTexture texture_error = Struct::SingleTexture();

		// Warning Texture
		Struct::SingleTexture texture_warning = Struct::SingleTexture();

		// Message Texture
		Struct::SingleTexture texture_message = Struct::SingleTexture();

		// OK Button
		GUI::Box box_ok;

		// Cancel Button
		GUI::Box box_cancel;

		// Audio Source
		ALuint notificationBuffer = NULL, notificationSource = NULL;

		// Initial Constructor
		Notification() {};

		// Prepare Object to Display a Message
		void prepareForMessage(NOTIFICATION_MESSAGES type, Struct::SingleTexture** texture, GUI::TextObject** text, uint8_t& text_size, std::string& message);

	public:

		// Remove the Copy Constructor
		Notification(const Notification&) = delete;

		// Initialize Object
		void initializeNotification();

		// Return the Singleton
		static Notification* get();

		// Delete the Singleton
		void deleteNotification();

		// Notification With OK Button
		void notificationMessage(NOTIFICATION_MESSAGES type, std::string& message);
		
		// Notification With OK Button and Cancel Button
		bool notificationCancelOption(NOTIFICATION_MESSAGES type, std::string& message);
	};
}

#endif
