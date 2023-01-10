#include "Notification.h"
#include "Class/Render/GUI/TextObject.h"
#include "Globals.h"
#include "Class/Render/Shader/Shader.h"
#include "Source/Rendering/Initialize.h"
#include "Source/Vertices/Rectangle/RectangleVertices.h"
#include "Source/Loaders/Textures.h"
#include "Source/Loaders/Audio.h"
#include "OPENAL/include/AL/alext.h"

void Editor::Notification::prepareForMessage(NOTIFICATION_MESSAGES type, Struct::SingleTexture** texture, GUI::TextObject** text, uint8_t& text_size, std::string& message)
{
    // Store Texture to be Used
    switch (type)
    {

    // Error Message
    case NOTIFICATION_MESSAGES::NOTIFICATION_ERROR:
    {
        // OpenAL Tutorial: https://www.youtube.com/watch?v=WvND0djMcfE
        // Research Paper: https://research.ncl.ac.uk/game/mastersdegree/workshops/audio/Sound%20Workshop.pdf
        // Guide: https://indiegamedev.net/2020/02/15/the-complete-guide-to-openal-with-c-part-1-playing-a-sound/
        // Worse Guid: https://www.openal.org/documentation/OpenAL_Programmers_Guide.pdf

        *texture = &texture_error;
        label_text.swapText("ERROR:");
        alSourcei(notificationSource, AL_BUFFER, 0);
        Source::Audio::loadAudioFile(notificationBuffer, std::string("../Resources/Sound/Notification/error.wav"));
        alSourcei(notificationSource, AL_BUFFER, notificationBuffer);
        alSourcePlay(notificationSource);
        break;
    }

    // Warning Message
    case NOTIFICATION_MESSAGES::NOTIFICATION_WARNING:
    {
        *texture = &texture_warning;
        label_text.swapText("WARNING:");
        alSourcei(notificationSource, AL_BUFFER, 0);
        Source::Audio::loadAudioFile(notificationBuffer, std::string("../Resources/Sound/Notification/warning.wav"));
        alSourcei(notificationSource, AL_BUFFER, notificationBuffer);
        alSourcePlay(notificationSource);
        break;
    }

    // Message Message
    case NOTIFICATION_MESSAGES::NOTIFICATION_MESSAGE:
    {
        *texture = &texture_message;
        label_text.swapText("MESSAGE:");
        alSourcei(notificationSource, AL_BUFFER, 0);
        Source::Audio::loadAudioFile(notificationBuffer, std::string("../Resources/Sound/Notification/message.wav"));
        alSourcei(notificationSource, AL_BUFFER, notificationBuffer);
        alSourcePlay(notificationSource);
        break;
    }

    }

    // Determine the Number of Lines in Message
    for (int i = 0; i < message.size(); i++)
        if (message[i] == '\n')
            text_size++;

    // Allocate Memory for Text Objects
    GUI::TextObject* text_;
    text_ = new GUI::TextObject[text_size];

    // Store Message in Text Objects
    uint8_t current_line = 0;
    GUI::TextData text_data;
    text_data = Source::Render::Initialize::constructText(-18.0f, 9.0f, 0.072f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), true, "");
    for (int i = 0; i < message.size(); i++)
    {
        // If Current Character is the New Line Character, Store Text and Increment Current Line
        if (message[i] == '\n')
        {
            text_[current_line] = GUI::TextData(text_data);
            text_data.position.y -= 3.0f;
            text_data.text = "";
            current_line++;
        }

        // Else, Append Character to String
        else
        {
            text_data.text += message[i];
        }
    }
    text_[current_line] = GUI::TextData(text_data);

    // Store Pointer to Text in Text Array
    *text = text_;

    // Disable Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Disable Depth Test
    glDisable(GL_DEPTH_TEST);
}

