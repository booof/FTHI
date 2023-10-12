#include "SceneController.h"
#include "Globals.h"
#include "Render/Shader/Shader.h"
#include "Source/Vertices/Rectangle/RectangleVertices.h"
#include "Events/EventListeners.h"
#include "Render/Objects/Level.h"
#include "Source/Algorithms/Common/Common.h"
#include "Events/EventListeners.h"
#include "Render/GUI/GUI.h"
#include "Notification.h"
#include "Render\GUI\SelectedText.h"
#include "Source\Rendering\Exit.h"

void Editor::SceneController::prepareForMode(SCENE_MODES mode, bool force_change)
{
    // Only Early Return if Reload is Not Forced
    if (!force_change)
    {
        // If Mode is None, Do Nothing
        if (mode == SCENE_MODES::NONE)
            return;

        // If Mode is Equal to Current Mode, Also Do Nothing
        if (mode == current_mode)
            return;
    }

    // Store New Mode
    current_mode = mode;

    // Bind Vertex Object
    glBindVertexArray(windowVAO);
    glBindBuffer(GL_ARRAY_BUFFER, windowVBO);

    // Generate Vertices and Objects
    switch (mode)
    {
    case SCENE_MODES::LEVEL:    prepareLevelCreation();  break;
    case SCENE_MODES::GUI:      prepareGUICreation();    break;
    case SCENE_MODES::EDITING:  prepareEditing();        break;
    }

    // Unbind Vertex Object
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Editor::SceneController::genBackground()
{
    // Vertices Buffer
    float vertices[42] = { 0 };

    // Generate Black Background
    Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, 151.0f, 87.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    // Generate Center of Window
    Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, 150.0f, 86.0f, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(vertices), vertices);

    // Generate the Upper Horizontal Tab Seperator
    Vertices::Rectangle::genRectColor(0.0f, 33.0f, -1.0f, 150.0f, 0.5f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices) * 2, sizeof(vertices), vertices);
}

void Editor::SceneController::prepareLevelCreation()
{
    // Data Structures for Elements
    GUI::TextData text_data;
    GUI::BoxData box_data;

    // Generate the Background Only
    genBackgroundOnly();

    // Generate the Title
    genTitle();

    // Generate the Level Creator Text
    text_data.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    text_data.static_ = true;
    text_data.scale = 0.2f;
    text_data.position = glm::vec2(-70.0f, 27.0f);
    text_data.text = "Scene Creator";
    text.push_back(GUI::TextObject(text_data));

    // Generate the Level Creator Underline
    text_data.scale = 0.1f;
    text_data.position = glm::vec2(-70.0f, 25.0f);
    text_data.text = "-----------------------------------";
    text.push_back(GUI::TextObject(text_data));

    // Generate the Level Name Text
    text_data.scale = 0.12f;
    text_data.position = glm::vec2(-70.0f, 20.0f);
    text_data.text = "Scene Name: ";
    text.push_back(GUI::TextObject(text_data));

    // Generate the Level Name Box
    box_data.width = 100.0f;
    box_data.height = 5.0f;
    box_data.zpos = -0.9f;
    box_data.outline_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    box_data.background_color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    box_data.text_color = glm::vec4(0.0f, 0.0f, 0.8f, 1.0f);
    box_data.highlight_color = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    box_data.centered = false;
    box_data.position = glm::vec2(5.0f, 21.0f);
    box_data.button_text = std::string("");
    box_data.mode = GUI::BOX_MODES::GENERAL_TEXT_BOX;
    boxes.push_back(GUI::Box(box_data));
    boxes.at(1).setDataPointer(instance_name);

    // Generate the Chunk Size Text
    text_data.position = glm::vec2(-70.0f, 7.0f);
    text_data.text = "Chunk Dimensions:";
    text.push_back(GUI::TextObject(text_data));

    // Generate the Chunk Width Text
    text_data.position = glm::vec2(-30.0f, 10.0f);
    text_data.text = "Width  [X-Axis]:";
    text.push_back(GUI::TextObject(text_data));

    // Generate the Chunk Width Box
    box_data.width = 40.0f;
    box_data.centered = true;
    box_data.position = glm::vec2(20.0f, 11.0f);
    box_data.mode = GUI::BOX_MODES::ABSOLUTE_NUMERICAL_TEXT_BOX;
    boxes.push_back(GUI::Box(box_data));
    boxes.at(2).setDataPointer(&level_data->sublevel_width);

    // Generate the Chunk Height Text
    text_data.position = glm::vec2(-30.0f, 4.0f);
    text_data.text = "Height [Y-Axis]:";
    text.push_back(GUI::TextObject(text_data));

    // Generate the Chunk Height Box
    box_data.position = glm::vec2(20.0f, 5.0f);
    boxes.push_back(GUI::Box(box_data));
    boxes.at(3).setDataPointer(&level_data->sublevel_height);

    // Generate the Rest of the Chunk Elements
    genChunkLayoutElements(-8.0f, 4, text_data, box_data);

    // Generate the Finish Creation Button
    box_data.width = 80.0f;
    box_data.position = glm::vec2(0.0f, -55.0f);
    box_data.mode = GUI::BOX_MODES::FUNCTION_BOX;
    box_data.button_text = std::string("Finish Scene");
    boxes.push_back(GUI::Box(box_data));

    // Generate the Scroll Bar
    bar = GUI::VerticalScrollBar(74.0f, 32.5f, 2.0f, 75.5f, 95.0f, 0.0f);

    // Store Dynamic Starts
    dynamic_text_start = 3;
    dynamic_box_start = 1;
}

void Editor::SceneController::genTitle()
{
    // Clear GUI Elements
    boxes.clear();
    text.clear();
    bar.percent = 0.0f;

    // Generate the Title 
    GUI::TextData title;
    title.scale = 0.25f;
    title.static_ = true;
    title.color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    title.position = glm::vec2(-73.0f, 35.3f);
    title.text = "Scene Controller";
    text.push_back(GUI::TextObject(title));

    // Generate the Exit Box
    GUI::BoxData exit_box;
    exit_box.outline_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    exit_box.background_color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    exit_box.text_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    exit_box.button_text = std::string("X");
    exit_box.centered = true;
    exit_box.mode = GUI::BOX_MODES::FUNCTION_BOX;
    exit_box.width = 5.0f;
    exit_box.height = 5.0f;
    exit_box.highlight_color = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    exit_box.position = glm::vec2(70.5f, 39.0f);
    boxes.push_back(GUI::Box(exit_box));
}

void Editor::SceneController::genBackgroundOnly()
{
    // Number of Vertices in Editor Mode
    const int BACKGROUND_VERTEX_COUNT = 126;

    // Vertices Buffer
    float vertices[42] = { 0 };

    // Allocate Buffer Data
    glBufferData(GL_ARRAY_BUFFER, BACKGROUND_VERTEX_COUNT * sizeof(GL_FLOAT), 0, GL_STATIC_DRAW);

    // Generate Background
    genBackground();
}

void Editor::SceneController::prepareGUICreation()
{

}

void Editor::SceneController::prepareEditing()
{
    // Number of Vertices in Editor Mode
    const int EDITOR_VERTEX_COUNT = 378;

    // Vertices Buffer
    float vertices[42] = { 0 };

    // Current Offset
    int offset = 3 * sizeof(vertices);

    // Allocate Buffer Data
    glBufferData(GL_ARRAY_BUFFER, EDITOR_VERTEX_COUNT * sizeof(GL_FLOAT), 0, GL_STATIC_DRAW);

    // Generate Background
    genBackground();

    // Generate the Lower Horizontal Tab Seperator
    Vertices::Rectangle::genRectColor(0.0f, 26.0f, -1.0f, 150.0f, 0.5f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);
    offset += sizeof(vertices);

    // Generate the Center Tab Seperator
    Vertices::Rectangle::genRectColor(0.0f, 29.5f, -1.0f, 0.5f, 7.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);
    offset += sizeof(vertices);

    // Generate the Left Tab Seperator
    Vertices::Rectangle::genRectColor(-37.5f, 29.5f, -1.0f, 0.5f, 7.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);
    offset += sizeof(vertices);

    // Generate the Right Tab Seperator
    Vertices::Rectangle::genRectColor(37.5f, 29.5f, -1.0f, 0.5f, 7.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);
    offset += sizeof(vertices);

    // Generate the Tab Highlighter That Will be Rendered Seperatly
    Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, 35.0f, 5.0f, glm::vec4(0.0f, 0.4f, 0.8f, 0.95f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);
    offset += sizeof(vertices);

    // Generate the Tab Selector That Will Also be Rendered Seperatly
    Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, 35.5f, 5.5f, glm::vec4(0.0f, 0.0f, 0.8f, 0.95f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);

    // If No Editing Mode is Set, Load Variable Mode Elements
    if (editing_mode == EDITING_MODES::NONE)
        prepareEditingSubMode(EDITING_MODES::VARIABLES, true);

    // Else, Check the Current Editing Mode
    else
        prepareEditingSubMode(editing_mode, true);

    // Reset Force Reload Variable
    force_list_reload = false;
}