void Editor::Notification::initializeNotification()
{
    // Temp Holder for Vertex Data
    float color_vertices[42];
    float texture_vertices[30];

    // Generate Window Vertex Objects
    glGenVertexArrays(1, &windowVAO);
    glGenBuffers(1, &windowVBO);

    // Bind Window Vertex Object
    glBindVertexArray(windowVAO);
    glBindBuffer(GL_ARRAY_BUFFER, windowVBO);

    // Allocate Buffer Data
    glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 126, NULL, GL_STATIC_DRAW);

    // Generate Window Outline Background
    Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, 71.0f, 41.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), color_vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color_vertices), color_vertices);

    // Generate Main Window Top Background
    Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, 70.0f, 40.0f, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), color_vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(color_vertices), sizeof(color_vertices), color_vertices);

    // Generate Main Window Background
    Vertices::Rectangle::genRectColor(0.0f, -2.0f, -1.0f, 70.0f, 36.0f, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), color_vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(color_vertices) * 2, sizeof(color_vertices), color_vertices);

    // Enable Position Vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
    glEnableVertexAttribArray(0);

    // Enable Color Vertices
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

    // Unbind Vertex Objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Generate Label Vertex Objects
    glGenVertexArrays(1, &labelVAO);
    glGenBuffers(1, &labelVBO);

    // Bind Label Vertex Object
    glBindVertexArray(labelVAO);
    glBindBuffer(GL_ARRAY_BUFFER, labelVBO);

    // Generate Label Vertices
    Vertices::Rectangle::genRectTexture(-28.0f, 10.0f, -1.0f, 10.0f, 10.0f, texture_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texture_vertices), texture_vertices, GL_STATIC_DRAW);

    // Enable Position Vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(0));
    glEnableVertexAttribArray(0);

    // Enable Texture Coordintates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

    // Unbind Vertex Objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Generate Label Text
    label_text = GUI::TextObject(Source::Render::Initialize::constructText(-34.0f, 17.0f, 0.12f, glm::vec4(0.0, 0.0f, 0.0f, 1.0f), true, ""));

    // Generate Master Element
    master = GUI::MasterElement(glm::vec2(0.0f, 0.0f), 70.0f, 40.0f);

    // Generate OK Box
    GUI::BoxData temp_box_data = Source::Render::Initialize::constrtuctBox(GUI::BOX_MODES::NULL_BOX, 0.0f, 0.0f, -1.0f, 20.0f, 4.0f, true, "OK", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    box_ok = GUI::Box(temp_box_data);

    // Generate Cancel Box
    temp_box_data.button_text = "Cancel";
    box_cancel = GUI::Box(temp_box_data);

    // Generate Error Image
    Source::Textures::loadSingleTexture("../Resources/Textures/Notifications/Error.png", texture_error);

    // Generate Warning Image
    Source::Textures::loadSingleTexture("../Resources/Textures/Notifications/Warning.png", texture_warning);

    // Generate Message Image
    Source::Textures::loadSingleTexture("../Resources/Textures/Notifications/Question.png", texture_message);

    // Generate Notification Buffer
    alGenBuffers(1, &notificationBuffer);

    // Generate Notification Source
    alGenSources(1, &notificationSource);
    alSourcef(notificationSource, AL_PITCH, 1);
    alSourcef(notificationSource, AL_GAIN, 1.0f);
    alSource3f(notificationSource, AL_POSITION, 0, 0, 0);
    alSource3f(notificationSource, AL_VELOCITY, 0, 0, 0);
    alSourcei(notificationSource, AL_LOOPING, AL_FALSE);
}

Editor::Notification* Editor::Notification::get()
{
    return &instance;
}

void Editor::Notification::deleteNotification()
{
    // Delete Objects
    glDeleteVertexArrays(1, &windowVAO);
    glDeleteBuffers(1, &windowVBO);
    glDeleteVertexArrays(1, &labelVAO);
    glDeleteBuffers(1, &labelVBO);
    alDeleteSources(1, &notificationSource);
    alDeleteBuffers(1, &notificationBuffer);
}