void Editor::SceneController::initializeSceneController()
{
    // Generate Vertex Objects
    glGenVertexArrays(1, &windowVAO);
    glGenBuffers(1, &windowVBO);
    glGenVertexArrays(1, &selectionVAO);
    glGenBuffers(1, &selectionVBO);

    // Bind Vertex Objects
    glBindVertexArray(windowVAO);
    glBindBuffer(GL_ARRAY_BUFFER, windowVBO);

    // Enable Position Data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
    glEnableVertexAttribArray(0);

    // Enable Color Data
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

    // Bind Vertex Objects
    glBindVertexArray(selectionVAO);
    glBindBuffer(GL_ARRAY_BUFFER, selectionVBO);

    // Enable Position Data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
    glEnableVertexAttribArray(0);

    // Enable Color Data
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

    // Unbind Vertex Objects
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Generate the Master Objects
    master_element_static = GUI::MasterElement(glm::vec2(0.0f, 0.0f), 150.0f, 86.0f);
    master_element_dynamic = GUI::MasterElement(glm::vec2(0.0f, 0.0f), 150.0f, 86.0f);
}

Editor::SceneController* Editor::SceneController::get()
{
    return &instance;
}

void* Editor::SceneController::getCurrentInstance()
{
    return currently_loaded_instance;
}

std::string& Editor::SceneController::getCurrentInstanceName()
{
    return *instance_name;
}

glm::mat4 Editor::SceneController::genTabMatrix(int index)
{
    return glm::translate(glm::mat4(1.0f), glm::vec3(-93.75f + 37.5f * (float)index, 29.5f, 0.0f));
}

void Editor::SceneController::readInstanceVector(std::ifstream& file, std::vector<Instance>& vec, int size)
{
    // Resize the Vector
    vec.resize(size);

    // Iterate Until All Instances are Read
    for (int i = 0; i < size; i++)
    {
        Instance& current_instance = vec.at(i);
        current_instance.index = i;
        file.read((char*)&current_instance.name_size, sizeof(current_instance.name_size));
        current_instance.name.resize(current_instance.name_size);
        file.read(&current_instance.name[0], current_instance.name_size);
    }
}

void Editor::SceneController::writeInstanceVector(std::ofstream& file, std::vector<Instance>& vec, int size)
{
    // Iterate Until All Instances are Written
    for (int i = 0; i < size; i++)
    {
        Instance& current_instance = vec.at(i);
        file.write((char*)&current_instance.name_size, sizeof(current_instance.name_size));
        file.write(current_instance.name.c_str(), current_instance.name_size);
    }
}

void Editor::SceneController::genLevelPath(int index)
{
    // I Don't Know Why, But C++ Acts Cringe Without the .c_str()
    instance_path = Global::project_resources_path + "\\Data\\Scenes\\" + levels.at(index).name.c_str() + "\\";
}

void Editor::SceneController::initializeLoop(SCENE_MODES mode, bool& looping, std::function<void()>& exit_function)
{
    // Enter Mode First
    prepareForMode(mode, false);

    // Bind Static Shader
    Global::colorShaderStatic.Use();
    glUniform1i(Global::staticLocColor, 1);

    // Bind Exit Function
    boxes.at(0).setFunctionPointer(exit_function);

    // Bind Vertical Scroll Bar
    master_element_dynamic.linkValue(new GUI::DefaultElements{ &bar, nullptr, &bar });
    glfwSetScrollCallback(Global::window, Source::Listeners::ScrollBarCallback);
    Global::scroll_bar = &bar;
}

void Editor::SceneController::renderInitial(float& modified_mouse_x, float& modified_mouse_y, glm::mat4& temp, bool& looping)
{
    // Clear Window
    glClearColor(0.45f, 0.45f, 0.45f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Handle Inputs
    glfwPollEvents();
    modified_mouse_x = (float)Global::mouseX / Global::zoom_scale;
    modified_mouse_y = (float)Global::mouseY / Global::zoom_scale;

    // Draw Window
    Global::colorShaderStatic.Use();
    glUniform1i(Global::staticLocColor, 1);
    glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(temp));
    glBindVertexArray(windowVAO);
    glDrawArrays(GL_TRIANGLES, 0, 42);

    // Draw Scroll Bar
    bar.blitzElement();

    // If ESC is Pressed, Exit Loop
    if (Global::Keys[GLFW_KEY_ESCAPE])
    {
        Global::Keys[GLFW_KEY_ESCAPE] = false;
        looping = false;
    }
}

void Editor::SceneController::renderFinal(glm::mat4& temp)
{
    // Draw Static Boxes
    glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(temp));
    for (std::_Vector_iterator it = boxes.begin(); it != boxes.begin() + dynamic_box_start; it++)
        it->blitzElement();

    // If Editing Mode is Active, Set Scissor Based on Current SubMode
    if (current_mode == SCENE_MODES::EDITING)
    {
        switch (editing_mode)
        {
        case EDITING_MODES::VARIABLES: glScissor(107, 53, 1048, 492); break;
        case EDITING_MODES::SCRITPS:   glScissor(107, 53, 1048, 492); break;
        case EDITING_MODES::LEVELS:
        case EDITING_MODES::GUIS:      glScissor(146, 92, 704, 410); break;
        }
    }

    // Levels and GUIs Use the Same Clamp
    else
        glScissor(107, 53, 1048, 492);

    // Draw Dynamic Boxes
    glUniformMatrix4fv(Global::modelLocRelativeFont, 1, GL_FALSE, glm::value_ptr(master_element_dynamic.getModel()));
    for (std::_Vector_iterator it = boxes.begin() + dynamic_box_start; it != boxes.end(); it++)
        it->blitzElement();

    // Draw Dynamic Text
    Global::fontOffsetShader.Use();
    glUniformMatrix4fv(Global::modelLocRelativeFont, 1, GL_FALSE, glm::value_ptr(master_element_dynamic.getModel()));
    for (std::_Vector_iterator it = text.begin() + dynamic_text_start; it != text.end(); it++)
        it->blitzOffsetText();

    // Draw Dynamic Boxes Text
    for (std::_Vector_iterator it = boxes.begin() + dynamic_box_start; it != boxes.end(); it++)
        it->blitzOffsetText();

    // Reset Clamp
    glScissor(0, 0, (GLsizei)Global::screenWidth, (GLsizei)Global::screenHeight);

    // Draw Static Boxes Text
    glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(temp));
    for (std::_Vector_iterator it = boxes.begin(); it != boxes.begin() + dynamic_box_start; it++)
        it->blitzOffsetText();

    // Draw Static Text
    for (std::_Vector_iterator it = text.begin(); it != text.begin() + dynamic_text_start; it++)
        it->blitzOffsetText();

    // Swap Buffer
    glfwSwapBuffers(Global::window);
    glFinish();
}

bool Editor::SceneController::testValidName()
{
    // Test if There are No Characters
    if (instance_name->size() == 0)
    {
        std::string message = "Warning: Name is Empty\n\nThe Instance Name Must Not be Empty";
        notification_->notificationMessage(NOTIFICATION_MESSAGES::NOTIFICATION_WARNING, message);
        return false;
    }

    // Test if There are Any Illegal Characters
    if (!Source::Algorithms::Common::testFileNameInvalidCharacters(*instance_name))
    {
        std::string message = "Warning: Invalid Character\n\nThe Instance Name Has an Invalid Character\n\nCharacters < > : \" / \\ | > * are Prohibited";
        notification_->notificationMessage(NOTIFICATION_MESSAGES::NOTIFICATION_WARNING, message);
        return false;
    }

    // Return True if Function Reaches Here
    return true;
}

bool Editor::SceneController::testValidLevel(void* backup)
{
    // Test Variables to See if They are Valid
    if (!testInvalidInputs(backup))
        return false;

    // Test if Level Name is Valid
    if (!testValidName())
        return false;

    // Get the Path to the Level Directory
    std::string path = Global::project_resources_path + "\\Data\\Scenes\\" + instance_name->c_str();

    // Test if Level Name Already Exists
    if (std::filesystem::is_directory(path))
    {
        std::string message = "Warning: Duplicate Name\n\nThe Scene Name Already Exists\n\nPlease Choose a Different Name";
        notification_->notificationMessage(NOTIFICATION_MESSAGES::NOTIFICATION_WARNING, message);
        return false;
    }

    // Return True Since Level is Valid and Ready to be Created
    return true;
}

bool Editor::SceneController::testValidGUI(void* backup)
{
    // Test Variables to See if They are Valid
    if (!testInvalidInputs(backup))
        return false;

    // Test if GUI Name is Valid
    if (!testValidName())
        return false;

    // Get the Path to the GUI Directory
    std::string path = Global::project_resources_path + "\\Data\\GUIs\\" + instance_name->c_str();

    // Test if Level Name Already Exists
    if (std::filesystem::is_directory(path))
    {
        std::string message = "Warning: Duplicate Name\n\nThe GUI Name Already Exists\n\nPlease Choose a Different Name";
        notification_->notificationMessage(NOTIFICATION_MESSAGES::NOTIFICATION_WARNING, message);
        return false;
    }

    // Return True Since Level is Valid and Ready to be Created
    return true;
}

void Editor::SceneController::testBoxes(int& highlighted_box)
{
    // Test Static Boxes First
    master_element_static.updateElement();

    // Test Collision With Boxes
    for (int i = 0; i < boxes.size(); i++)
    {
        // Once Static Boxes are Finished, Test Dynamic Boxes
        if (i == dynamic_box_start)
            master_element_dynamic.updateElement();

        if (boxes[i].updateElement())
        {
            highlighted_box = i;
            break;
        }
    }

    // If No Bock Was Selected, but Something was Clicked, Deselect Boxes
    if (highlighted_box != -1 && Global::LeftClick)
    {
        boxes[highlighted_box].setFalse();
        Global::LeftClick = false;
        highlighted_box = -1;
        selected_text->stopSelecting();
    }
}

void Editor::SceneController::loopCreation(bool& looping, bool& successfull, void* backup)
{
    // Variables for the Loop
    glm::mat4 temp = glm::mat4(1.0f);
    float modified_mouse_x = 0.0f;
    float modified_mouse_y = 0.0f;
    int highlighted_box = -1;

    // Assign the Closer
    selected_text->assignCloser([&]()->void { testInvalidInputs(backup); });

    // Loop
    while (!glfwWindowShouldClose(Global::window) && looping && !successfull)
    {
        // Initial Rendering
        renderInitial(modified_mouse_x, modified_mouse_y, temp, looping);
        selected_text->assignCloser([&]()->void { testInvalidInputs(backup); });

        // If Bar is Not Being Updated, Test Boxes
        if (!bar.updateElement())
            testBoxes(highlighted_box);

        // Final Rendering
        renderFinal(temp);
    }

    // Unbind Selected Text
    selected_text->assignCloser(nullptr);
    glfwSetScrollCallback(Global::window, Source::Listeners::ScrollCallback);
    delete backup;
}

void* Editor::SceneController::genBackupData()
{
    // If Editing a Level, Generate a New Scene Data Structure
    if (current_instance_type == INSTANCE_TYPES::LEVEL)
        return new Render::Objects::SceneData;

    // Else, Generate a New GUI Data Structure
    return new GUI::GUIData;
}

bool Editor::SceneController::testInvalidInputs(void* backup)
{
    // Determines if There Were No Invalid Inputs
    bool valid = true;

    // Test Level Values
    if (current_instance_type == INSTANCE_TYPES::LEVEL)
    {
        // Get Scene Data
        Render::Objects::SceneData& data = *static_cast<Render::Objects::SceneData*>(backup);

        // Test if Chunk Width is Equal to 0
        if (level_data->sublevel_width == 0.0f)
        {
            std::string message = "Warning: Invalid Input\n\nThe Chunk Width Must be a Positive\nNon-Zero Number";
            notification_->notificationMessage(NOTIFICATION_MESSAGES::NOTIFICATION_WARNING, message);
            level_data->sublevel_width = data.sublevel_width;
            valid = false;
        }

        // Test if Chunk Height is Equal to 0
        if (level_data->sublevel_height == 0.0f)
        {
            std::string message = "Warning: Invalid Input\n\nThe Chunk Height Must be a Positive\nNon-Zero Number";
            notification_->notificationMessage(NOTIFICATION_MESSAGES::NOTIFICATION_WARNING, message);
            level_data->sublevel_height = data.sublevel_height;
            valid = false;
        }

        // Test if Render Distance is Equal to 0
        if (level_data->render_distance == 0)
        {
            std::string message = "Warning: Invalid Input\n\nThe Render Distance Must be a Positive\nNon-Zero Integer";
            notification_->notificationMessage(NOTIFICATION_MESSAGES::NOTIFICATION_WARNING, message);
            level_data->render_distance = data.render_distance;
            valid = false;
        }

        // Test if Initial Zoom Scale is Less Than 0.01
        if (level_data->initial_scale < 0.01)
        {
            std::string message = "Warning: Invalid Input\n\nThe Initial Zoom Scale Must Be at\nLeast 0.01";
            notification_->notificationMessage(NOTIFICATION_MESSAGES::NOTIFICATION_WARNING, message);
            level_data->initial_scale = data.initial_scale;
            valid = false;
        }

        // To Store Changes in Backup, Copy Level Data to Backup
        data = *level_data;
    }

    // Test GUI Values
    else
    {
        // Get GUI Data
        GUI::GUIData& data = *static_cast<GUI::GUIData*>(backup);
    }

    //

    // Return Validitity of Data
    return valid;
}

void Editor::SceneController::writeInstanceData()
{
    // Write Changes to Local Variables
    if (current_instance_type == INSTANCE_TYPES::LEVEL)
        writeSceneData();
    else
        writeGUIData();

    // Write Changes to Globals
    writeGlobals();
}

void Editor::SceneController::writeSceneData()
{
    // Get the Scene Path
    std::string path = Global::project_resources_path + "\\Data\\Scenes\\" + instance_name->c_str() + "\\SceneData.dat";

    // Open the File
    std::ofstream scene_file = std::ofstream(path);

    // Write the Scene Data
    scene_file.write((char*)level_data, sizeof(Render::Objects::SceneData));

    // Write the Scene Name
    scene_file.write(instance_name->c_str(), level_data->name_size);

    // Close the File
    scene_file.close();
}

void Editor::SceneController::writeGUIData()
{
    // Get the GUI Path
    std::string path = Global::project_resources_path + "\\Data\\GUIs\\" + instance_name->c_str() + "\\GUIData.dat";

    // Open the File
    std::ofstream gui_file = std::ofstream(path);

    // Write the GI Data
    gui_file.write((char*)gui_data, sizeof(GUI::GUIData));

    // Write the GUI Name
    gui_file.write(instance_name->c_str(), gui_data->name_size);

    // Close the File
    gui_file.close();
}

void Editor::SceneController::writeSceneList()
{
    // Open the Scene List File
    std::ofstream scene_list_file = std::ofstream(Global::project_resources_path + "\\Data\\CommonData\\SceneList.dat");

    // Write the Scene List
    scene_list_file.write((char*)&list, sizeof(list));

    // Read the Levels Into the Level Vector
    writeInstanceVector(scene_list_file, levels, list.scene_count);

    // Read the GUIs Into the GUI Vector
    writeInstanceVector(scene_list_file, guis, list.gui_count);

    // Close the File
    scene_list_file.close();
}

void Editor::SceneController::writeGlobals()
{
}