void Editor::Notification::notificationMessage(NOTIFICATION_MESSAGES type, std::string& message)
{
    Struct::SingleTexture* texture = NULL;
    GUI::TextObject* text = NULL;
    uint8_t text_size = 1;
    glm::mat4 temp = glm::mat4(1.0f);

    // Prepare for Message
    prepareForMessage(type, &texture, &text, text_size, message);

    // Move OK Button to Bottom Center of Window
    box_ok.moveElement(0.0f, -17.0f);

    // Loop Until Button is Pressed
    bool stop_looping = false;
    while (!stop_looping)
    {
        // Clear Window
        glClearColor(0.45f, 0.45f, 0.45f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Handle Inputs
        glfwPollEvents();
        master.updateElement();

        // Draw Window
        Global::colorShaderStatic.Use();
        glUniform1i(Global::staticLocColor, 1);
        glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(temp));
        glBindVertexArray(windowVAO);
        glDrawArrays(GL_TRIANGLES, 0, 18);
        glBindVertexArray(0);

        // Draw Button
        box_ok.blitzElement();
        glUniform1i(Global::staticLocColor, 0);

        // Draw Label
        Global::texShaderStatic.Use();
        glUniform1i(Global::staticLocTexture, 1);
        glUniform1i(Global::directionLoc, 1);
        glUniformMatrix4fv(Global::modelLocTextureStatic, 1, GL_FALSE, glm::value_ptr(temp));
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        glBindVertexArray(labelVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUniform1i(Global::staticLocTexture, 0);

        // Draw Button Text
        Global::fontShader.Use();
        box_ok.blitzText();

        // Draw Label Text
        label_text.blitzText();

        // Draw Message
        for (int i = 0; i < text_size; i++)
            text[i].blitzText();

        // Perform Collision Detection on Box
        stop_looping = box_ok.updateElement();

        Global::LeftClick = false;

        // Swap Buffer
        glfwSwapBuffers(Global::window);
        glFinish();
    }

    // Delete Text
    delete[] text;

    // Idea: Make it so Only the Boxes Need to be Modified Since Nothing Else Will Change Each Frame
}

bool Editor::Notification::notificationCancelOption(NOTIFICATION_MESSAGES type, std::string& message)
{
    Struct::SingleTexture* texture = NULL;
    GUI::TextObject* text = NULL;
    uint8_t text_size = 1;
    glm::mat4 temp = glm::mat4(1.0f);

    // Prepare for Message
    prepareForMessage(type, &texture, &text, text_size, message);

    // Move OK Button to Bottom Left of Window
    box_ok.moveElement(-16.0f, -17.0f);

    // Move Cancel Button to Bottom Right of Window
    box_cancel.moveElement(16.0f, -17.0f);

    // Loop Until Button is Pressed
    uint8_t stop_looping = 0;
    while (!stop_looping)
    {
        // Clear Window
        glClearColor(0.45f, 0.45f, 0.45f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Handle Inputs
        glfwPollEvents();
        master.updateElement();

        // Draw Window
        Global::colorShaderStatic.Use();
        glUniform1i(Global::staticLocColor, 1);
        glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(temp));
        glBindVertexArray(windowVAO);
        glDrawArrays(GL_TRIANGLES, 0, 18);
        glBindVertexArray(0);

        // Draw Buttons
        box_ok.blitzElement();
        box_cancel.blitzElement();
        glUniform1i(Global::staticLocColor, 0);

        // Draw Label
        Global::texShaderStatic.Use();
        glUniform1i(Global::staticLocTexture, 1);
        glUniform1i(Global::directionLoc, 1);
        glUniformMatrix4fv(Global::modelLocTextureStatic, 1, GL_FALSE, glm::value_ptr(temp));
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texture->texture);
        glBindVertexArray(labelVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUniform1i(Global::staticLocTexture, 0);

        // Draw Button Texts
        Global::fontShader.Use();
        box_ok.blitzText();
        box_cancel.blitzText();

        // Draw Label Text
        label_text.blitzText();

        // Draw Message
        for (int i = 0; i < text_size; i++)
            text[i].blitzText();

        // Perform Collision Detection on OK Box
        if (box_ok.updateElement())
            stop_looping = 1;

        // Perform Collision Detection on Cancel Box
        if (box_cancel.updateElement())
            stop_looping = 2;

        Global::LeftClick = false;

        // Swap Buffer
        glfwSwapBuffers(Global::window);
        glFinish();
    }

    // Delete Text
    delete[] text;

    // Return The Box That was Selected
    return (stop_looping == 1);
}

Editor::Notification Editor::Notification::instance;