int Editor::SceneController::getIndexFromMouse(float relative_mouse_y)
{
    master_element_dynamic.updateElementModel();
    return floor((20.0f + master_element_dynamic.getVerticalOffset() - relative_mouse_y) / 7.0f);
}

float Editor::SceneController::getPositionFromIndex(int index)
{
    master_element_dynamic.updateElementModel();
    return 16.5f - 7.0f * (float)index + master_element_dynamic.getVerticalOffset();
}

void Editor::SceneController::setDefault()
{
    // Only Execute This Function if In Level Mode
    if (editing_mode == EDITING_MODES::LEVELS)
    {
        // If Currently Selected Index is -1, Send Message
        if (selected_selection == -1)
        {
            notification_->notificationSoundOnly(NOTIFICATION_MESSAGES::NOTIFICATION_WARNING);
            return;
        }

        // Save the New Default Level Index
        list.defualt_level_index = selected_selection;

        // Write the Changes in the Scene List
        writeSceneList();
    }
}

void Editor::SceneController::loadInstance()
{
    // If Currently Selected Index is -1, Send Message
    if (selected_selection == -1)
    {
        notification_->notificationSoundOnly(NOTIFICATION_MESSAGES::NOTIFICATION_WARNING);
        return;
    }

    // Test if the Currently Loaded Instance is Not Saved
    if (!Source::Render::Exit::determineSafeToExit())
        return;

    // If the Currently Loaded Instance is Not Nullptr, Delete it
    if (currently_loaded_instance != nullptr)
    {
        // Write Changes to Instance Data
        writeInstanceData();

        // Delete the Instance
        delete currently_loaded_instance;
    }

    // If Current Mode is Level, Load the Level at the Index
    if (editing_mode == EDITING_MODES::LEVELS)
        loadLevel(selected_selection);

    // Else, Load the GUI at the Index
    else
        loadGUI(selected_selection);
}

void Editor::SceneController::loadLevel(int index)
{
    // Get the Level Path at the Level Index
    genLevelPath(index);

    // Generate the Level at the Origin
    Render::Objects::Level* new_level = new Render::Objects::Level(instance_path, 0.0f, 0.0f, false);
    
    // Retrieve the Scene Data
    new_level->getSceneInfo(&level_data, &instance_name);

    // Set the Mode to Editing Level
    current_instance_type = INSTANCE_TYPES::LEVEL;

    // Save the New Instance
    currently_loaded_instance = new_level;
    current_instance = &levels.at(index);
}

void Editor::SceneController::loadGUI(int index)
{
}

void Editor::SceneController::addInstance()
{
    // String to Prevent Path Deletion
    std::string initial_path = "";

    // Test File Identifier
    uint32_t test_identifier = 0;

    // Generate the File Explorer Dialogue
    OPENFILENAME file_dialogue = { 0 };
    TCHAR szFile[260] = { 0 };
    TCHAR szTitle[260] = { 0 };
    file_dialogue.lStructSize = sizeof(OPENFILENAME);
    file_dialogue.lpstrFile = szFile;
    file_dialogue.nMaxFile = sizeof(szFile);
    file_dialogue.lpstrFilter = ("Data Files (.DAT)");
    file_dialogue.nFilterIndex = 1;
    file_dialogue.lpstrFileTitle = szTitle;
    file_dialogue.nMaxFileTitle = sizeof(szTitle);
    file_dialogue.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    // Select Correct Starting Location
    if (editing_mode == EDITING_MODES::LEVELS) {
        file_dialogue.lpstrTitle = "Select A Valid Scene Folder";
        initial_path = Global::project_resources_path + "\\Data\\Scenes";
        test_identifier = 2099904513;
    } else {
        file_dialogue.lpstrTitle = "Select A Valid GUI Folder";
        initial_path = Global::project_resources_path + "\\Data\\GUIs";
        test_identifier = 2099904769;
    }

    // Store Starting Location
    file_dialogue.lpstrInitialDir = initial_path.c_str();

    // Allow User to Search for Project
    if (GetOpenFileName(&file_dialogue) == TRUE)
    {
        // Save the Path to the Data File
        std::string data_path = file_dialogue.lpstrFile;

        // Get the Selected File Name
        std::string file_name = Source::Algorithms::Common::getFileName(data_path, true);

        // If the File Name is Not "SceneData.dat", Throw Error
        if (file_name != std::string("SceneData.dat"))
        {
            std::string message = "ERROR: INVALID File\n\nThe Selected File is Not a Valid\n\"SceneData.dat\" File\n\nPlease Select a Valid File";
            notification_->notificationMessage(NOTIFICATION_MESSAGES::NOTIFICATION_ERROR, message);
            return;
        }

        // Get the Path to the Instance Directory
        std::string dir_path = Source::Algorithms::Common::getFilePath(data_path);

        // Get the Instance Name
        std::string instance_name = Source::Algorithms::Common::getFileName(dir_path, true);

        // Get Path of the Directory
        std::string test_path = Source::Algorithms::Common::getFilePath(dir_path);

        // If the Path of the Directory Does Not Equal the Initial Path, Throw Error
        if (test_path != initial_path)
        {
            std::string message = "ERROR: INVALID PATH\n\nThe Selected File is Not Located in the\nCorrect Directory\n\nPlease Select From Either the Scenes or\nGUIs Directories";
            notification_->notificationMessage(NOTIFICATION_MESSAGES::NOTIFICATION_ERROR, message);
            return;
        }

        // Read the Identifier for the "SceneData.dat" File
        uint32_t actual_identifier = 0;
        std::ifstream file_actual = std::ifstream(data_path);
        file_actual.read((char*)&actual_identifier, sizeof(uint32_t));
        file_actual.close();

        // If the Identifiers Don't Match, Throw Error
        if (actual_identifier != test_identifier)
        {
            std::string message = "ERROR: INVALID .DAT FILE\n\nThe Selected File is Not a Valid .DAT File\nor is a .DAT File Used for A Different Purpose\n\nPlease Select a Valid .DAT File";
            notification_->notificationMessage(NOTIFICATION_MESSAGES::NOTIFICATION_ERROR, message);
            return;
        }

        // If the Instance is Already Added, Throw Error
        if ((editing_mode == EDITING_MODES::LEVELS && testMatchingName(instance_name, levels)) || (editing_mode == EDITING_MODES::GUIS && testMatchingName(instance_name, guis)))
        {
            std::string message = "ERROR: DUPLICATE INSTANCE\n\nThe Selected File is Already Added\nTo the Project\n\nPlease Select a Different .DAT File";
            notification_->notificationMessage(NOTIFICATION_MESSAGES::NOTIFICATION_ERROR, message);
            return;
        }

        // Generate a New Instance for the Correct Vector
        if (editing_mode == EDITING_MODES::LEVELS)
            addInstanceToVector(instance_name, levels, list.scene_count);
        else
            addInstanceToVector(instance_name, guis, list.gui_count);

        // Write the Changes to the List File
        writeSceneList();

        // Read the Changes Back In
        loadScenes();
    }
}

void Editor::SceneController::addInstanceToVector(std::string& new_name, std::vector<Instance>& vec, uint16_t& list_count)
{
    // Copy the Index of the Currently Loaded Instance
    uint16_t current_instance_index = current_instance->index;

    // Add Instance to the Vector
    vec.push_back(Instance{ (uint16_t)vec.size(), (uint16_t)new_name.size(), new_name });

    // Increment the Number of Items in the List
    list_count++;

    // If Vector Type Is the Same as the Current Instance Type, Copy the New Current Instance Address
    if ((editing_mode == EDITING_MODES::LEVELS && current_instance_type == INSTANCE_TYPES::LEVEL)
        || (editing_mode == EDITING_MODES::GUIS && current_instance_type == INSTANCE_TYPES::GUI))
        current_instance = &vec.at(current_instance_index);
}

bool Editor::SceneController::testMatchingName(std::string& test_name, std::vector<Instance>& vec) 
{
    // Iterate Through Each Instance and Test Name
    for (Instance& instance : vec)
    {
        if (instance.name == test_name)
            return true;
    }

    // Return False Since No Matches Were Found
    return false;
}

void Editor::SceneController::removeInstanceHelperHelper(std::vector<Instance>& vec, uint16_t& count)
{
    // Flag is Current Instance Type Matches
    bool matching_types = false;

    // Flag if Current Instance is Being Removed
    bool current_instance_is_removed = false;

    // Name of the Current Instance to Regenerate
    std::string name_copy = "";

    // If Mode is Currently Level, Change Default Index
    if (editing_mode == EDITING_MODES::LEVELS)
    {
        // If Defualt Index is Greater Than Selected Index, Decrement Default
        if (list.defualt_level_index > selected_selection)
            list.defualt_level_index--;

        // If Default Index Equals Selected Index, Set Default Index to 0
        if (list.defualt_level_index == selected_selection)
            list.defualt_level_index = 0;
    }

    // If Current Instance Type Matches Editing Mode, Test if Current Instance is Being Removed
    if ((editing_mode == EDITING_MODES::LEVELS && current_instance_type == INSTANCE_TYPES::LEVEL)
        || (editing_mode == EDITING_MODES::GUIS && current_instance_type == INSTANCE_TYPES::GUI))
    {
        // Set the Matching Types Flag
        matching_types = true;

        // If Current Instance is Being Removed, Set Flag
        if (current_instance->index == selected_selection)
            current_instance_is_removed = true;

        // Else, Copy Name to Regenrate
        else
            name_copy = current_instance->name;
    }

    // Erase Instance at Index
    vec.erase(vec.begin() + selected_selection);

    // Decrement the Instance Count
    count--;

    // Save Changes to List
    writeSceneList();

    // Read the Scene Data To Finalize Changes
    loadScenes();

    // Test if Instance Needs to be Reloaded
    if (matching_types)
    {
        // If Loaded Instance Was Removed, Load the Default
        if (current_instance_is_removed)
            loadDefault();

        // Else, Search Through Vector for Matching Name
        else
        {
            for (Instance& instance : vec)
            {
                if (instance.name == name_copy)
                {
                    current_instance = &instance;
                    break;
                }
            }
        }
    }
}

bool Editor::SceneController::removeInstanceHelper()
{
    // If Currently Selected Index is -1, Send Message
    if (selected_selection == -1)
    {
        notification_->notificationSoundOnly(NOTIFICATION_MESSAGES::NOTIFICATION_WARNING);
        return false;
    }

    // If Mode is Selecting Levels and Levels Size is 1, Send Message
    if (editing_mode == EDITING_MODES::LEVELS && levels.size() == 1)
    {
        notification_->notificationSoundOnly(NOTIFICATION_MESSAGES::NOTIFICATION_WARNING);
        return false;
    }

    // Remove Instance From Correct Vector
    if (editing_mode == EDITING_MODES::LEVELS)
        removeInstanceHelperHelper(levels, list.scene_count);
    else
        removeInstanceHelperHelper(guis, list.gui_count);

    // Return True Since Removal Was Successfull
    return true;
}

void Editor::SceneController::removeInstance()
{
    // Use the Helper Function
    if (!removeInstanceHelper())
        return;

    // Set Selected Index to -1
    selected_selection = -1;
}

void Editor::SceneController::deleteInstance()
{
    // The Path To the Directory That Will be Removed
    std::string removal_path = "";

    // Don't Send Message if There is an Error
    if (!(selected_selection == -1 || (editing_mode == EDITING_MODES::LEVELS && levels.size() == 1)))
    {
        // Send Warning Message With Cancel Option
        std::string message1 = "WARNING: Instance DELETION\n\nThe Selected Instance is About to be Deleted\nAnd Will be Gone Forever\n\n(A Very Long Time)";
        if (!notification_->notificationCancelOption(NOTIFICATION_MESSAGES::NOTIFICATION_WARNING, message1))
            return;

        // Send Confirmation Message With Cancel Option
        std::string message2 = "Are You Sure?";
        if (!notification_->notificationCancelOption(NOTIFICATION_MESSAGES::NOTIFICATION_WARNING, message2))
            return;

        // Get the Removal Path Based on Current Instance Selection
        if (editing_mode == EDITING_MODES::LEVELS)
            removal_path = Global::project_resources_path + "\\Data\\Scenes\\" + levels.at(selected_selection).name.c_str();
        else
            removal_path = Global::project_resources_path + "\\Data\\GUIs\\" + guis.at(selected_selection).name.c_str();
    }

    // Remove Instance
    if (!removeInstanceHelper())
        return;

    // Delete the Folder of the Instance
    std::filesystem::remove_all(removal_path);

    // Set Selected Index to -1
    selected_selection = -1;
}

void Editor::SceneController::prepareEditingSubMode(EDITING_MODES mode, bool force_reload)
{
    // If Mode is Equal to Current Mode, Also Do Nothing
    if (!force_reload && mode == editing_mode)
        return;

    // Store New Editing Mode
    editing_mode = mode;

    // Generate the Title and Exit Box
    genTitle();

    // Generate the Variables Label 
    GUI::TextData tab;
    tab.scale = 0.2f;
    tab.static_ = true;
    tab.color = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
    tab.position = glm::vec2(-69.5f, 27.3f);
    tab.text = "Variables";
    text.push_back(GUI::TextObject(tab));

    // Generate the Scripts Label 
    tab.position = glm::vec2(-28.2f, 27.3f);
    tab.text = "Scripts";
    text.push_back(GUI::TextObject(tab));

    // Generate the Scenes Label 
    tab.position = glm::vec2(9.1f, 27.3f);
    tab.text = "Scenes";
    text.push_back(GUI::TextObject(tab));

    // Generate the GUIs Label 
    tab.position = glm::vec2(49.0f, 27.3f);
    tab.text = "GUIs";
    text.push_back(GUI::TextObject(tab));

    // Generate the Elements of the Specified Mode
    switch (editing_mode)
    {
    case EDITING_MODES::VARIABLES:  genVariableMode();  break;
    case EDITING_MODES::SCRITPS:    genScriptMode();    break;
    case EDITING_MODES::LEVELS:     genLevelMode();     break;
    case EDITING_MODES::GUIS:       genGUIMode();       break;
    }
}

void Editor::SceneController::genChunkLayoutElements(float initial_y, int initial_box, GUI::TextData& text_data, GUI::BoxData& box_data)
{
    // Generate Stationary Text
    text_data.position = glm::vec2(-70.0f, initial_y);
    text_data.text = "Enable Stationary Camera";
    text.push_back(GUI::TextObject(text_data));

    // Generate Stationary Box
    box_data.position = glm::vec2(-50.0f, initial_y - 5.0f);
    box_data.mode = GUI::BOX_MODES::TOGGLE_BOX;
    box_data.button_text = std::string("Stationary?");
    box_data.centered = true;
    box_data.width = 25.0f;
    boxes.push_back(GUI::Box(box_data));
    boxes.at(initial_box++).setDataPointer(&level_data->stationary);

    // Generate Wrapping Text
    text_data.position = glm::vec2(-1.0f, initial_y);
    text_data.text = "Enable Chunk Wrapping";
    text.push_back(GUI::TextObject(text_data));

    // Generate Wrapping Box
    box_data.position = glm::vec2(20.0f, initial_y - 5.0f);
    box_data.button_text = std::string("Wrapping?");
    boxes.push_back(GUI::Box(box_data));
    boxes.at(initial_box++).setDataPointer(&level_data->wrap_sublevels);

    // Generate Sublevel Count Text
    text_data.position = glm::vec2(-70.0f, initial_y - 15.0f);
    text_data.text = "Number of Chunks From Origin";
    text.push_back(GUI::TextObject(text_data));

    // Generate North Text
    text_data.position = glm::vec2(-65.0f, initial_y - 21.0f);
    text_data.text = "North  [+Y]:";
    text.push_back(GUI::TextObject(text_data));

    // Generate North Box
    box_data.mode = GUI::BOX_MODES::ABSOLUTE_INTEGER_TEXT_BOX;
    box_data.position = glm::vec2(-27.0f, initial_y - 20.0f);
    box_data.width = 30.0f;
    boxes.push_back(GUI::Box(box_data));
    boxes.at(initial_box++).setDataPointer(&level_data->sublevel_count_north);

    // Generate South Text
    text_data.position = glm::vec2(-65.0f, initial_y - 27.0f);
    text_data.text = "South  [-Y]:";
    text.push_back(GUI::TextObject(text_data));

    // Generate South Box
    box_data.position = glm::vec2(-27.0f, initial_y - 26.0f);
    boxes.push_back(GUI::Box(box_data));
    boxes.at(initial_box++).setDataPointer(&level_data->sublevel_count_south);

    // Generate East Text
    text_data.position = glm::vec2(-65.0f, initial_y - 33.0f);
    text_data.text = "East    [+Y]:";
    text.push_back(GUI::TextObject(text_data));

    // Generate East Box
    box_data.position = glm::vec2(-27.0f, initial_y - 32.0f);
    boxes.push_back(GUI::Box(box_data));
    boxes.at(initial_box++).setDataPointer(&level_data->sublevel_count_east);

    // Generate West Text
    text_data.position = glm::vec2(-65.0f, initial_y - 39.0f);
    text_data.text = "West   [-X]:";
    text.push_back(GUI::TextObject(text_data));

    // Generate West Box
    box_data.position = glm::vec2(-27.0f, initial_y - 38.0f);
    boxes.push_back(GUI::Box(box_data));
    boxes.at(initial_box++).setDataPointer(&level_data->sublevel_count_west);

    // Generate Render Distance Text
    text_data.position = glm::vec2(13.0f, initial_y - 22.0f);
    text_data.text = "Render Distance";
    text.push_back(GUI::TextObject(text_data));

    // Generate Clarification Text
    text_data.position = glm::vec2(5.0f, initial_y - 26.0f);
    text_data.text = "(Radius of Loaded Chunks)";
    text.push_back(GUI::TextObject(text_data));

    box_data.position = glm::vec2(27.25f, initial_y - 32.0f);
    boxes.push_back(GUI::Box(box_data));
    boxes.at(initial_box++).setDataPointer(&level_data->render_distance);
}

void Editor::SceneController::genSelectionList(std::vector<Instance>& vec)
{
    // Vertices
    float vertices[42] = { 0.0f };

    // Offsets
    int offset = 0;
    float y_offset = 7.0f;

    // Text Data
    GUI::TextData data;
    data.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    data.scale = 0.2f;
    data.static_ = true;

    // From the Vector Size, Determine the Total Number of Vertices to Generate
    int vertex_count = vec.size() * 42 + 42 * 10;
    selection_vertex_count = vertex_count / 7 - 54;

    // Bind Vertex Object
    glBindVertexArray(selectionVAO);
    glBindBuffer(GL_ARRAY_BUFFER, selectionVBO);

    // Allocate Vertices
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(GL_FLOAT), 0, GL_STATIC_DRAW);

    // Generate the Default Instance Identifier
    Vertices::Rectangle::genRectColor(-20.0f, -3.0f, -1.0f, 98.0f, 0.5f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);
    offset += sizeof(vertices);

    // Generate the Currently Loaded Instance Identifier
    Vertices::Rectangle::genRectColor(-20.0f, 0.0f, -1.0f, 98.0f, 5.0f, glm::vec4(0.0f, 0.0f, 0.5f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);
    offset += sizeof(vertices);

    // Generate the Selected Identifier
    Vertices::Rectangle::genRectColor(-20.0f, 0.0f, -1.0f, 97.0f, 4.5f, glm::vec4(0.0f, 0.0f, 0.8f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);
    offset += sizeof(vertices);

    // Generate the Highlighter
    Vertices::Rectangle::genRectColor(-20.0f, 0.0f, -1.0f, 96.0f, 4.0f, glm::vec4(0.0f, 0.0f, 0.9f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);
    offset += sizeof(vertices);

    // Generate the Background 
    float background_height = 7.0f * vec.size();
    background_height = (background_height < 58.0f) ? 58.0f : background_height;
    float background_y = 20.0f - background_height * 0.5f;
    Vertices::Rectangle::genRectColor(-20.0f, background_y, -1.0f, 100.0f, background_height, glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);
    offset += sizeof(vertices);

    // Generate the Top Rectangle
    Vertices::Rectangle::genRectColor(-20.0f, 20.0f, -1.0f, 100.0f, 1.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);
    offset += sizeof(vertices);

    // Generate the Bottom Rectangle
    Vertices::Rectangle::genRectColor(-20.0f, -38.0f, -1.0f, 100.0f, 1.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);
    offset += sizeof(vertices);

    // Generate the Left Rectangle
    Vertices::Rectangle::genRectColor(-70.0f, background_y, -1.0f, 1.0f, background_height + 1.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);
    offset += sizeof(vertices);

    // Generate the Right Rectangle
    Vertices::Rectangle::genRectColor(30.0f, background_y, -1.0f, 1.0f, background_height + 1.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);
    offset += sizeof(vertices);

    // Generate the Initial Top Rectangle
    Vertices::Rectangle::genRectColor(-20.0f, 20.0f, -1.0f, 100.0f, 1.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);
    offset += sizeof(vertices);

    // Iterate Through Vector List and Generate Each Necessary Instance
    for (Instance& instance : vec)
    {
        // Generate the Name Text Object
        data.position = glm::vec2(-68.0f, 21.0f - y_offset);
        data.text = instance.name;
        text.push_back(data);

        // Generate the Bottom Rectangle for the Instance
        Vertices::Rectangle::genRectColor(-20.0f, 20.0f - y_offset, -1.0f, 100.0f, 1.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
        glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vertices), vertices);
        offset += sizeof(vertices);
        y_offset += 7.0f;
    }

    // Unbind Vertex Object
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Generate the Scroll Bar
    bar = GUI::VerticalScrollBar(31.0f, 20.5f, 2.0f, 59.0f, background_height + 200.0f, 0.0f);
}

void Editor::SceneController::genVariableMode()
{
    // Data Structures for Elements
    GUI::TextData text_data;
    GUI::BoxData box_data;

    // Resize Vectors

    // Generate Level Name Text
    text_data.position = glm::vec2(-70.0f, 18.0f);
    text_data.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    text_data.scale = 0.18f;
    text_data.static_ = true;
    text_data.text = std::string("Scene Name: " + *instance_name);
    text.push_back(GUI::TextObject(text_data));

    // Generate Time Created Text
    text_data.position = glm::vec2(-70.0f, 8.0f);
    text_data.text = "Date Created: ";
    text.push_back(GUI::TextObject(text_data));

    // Generate the Local Variables Seperator
    text_data.position = glm::vec2(-68.0f, 3.0f);
    text_data.scale = 0.12f;
    text_data.text = "----------------------------------------  Local Variables  ----------------------------------------";
    text.push_back(GUI::TextObject(text_data));

    // Generate Gravity Text
    text_data.position = glm::vec2(-70.0f, -2.0f);
    text_data.text = "Gravity Scale Factor (Default 1.0)";
    text.push_back(GUI::TextObject(text_data));

    // Generate Gravity Box
    box_data.width = 40.0f;
    box_data.height = 5.0f;
    box_data.zpos = -0.9f;
    box_data.outline_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    box_data.background_color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    box_data.text_color = glm::vec4(0.0f, 0.0f, 0.8f, 1.0f);
    box_data.highlight_color = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    box_data.centered = false;
    box_data.position = glm::vec2(-45.0f, -7.0f);
    box_data.mode = GUI::BOX_MODES::NUMERICAL_TEXT_BOX;
    box_data.button_text = Source::Algorithms::Common::removeTrailingZeros(std::to_string(level_data->gravity_scale));
    boxes.push_back(GUI::Box(box_data));
    boxes.at(1).setDataPointer(&level_data->gravity_scale);

    // Generate Initial Zoom Scale Text
    text_data.position = glm::vec2(10.0f, -2.0f);
    text_data.text = "Initial Zoom Scale (Default 0.2)";
    text.push_back(GUI::TextObject(text_data));

    // Generate Initial Zoom Scale Box
    box_data.width = 40.0f;
    box_data.position = glm::vec2(35.0f, -7.0f);
    boxes.push_back(GUI::Box(box_data));
    boxes.at(2).setDataPointer(&level_data->initial_scale);

    // Generate Camera Position Text
    text_data.position = glm::vec2(-70.0f, -19.0f);
    text_data.text = "Initial Camera Position:";
    text.push_back(GUI::TextObject(text_data));

    // Generate Camera X Text
    text_data.position = glm::vec2(-26.0f, -16.0f);
    text_data.text = "Camera X:";
    text.push_back(GUI::TextObject(text_data));

    // Generate Camera X Box
    box_data.width = 40.0f;
    box_data.centered = true;
    box_data.position = glm::vec2(16.0f, -15.0f);
    boxes.push_back(GUI::Box(box_data));
    boxes.at(3).setDataPointer(&level_data->initial_camera_x);

    // Generate Camera Y Text
    text_data.position = glm::vec2(-26.0f, -22.0f);
    text_data.text = "Camera Y:";
    text.push_back(GUI::TextObject(text_data));

    // Generate Camera Y Box
    box_data.position = glm::vec2(16.0f, -21.0f);
    boxes.push_back(GUI::Box(box_data));
    boxes.at(4).setDataPointer(&level_data->initial_camera_y);

    // Generate Chunk Elements
    genChunkLayoutElements(-32.0f, 5, text_data, box_data);

    // Generate the Global Variables Seperator
    text_data.position = glm::vec2(-69.0f, -79.0f);
    text_data.scale = 0.12f;
    text_data.text = "----------------------------------------  Global Variables  ----------------------------------------";
    text.push_back(GUI::TextObject(text_data));

    // Generate the Scroll Bar
    bar = GUI::VerticalScrollBar(74.0f, 26.0f, 2.0f, 69.0f, 115.0f, 0.0f);

    // Store Dynamic Starts
    dynamic_text_start = 5;
    dynamic_box_start = 1;
}

void Editor::SceneController::genScriptMode()
{

}

void Editor::SceneController::genLevelMode()
{
    // Data Structures for Elements
    GUI::TextData text_data;
    GUI::BoxData box_data;

    // Generate Set Default Box
    box_data.width = 30.0f;
    box_data.height = 7.0f;
    box_data.zpos = -0.9f;
    box_data.outline_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    box_data.background_color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
    box_data.text_color = glm::vec4(0.0f, 0.0f, 0.8f, 1.0f);
    box_data.highlight_color = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f);
    box_data.centered = true;
    box_data.position = glm::vec2(54.0f, 17.0f);
    box_data.mode = GUI::BOX_MODES::FUNCTION_BOX;
    box_data.button_text = std::string("Set Default");
    boxes.push_back(GUI::Box(box_data));
    boxes.at(1).setFunctionPointer([&]()->void { setDefault(); });

    // Generate Load Level Box 
    box_data.position = glm::vec2(54.0f, 7.0f);
    box_data.button_text = std::string("Load Level");
    boxes.push_back(GUI::Box(box_data));
    boxes.at(2).setFunctionPointer([&]()->void { loadInstance(); });

    // Generate Add Level Box
    box_data.position = glm::vec2(54.0f, -3.0f);
    box_data.button_text = std::string("Add Level");
    boxes.push_back(GUI::Box(box_data));
    boxes.at(3).setFunctionPointer([this]()->void { addInstance(); });

    // Generate Create Level Box
    box_data.position = glm::vec2(54.0f, -13.0f);
    box_data.button_text = std::string("Create Level");
    boxes.push_back(GUI::Box(box_data));
    boxes.at(4).setFunctionPointer([this]()->void { generateNewLevel(); });

    // Generate Remove Level Box
    box_data.position = glm::vec2(54.0f, -23.0f);
    box_data.button_text = std::string("Remove Level");
    boxes.push_back(GUI::Box(box_data));
    boxes.at(5).setFunctionPointer([this]()->void { removeInstance(); });

    // Generate Delete Level Box
    box_data.position = glm::vec2(54.0f, -33.0f);
    box_data.button_text = std::string("Delete Level");
    boxes.push_back(GUI::Box(box_data));
    boxes.at(6).setFunctionPointer([this]()->void { deleteInstance(); });

    // Generate the Level List
    genSelectionList(levels);

    // Store Dynamic Starts
    dynamic_text_start = 5;
    dynamic_box_start = 7;
}

void Editor::SceneController::genGUIMode()
{

}

void Editor::SceneController::loadDefault()
{
    // If Instance is Not Nullptr, Delete it
    if (currently_loaded_instance != nullptr)
        delete currently_loaded_instance;

    // Load the Level at the Default Level Index
    loadLevel(list.defualt_level_index);
}

void Editor::SceneController::loadScenes()
{
    // Clear the Vectors
    levels.clear();
    guis.clear();

    // Open the Scene List File
    std::ifstream scene_list_file = std::ifstream(Global::project_resources_path + "\\Data\\CommonData\\SceneList.dat");

    // Read the Scene List
    scene_list_file.read((char*)&list, sizeof(list));

    // Read the Levels Into the Level Vector
    readInstanceVector(scene_list_file, levels, list.scene_count);

    // Read the GUIs Into the GUI Vector
    readInstanceVector(scene_list_file, guis, list.gui_count);

    // Close the File
    scene_list_file.close();

//#define WRITE_SCENE
#ifdef WRITE_SCENE

    static bool x = true;
    std::string y = "test";
    if (x)
    {
        x = false;
        levels.clear();
        guis.clear();
        list = SceneList{ 0, 0, 42, 125, 1, 0, 0 };
        levels.push_back(Instance{ 0, (uint16_t)y.size(), y });
        writeSceneList();
        loadScenes();
    }

#endif

    // State that Things Should be Force Reloaded
    force_list_reload = true;
}

bool Editor::SceneController::generateFirstLevel()
{
    // Clear the Instance Vectors
    levels.clear();
    guis.clear();

    // Set the Scene List to Have Only 1 Level
    list.scene_count = 1;
    list.gui_count = 0;
    list.defualt_level_index = 0;

    // Enter Level Creation Mode
    return generateNewLevel();
}

void Editor::SceneController::startEditingMode()
{
    // Loop Variables
    glm::mat4 temp = glm::mat4(1.0f);
    glm::mat4 highlighter_matrix = glm::mat4(1.0f);
    glm::mat4 selected_tab = genTabMatrix((int)editing_mode);
    int highlighted_tab = 0;
    int highlighted_box = -1;
    int highlighted_selection = -1;
    float modified_mouse_x = 0.0f;
    float modified_mouse_y = 0.0f;
    selected_selection = -1;

    // Initialize Loop
    bool looping = true;
    std::function exit_function = [&]()->void { looping = false; };
    initializeLoop(SCENE_MODES::EDITING, looping, exit_function);

    // Generate Backup Data 
    void* backup_data = genBackupData();
    selected_text->assignCloser([&]()->void { testInvalidInputs(backup_data); });

    // GUI Loop
    while (!glfwWindowShouldClose(Global::window) && looping)
    {
        // Initial Rendering
        renderInitial(modified_mouse_x, modified_mouse_y, temp, looping);
        selected_text->assignCloser([&]()->void { testInvalidInputs(backup_data); });

        // Draw Selected Tab
        glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(selected_tab));
        glDrawArrays(GL_TRIANGLES, 48, 6);
        glBindVertexArray(0);

        // If Currently Highlighting a Tab, Show Highlighter
        if (highlighted_tab)
        {
            // Draw Highlighter
            glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(highlighter_matrix));
            glBindVertexArray(windowVAO);
            glDrawArrays(GL_TRIANGLES, 42, 6);
            glBindVertexArray(0);

            // If Left Click, Switch Tabs
            if (Global::LeftClick)
            {
                Global::LeftClick = false;
                prepareEditingSubMode((EDITING_MODES)highlighted_tab, false);
                selected_tab = genTabMatrix((int)editing_mode);
                boxes.at(0).setFunctionPointer(exit_function);
                int selected_selection = -1;
                int highlighted_selection = -1;
            }
        }

        // If Mode is Select Level or Select GUI, Select the List
        if (editing_mode == EDITING_MODES::LEVELS || editing_mode == EDITING_MODES::GUIS)
        {
            // Draw the List
            glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(temp));
            glBindVertexArray(selectionVAO);
            glDrawArrays(GL_TRIANGLES, 24, 30);

            // Set the Scissor
            glScissor(146, 92, 704, 410);

            // Update Dynamic Master Element
            master_element_dynamic.updateElementModel();

            // Draw the Dynamic Vertex Objects
            glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(master_element_dynamic.getModel()));
            glDrawArrays(GL_TRIANGLES, 54, selection_vertex_count);

            // If Level Mode, Highlight the Defualt Level
            if (editing_mode == EDITING_MODES::LEVELS)
            {
                // Get the Model Matrix
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, getPositionFromIndex(list.defualt_level_index), 0.0f));

                // Draw the Object
                glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 0, 6);

                // Also Test if Highlighter is Past Boundaries
                if (highlighted_selection >= levels.size())
                    highlighted_selection = -1;
            }

            // Else, Test if Highlighter is Past Boundaries
            else if (highlighted_selection >= guis.size())
                highlighted_selection = -1;

            // If Current Mode Matches Current Instance, Draw Currently Loaded Identifier
            if ((current_instance_type == INSTANCE_TYPES::LEVEL && editing_mode == EDITING_MODES::LEVELS)
                || (current_instance_type == INSTANCE_TYPES::GUI && editing_mode == EDITING_MODES::GUIS))
            {
                // Get the Model Matrix
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, getPositionFromIndex(current_instance->index), 0.0f));

                // Draw the Object
                glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 6, 6);
            }

            // If Selecting, Draw Selecting Identifier
            if (selected_selection != -1)
            {
                // Get the Model Matrix
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, getPositionFromIndex(selected_selection), 0.0f));

                // Draw the Object
                glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 12, 6);
            }

            // If Highlighting, Draw Highlighter
            if (highlighted_selection != -1)
            {
                // Get the Model Matrix
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, getPositionFromIndex(highlighted_selection), 0.0f));

                // Draw the Object
                glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(model));
                glDrawArrays(GL_TRIANGLES, 18, 6);

                // If Left Click, Select the Instance
                if (Global::LeftClick)
                {
                    Global::LeftClick = false;
                    selected_selection = highlighted_selection;
                }
            }

            // Reset Scissor
            glScissor(0, 0, (GLsizei)Global::screenWidth, (GLsizei)Global::screenHeight);

            // Unbind the Vertex Object
            glBindVertexArray(0);
        }

        // If Cursor Moved, Update Mouse Location
        if (!bar.updateElement() && (Global::cursor_Move || Global::LeftClick))
        {
            // Reset Some Mouse Values
            highlighted_tab = 0;
            highlighted_selection = -1;

            // Test if Mouse is Inside the Window
            if (modified_mouse_x > -75.0f && modified_mouse_x < 75.0f && modified_mouse_y > -43 && modified_mouse_y < 43)
            {
                // If Mouse is Inside Tab Section, Highlight Tab
                if (modified_mouse_y > 26.0f && modified_mouse_y < 33)
                {
                    // Get the Tab Index
                    highlighted_tab = floor((75 + modified_mouse_x) / 37.5f) + 1;

                    // Calculate the Model Matrix for the Tab Highlighter
                    highlighter_matrix = genTabMatrix(highlighted_tab);
                }

                // If Selecting Instance and Mouse is Inside Window, Determine Instance
                else if ((editing_mode == EDITING_MODES::LEVELS || editing_mode == EDITING_MODES::GUIS) &&
                    modified_mouse_x > -70.0f && modified_mouse_x < 30.0f && modified_mouse_y > -38.0f && modified_mouse_y < 20.0f)
                    highlighted_selection = getIndexFromMouse(modified_mouse_y);

                // Else, Test Boxes
                else
                    testBoxes(highlighted_box);
            }

            // Reset More Variables
            Global::cursor_Move = false;
        }

        // If Mode Switched From Editing, Switch Back
        if (current_mode != SCENE_MODES::EDITING || force_list_reload)
        {
            prepareForMode(SCENE_MODES::EDITING, true);
            boxes.at(0).setFunctionPointer(exit_function);
            selected_text->assignCloser([&]()->void { testInvalidInputs(backup_data); });
            glfwSetScrollCallback(Global::window, Source::Listeners::ScrollBarCallback);
            int selected_selection = -1;
            int highlighted_selection = -1;
            continue;
        }

        // Final Rendering
        renderFinal(temp);
    }

    // Unstatic Project
    Global::colorShaderStatic.Use();
    glUniform1i(Global::staticLocColor, 0);

    // In the Event a Box is Active, Deactivate It
    for (GUI::Box& box_object : boxes)
        box_object.updateElement();

    // Unbind Bind ScrollBar
    glfwSetScrollCallback(Global::window, Source::Listeners::ScrollCallback);

    // Unbind Selected Text
    selected_text->assignCloser(nullptr);
    delete backup_data;

    // Save the Changes to the Variables
    writeInstanceData();

    // Reload the Current Instance to Get Variable Changes
}

bool Editor::SceneController::generateNewLevel()
{
    // Generate the New Scene Data Struct
    INSTANCE_TYPES old_type = current_instance_type;
    current_instance_type = INSTANCE_TYPES::LEVEL;
    Render::Objects::SceneData* old_data = level_data;
    Render::Objects::SceneData new_data;
    std::string* old_name = instance_name;
    std::string new_name = "New Scene";
    level_data = &new_data;
    instance_name = &new_name;

    // Initialize Loop
    bool looping = true;
    std::function exit_function = [&]()->void { looping = false; };
    initializeLoop(SCENE_MODES::LEVEL, looping, exit_function);
    void* backup = genBackupData();

    // Bind the Success Lambda
    bool successfull = false;
    std::function success = [&]()->void { successfull = testValidLevel(backup); };
    boxes.at(11).setFunctionPointer(success);

    // Scene Loop
    loopCreation(looping, successfull, backup);

    // If Successful, Create a New Instance
    if (successfull)
    {
        // Save the Name Size
        level_data->name_size = instance_name->size();

        // Append Instance to Scene Vector
        levels.push_back(Instance{ (uint16_t)levels.size(), level_data->name_size, *instance_name });
        list.scene_count++;

        // Get the Scene Path
        std::string path = Global::project_resources_path + "\\Data\\Scenes\\" + instance_name->c_str();

        // Generate the Directories
        std::filesystem::create_directory(path);
        std::filesystem::create_directory(path + "\\EditorLevelData");
        std::filesystem::create_directory(path + "\\LevelData");

        // Write the Scene File
        writeSceneData();

        // Write the Changes to the Instance List
        writeSceneList();
    }

    // Revert the Scene Data
    current_instance_type = old_type;
    level_data = old_data;
    instance_name = old_name;

    // Return the Success of the Level
    return successfull;
}

void Editor::SceneController::generateNewGUI()
{
    // Generate the New GUI Data Struct
    INSTANCE_TYPES old_type = current_instance_type;
    current_instance_type = INSTANCE_TYPES::GUI;
    GUI::GUIData* old_data = gui_data;
    GUI::GUIData new_data;
    std::string* old_name = instance_name;
    std::string new_name = "New GUI";
    gui_data = &new_data;
    instance_name = &new_name;

    // Initialize Loop
    bool looping = true;
    std::function exit_function = [&]()->void { looping = false; };
    initializeLoop(SCENE_MODES::GUI, looping, exit_function);
    void* backup = genBackupData();

    // Bind the Success Lambda
    bool successfull = false;
    std::function success = [&]()->void { successfull = testValidGUI(backup); };

    // GUI Loop
    loopCreation(looping, successfull, backup);

    // If Successful, Create a New Instance
    if (successfull)
    {
        // Save the Name Size
        gui_data->name_size = instance_name->size();

        // Append Instance to Scene Vector
        guis.push_back(Instance{ (uint16_t)guis.size(), gui_data->name_size, *instance_name });
        list.gui_count++;

        // Get the GUI Path
        std::string path = Global::project_resources_path + "\\Data\\GUIs\\" + instance_name->c_str();

        // Generate the Directories
        std::filesystem::create_directory(path);
        std::filesystem::create_directory(path + "\\EditorGUIData");
        std::filesystem::create_directory(path + "\\GUIData");

        // Write the Scene File
        writeGUIData();

        // Write the Changes to the Instance List
        writeSceneList();
    }

    // Revert the GUI Data
    current_instance_type = old_type;
    gui_data = old_data;
    instance_name = old_name;
}

Editor::SceneController Editor::SceneController::instance;
