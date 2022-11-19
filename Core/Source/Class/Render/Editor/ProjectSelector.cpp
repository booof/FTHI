#include "ProjectSelector.h"
#include "Globals.h"
#include "Vertices/Rectangle/RectangleVertices.h"
#include "Render/Shader/Shader.h"
#include "Source/Rendering/Initialize.h"
#include "Source/Algorithms/Common/Common.h"
#include "Source/Loaders/Fonts.h"
#include "Source/Events/EventListeners.h"
#include "Render/Editor/ScriptWizard.h"
#include "Render/Editor/Notification.h"
#include "Render/Editor/Debugger.h"
#include "Source/Rendering/Exit.h"
#include "Render/Objects/ChangeController.h"

#include <strsafe.h>
//#include <shobjidl.h> 
#include <time.h>

typedef void(__stdcall* bindFunctionPointerPointers)(int, Object::Object*);
typedef void(__stdcall* updateGlobalScriptsPointers)();
typedef void(__stdcall* DLLMAIN)(bool*, double*, double*, glm::vec2*, float*, float*, bool*, bool*, float*, Render::Camera::Camera*, float*);

HINSTANCE test_dll;

void ErrorExit(LPTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw);
}

void Editor::ProjectSelector::readProjectListFile(ProjectInstance** instances, int& instance_count)
{
    // Open Project File
    std::ifstream file;
    //file.open("C:\\Users\\ellio\\OneDrive\\Documents\\Visual Studio 2019 - Copy\\projects\\Game Engine\\FTHI\\Resources\\Data\\EngineData\\projects.dat");
    file.open("../Resources/Data/projects.dat");

    // Project File is Empty
    std::stringstream file_stream;
    file_stream << file.rdbuf();
    if (file_stream.str().size() < 2)
    {
        instance_count = 0;
        *instances = nullptr;
        return;
    }
    file.seekg(0);

    // Read Startup Enabled Bool
    file.read((char*)&startup_enabled, 1);

    // Read Number of Projects and Allocate Memory for Instances
    char count;
    file.read(&count, 1);
    instance_count = count;
    *instances = new ProjectInstance[instance_count];

    // Read End Line Character
    char burner_char;
    file.read(&burner_char, 1);

    // Dereference the Instance List
    ProjectInstance* instances_ = *instances;

    // Read File Data
    int project_count = 0;
    int project_line_count = 0;
    std::string line = "";
    while (std::getline(file, line))
    {
        // Determine Where to Store File Data
        switch (project_line_count)
        {

        // Project Name
        case 0:
        {
            instances_[project_count].name = line;
            break;
        }

        // Project Path
        case 1:
        {
            instances_[project_count].path = line;
            break;
        }

        // Access Time
        case 2:
        {
            instances_[project_count].access_time = line;
            break;
        }

        // Raw Access Time
        case 3:
        {
            instances_[project_count].raw_access_time = Source::Algorithms::Common::convertStringToInt(line);
            break;
        }

        }

        // Increment Project Line Count
        project_line_count++;
        if (project_line_count == 4)
        {
            project_line_count = 0;
            project_count++;
        }
    }

    // Close File
    file.close();
}

void Editor::ProjectSelector::writeProjectListFile()
{

}

void Editor::ProjectSelector::recompileProject()
{
    // Reset Debugger Flags
    debugger->enableFlag();

    // Run Batch Script to Recompile the Project
    std::filesystem::remove(current_project_path + "\\Code\\compilation complete");
    //std::string cmd_path = "Start "" /b CALL \"C:\\Users\\ellio\\OneDrive\\Documents\\Visual Studio 2019 - Copy\\projects\\Game Engine\\FTHI\\Core\\EngineLibs\\ProjectCompiler\\ProjectCompiler.bat\" \"" + current_project_path + "\\Build\"";
    std::string cmd_path = "Start "" /b CALL \"" + Global::engine_path + "\\Core\\EngineLibs\\ProjectCompiler\\ProjectCompiler.bat\" \"" + current_project_path + "\\Build\"";
    system(cmd_path.c_str());
}

void Editor::ProjectSelector::toggleEngineMode()
{
    // Editor to Gameplay
    if (Global::editing)
    {
        // Return if Debugger Cannot Run
        if (!debugger->testIfCanRun())
            return;

        // Load DLL
        loadDLL();
    }

    // Gameplay to Editor
    else
    {
        // Free DLL
        FreeLibrary(test_dll);
        project_initialized = false;
    }

    Global::editing = !Global::editing;
    Global::reload_all = true;
}

void Editor::ProjectSelector::loadDLL()
{
    // Wait Until Compilation is Complete
    while (!std::filesystem::exists(std::filesystem::path(current_project_path + "\\Code\\compilation complete"))) {}

    // Free DLL if Currently Loaded
    if (project_initialized)
    {
        FreeLibrary(test_dll);
    }

    // Delete the PDB File Because The Program Will Crash Otherwise
    std::filesystem::remove(current_project_path + "\\Build\\Debug\\project_lib.pdb");

    // Free Error Log
    SetLastError(0);

    // Open the DLL
    //std::string dll_path = current_project_path + "\\Build\\Release\\project_lib.dll";
    std::string dll_path = current_project_path + "\\Build\\Debug\\project_lib.dll";
    test_dll = LoadLibrary(TEXT(dll_path.c_str()));
    if (GetLastError())
    {
        if (!test_dll)
            test_dll = GetModuleHandle("project_lib.dll");
        else
            ErrorExit((LPTSTR)"LoadLibrary");
    }

    if (test_dll != NULL)
    {
        bindFunctionPointerPointers funct3 = (bindFunctionPointerPointers)GetProcAddress(test_dll, "bindFunctionPointer");
        if (GetLastError())
            ErrorExit((LPTSTR)"GetProcAddress");
        updateGlobalScriptsPointers funct4 = (updateGlobalScriptsPointers)GetProcAddress(test_dll, "updateGlobalScripts");
        if (GetLastError())
            ErrorExit((LPTSTR)"GetProcAddress");
        DLLMAIN dll_main = (DLLMAIN)GetProcAddress(test_dll, "initialize_dll");
        dll_main(Global::Keys, &Global::mouseX, &Global::mouseY, &Global::deltaMouse, &Global::mouseRelativeX, &Global::mouseRelativeY, &Global::LeftClick, &Global::RightClick, &Global::zoom_scale, Global::camera_pointer, &Global::deltaTime);

        // Bind Script Functions
        Global::bindFunctionPointer = (bindFunctionPointerPointer)GetProcAddress(test_dll, "bindFunctionPointer");
        Global::updateGlobalScripts = (updateGlobalScriptsPointer)GetProcAddress(test_dll, "updateGlobalScripts");

        // Enable DLL Flag
        project_initialized = true;
    }
}

void Editor::ProjectSelector::loadProject()
{
    // Test if Current Project Equals Old Project
    if (Global::project_file_path == current_project_path + "\\" + current_project_name)
        return;

    // If Another Project is Currently Loaded, Prompt User if They Want to Save
    if (Global::project_file_path != "")
    {
        if (!Source::Render::Exit::determineSafeToExit())
        {
            return;
        }
    }

    // Bind Path to Level Data
    Global::project_file_path = current_project_path + "\\" + current_project_name;
    Global::level_data_path = current_project_path + "\\Data\\LevelData\\";
    Global::editor_level_data_path = current_project_path + "\\Data\\EditorLevelData\\";
    Global::project_map_path = current_project_path + "\\Code\\Maps\\";
    Global::project_scripts_path = current_project_path + "\\Code\\Scripts\\";
    Global::script_folder_path = current_project_path + "\\Scripts";
    Global::project_solution_path = current_project_path + "\\Build\\Project.sln";
    Global::project_name = current_project_name;

    // Read Script Files
    script_wizard->loadScriptData();

    // Tell Level to Reload All
    if (project_initialized)
        change_controller->reset();

    // Enable Project
    project_initialized = true;
}

void Editor::ProjectSelector::copyFileContents(std::string read_path, std::string write_path)
{
    // Files for Reading and Writing
    std::ifstream read;
    std::ofstream write;

    // Open Files
    read.open(read_path);
    write.open(write_path);

    // Copy File Contents
    write << read.rdbuf();

    // Close Files
    read.close();
    write.close();
}

std::string Editor::ProjectSelector::getDateAndTime()
{
    // Layout: mm/dd/yyyy h:min

    // Notes:
    // As of 2022: 13 leap years since 1970
    // Leap Count is Number of Leap Years Since 1970, Not Including the Current Year if it is a Leap Year

    // Time Zones
    enum TIME_ZONES : uint8_t
    {
        UST = 0,
        PST,
        MT,
        EST
    };

    // The Number of Days In Each Month (Febuary is Incremented by 1 if Leap Year)
    uint8_t days_per_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // The Time Offset (In Seconds) For Each Enumerated Time Zone
    int32_t time_zone_offset[4] = { 0, -28800, -25200, -18000 };

    // Get The Current Time (In Seconds) Since 1970
    time_t tm = time(NULL);

    // Modify the Time Based on the Selected Time Zone
    tm += time_zone_offset[PST] + 3600;

    // Calculate the Current Year and Leap Year Values
    time_t year = (time_t)floor(tm / 31536000);
    time_t leap_year_count = (time_t)floor((year + 1) / 4);
    days_per_month[2] += uint8_t(year % 4 == 0);
    tm -= year * (time_t)31536000;

    // Calculate the Current Day of the Year
    time_t day = (time_t)floor(tm / 86400);
    tm -= day * 86400;

    // Modify Days Based on Leap Year
    day -= leap_year_count - 2;
    if (day < 0)
    {
        year--;
        day += 365;
    }

    // Determine the Month and Day of the Month
    uint8_t month = 0;
    while (day - days_per_month[month] > 0)
    {
        day -= days_per_month[month];
        month++;
    }
    month++;

    // Calculate the Hours in 24-Clock Time
    uint8_t hour = (uint8_t)floor(tm / 3600);
    tm -= hour * 3600;

    // Determine the Minutes
    uint8_t min = (uint8_t)floor(tm / 60);
    std::string min_string = (min < 10) ? "0" + std::to_string(min) : std::to_string(min);

    // 24-Hour Mode
    if (use_24_hour_clock)
    {
        return std::string(std::to_string(month) + "/" + std::to_string(day) + "/" + std::to_string(1970 + year) + " " + std::to_string(hour) + ":" + min_string);
    }

    // AM/PM Mode
    else
    {
        // AM
        if (hour < 12)
        {
            // Midnight
            if (hour == 0)
            {
                return std::string(std::to_string(month) + "/" + std::to_string(day) + "/" + std::to_string(1970 + year) + " 12:" + min_string + " AM");
            }

            // Morning
            else
            {
                return std::string(std::to_string(month) + "/" + std::to_string(day) + "/" + std::to_string(1970 + year) + " " + std::to_string(hour) + ":" + min_string + " AM");
            }
        }

        // PM
        else
        {
            // Shift Hour Down by 12 Hours
            hour -= 12;

            // Noon
            if (hour == 0)
            {
                return std::string(std::to_string(month) + "/" + std::to_string(day) + "/" + std::to_string(1970 + year) + " 12:" + min_string + " PM");
            }

            // Afternoom
            else
            {
                return std::string(std::to_string(month) + "/" + std::to_string(day) + "/" + std::to_string(1970 + year) + " " + std::to_string(hour) + ":" + min_string + " PM");
            }
        }
    }
}

void Editor::ProjectSelector::addProjectToFile(ProjectInstance instance)
{
    // Copy File Data Into Stream
    std::stringstream file_stream;
    std::ifstream in_file;
    //in_file.open("C:\\Users\\ellio\\OneDrive\\Documents\\Visual Studio 2019 - Copy\\projects\\Game Engine\\FTHI\\Resources\\Data\\EngineData\\projects.dat");
    in_file.open("../Resources/Data/projects.dat");
    file_stream << in_file.rdbuf();
    in_file.close();

    // Open File for Writing
    std::ofstream out_file;
    //out_file.open("C:\\Users\\ellio\\OneDrive\\Documents\\Visual Studio 2019 - Copy\\projects\\Game Engine\\FTHI\\Resources\\Data\\EngineData\\projects.dat");
    out_file.open("../Resources/Data/projects.dat");

    // Project File is Empty
    if (file_stream.str().size() < 2)
    {
        // Write Initial Header
        out_file.put((char)1);
        out_file.put((char)1);
        out_file.put('\n');

        // Write Project
        out_file << instance.name << "\n";
        out_file << instance.path << "\n";
        out_file << instance.access_time << "\n";
        out_file << std::to_string(instance.raw_access_time) << "\n";
    }

    // There is Data Already in File
    else
    {
        // Copy the Startup Enabled Flag
        char temp_byte;
        file_stream.read(&temp_byte, 1);
        out_file.put(temp_byte);

        // Copy the Number of Projects Incremented by 1
        file_stream.read(&temp_byte, 1);
        temp_byte++;
        out_file.put(temp_byte);

        // Copy the End Line Character
        file_stream.read(&temp_byte, 1);
        out_file.put(temp_byte);

        // Write Project
        out_file << instance.name << "\n";
        out_file << instance.path << "\n";
        out_file << instance.access_time << "\n";
        out_file << std::to_string(instance.raw_access_time) << "\n";

        // Write Rest of File
        out_file << file_stream.rdbuf();
    }

    // Close File
    out_file.close();
}

void Editor::ProjectSelector::removeProjectFromFile(int index)
{
    // Copy File Data Into Stream
    std::stringstream file_stream;
    std::ifstream in_file;
    //in_file.open("C:\\Users\\ellio\\OneDrive\\Documents\\Visual Studio 2019 - Copy\\projects\\Game Engine\\FTHI\\Resources\\Data\\EngineData\\projects.dat");
    in_file.open("../Resources/Data/projects.dat");
    file_stream << in_file.rdbuf();
    in_file.close();

    // Open File for Writing
    std::ofstream out_file;
    //out_file.open("C:\\Users\\ellio\\OneDrive\\Documents\\Visual Studio 2019 - Copy\\projects\\Game Engine\\FTHI\\Resources\\Data\\EngineData\\projects.dat");
    out_file.open("../Resources/Data/projects.dat");

	// Copy the Startup Enabled Flag
	char temp_byte;
	file_stream.read(&temp_byte, 1);
	out_file.put(temp_byte);

	// Copy the Number of Projects Decremented by 1
	file_stream.read(&temp_byte, 1);
	temp_byte--;
	out_file.put(temp_byte);

	// Copy the End Line Character
	file_stream.read(&temp_byte, 1);
	out_file.put(temp_byte);

    // Read Lines From Stream Into File, Excluding the Project to  Remove
    int project_count = 0;
    int project_line_count = 0;
    std::string line = "";
    while (std::getline(file_stream, line))
    {
        // Don't Copy Project if The Current Project Index Equals the Project Index to Remove
        if (project_count != index)
            out_file << line << "\n";

        // Increment Project Line Count
        project_line_count++;
        if (project_line_count == 4)
        {
            project_line_count = 0;
            project_count++;
        }
    }

    // Close File
    out_file.close();
}

bool Editor::ProjectSelector::createProject()
{
    // Documentation for OPENFILENAME
    //https://docs.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-openfilenamea

    // Generate Open File Dialogue
    OPENFILENAME file_dialogue = { 0 };
    TCHAR szFile[260] = { 0 };
    TCHAR szTitle[260] = { 0 };
    // Initialize remaining fields of OPENFILENAME structure
    file_dialogue.lStructSize = sizeof(OPENFILENAME);
    //file_dialogue.hwndOwner = hWnd;
    file_dialogue.lpstrFile = szFile;
    file_dialogue.nMaxFile = sizeof(szFile);
    file_dialogue.lpstrFilter = ("Engine Project File\0*.DPRJ");
    file_dialogue.nFilterIndex = 1;
    file_dialogue.lpstrFileTitle = szTitle;
    file_dialogue.lpstrTitle = "Select A Valid Project File (.dprj)";
    file_dialogue.nMaxFileTitle = sizeof(szTitle);
    //file_dialogue.lpstrInitialDir = "C:\\Users\\ellio\\OneDrive\\Documents\\Visual Studio 2019 - Copy\\projects\\Game Engine\\Projects";
    file_dialogue.lpstrInitialDir = Global::default_project_directory_path.c_str();
    file_dialogue.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    // Allow User to Search for Project
    if (GetSaveFileName(&file_dialogue) == TRUE)
    {
        // Store the Current Project Path File Path and File Name
        current_project_path = file_dialogue.lpstrFile;
        current_project_name = file_dialogue.lpstrFileTitle;

        // Activate Generate the Folders for the Project
        std::filesystem::create_directories(file_dialogue.lpstrFile);

        std::filesystem::create_directories(current_project_path + "\\Build");
        std::filesystem::create_directories(current_project_path + "\\Code");
        std::filesystem::create_directories(current_project_path + "\\Code\\Maps");
        std::filesystem::create_directories(current_project_path + "\\Code\\Scripts");
        std::filesystem::create_directories(current_project_path + "\\Data");
        std::filesystem::create_directories(current_project_path + "\\Data\\CommonData");
        std::filesystem::create_directories(current_project_path + "\\Data\\LevelData");
        std::filesystem::create_directories(current_project_path + "\\Data\\EditorLevelData");
        std::filesystem::create_directories(current_project_path + "\\Fonts");
        std::filesystem::create_directories(current_project_path + "\\Music");
        std::filesystem::create_directories(current_project_path + "\\Models");
        std::filesystem::create_directories(current_project_path + "\\Scripts");
        std::filesystem::create_directories(current_project_path + "\\Sound");
        std::filesystem::create_directories(current_project_path + "\\Textures");

        // Generate Copies of Code Files
        //std::string script_template_path = "C:\\Users\\ellio\\OneDrive\\Documents\\Visual Studio 2019 - Copy\\projects\\Game Engine\\FTHI\\Resources\\ProjectCodeTemplates";
        std::string script_template_path = "../Resources/ProjectCodeTemplates";
        copyFileContents(script_template_path + "\\Main.cpp", current_project_path + "\\Code\\Main.cpp");
        copyFileContents(script_template_path + "\\ScriptHandler.cpp", current_project_path + "\\Code\\ScriptHandler.cpp");
        copyFileContents(script_template_path + "\\Globals.h", current_project_path + "\\Code\\Globals.h");
        copyFileContents(script_template_path + "\\Constants.h", current_project_path + "\\Code\\Constants.h");
        copyFileContents(script_template_path + "\\ArraySizes.h", current_project_path + "\\Code\\Scripts\\ArraySizes.h");
        copyFileContents(script_template_path + "\\GlobalScriptHeader.h", current_project_path + "\\Code\\Scripts\\GlobalScriptHeader.h");
        copyFileContents(script_template_path + "\\GlobalScripts.h", current_project_path + "\\Code\\Scripts\\GlobalScripts.h");
        copyFileContents(script_template_path + "\\ObjectScriptHeader.h", current_project_path + "\\Code\\Scripts\\ObjectScriptHeader.h");
        copyFileContents(script_template_path + "\\ObjectScripts.h", current_project_path + "\\Code\\Scripts\\ObjectScripts.h");
        copyFileContents(script_template_path + "\\FileList.lst", current_project_path + "\\Code\\Maps\\FileList.lst");
        copyFileContents(script_template_path + "\\ScriptMap.lst", current_project_path + "\\Code\\Maps\\ScriptMap.lst");
        copyFileContents(script_template_path + "\\TextureMap.lst", current_project_path + "\\Code\\Maps\\TextureMap.lst");
        copyFileContents(script_template_path + "\\MaterialMap.lst", current_project_path + "\\Code\\Maps\\MaterialMap.lst");
        copyFileContents(script_template_path + "\\PhysicsMatrerialMap.lst", current_project_path + "\\Code\\Maps\\PhysicsMaterialMap.lst");
        copyFileContents(script_template_path + "\\ScriptCounts.lst", current_project_path + "\\Code\\Scripts\\ScriptCounts.lst");

        // Generate the CmakeLists File 
        copyFileContents(script_template_path + "\\CMakeLists.txt", current_project_path + "\\Code\\CMakeLists.txt");

        // Build the Initial Project
        //std::string cmd_path = "start "" /b CALL \"C:\\Users\\ellio\\OneDrive\\Documents\\Visual Studio 2019 - Copy\\projects\\Game Engine\\FTHI\\Core\\EngineLibs\\ProjectBuilder\\ProjectBuilder.bat\" \"" + current_project_path + "\\Build\"";
        std::string cmd_path = "start "" /b CALL \"" + Global::engine_path + "\\Core\\EngineLibs\\ProjectBuilder\\ProjectBuilder.bat\" \"" + current_project_path + "\\Build\"";
        system(cmd_path.c_str());

        // Generate and Write to the Project File
        std::ofstream project_file;
        project_file.open(current_project_path + "\\" + current_project_name + ".dprj");
        project_file << current_project_name << "\n";
        std::string current_time = getDateAndTime();
        project_file << "Date Created: " << current_time << "\n";
        project_file << "Date Last Modified: " << current_time << "\n";
        project_file.close();

        // Generate Project Instance From Project
        ProjectInstance instance;
        instance.name = current_project_name;
        instance.path = current_project_path;
        instance.access_time = current_time;
        instance.raw_access_time = time(NULL);

        // Write Addition to the Project File
        addProjectToFile(instance);

        // Store Index of New Project
        current_project_index = 0;

        // Load Project
        loadProject();

        // Wait Until Project is Finished Building
        while (!std::filesystem::exists(std::filesystem::path(current_project_path + "\\Code\\compilation complete"))) {}

        return true;
    }

    return false;
}

bool Editor::ProjectSelector::addProject(ProjectInstance* instances, int& instance_count)
{
    // Documentation for OPENFILENAME
    //https://docs.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-openfilenamea

    // Generate Open File Dialogue
    OPENFILENAME file_dialogue = { 0 };
    TCHAR szFile[260] = { 0 };
    TCHAR szTitle[260] = { 0 };
    // Initialize remaining fields of OPENFILENAME structure
    file_dialogue.lStructSize = sizeof(OPENFILENAME);
    //file_dialogue.hwndOwner = hWnd;
    file_dialogue.lpstrFile = szFile;
    file_dialogue.nMaxFile = sizeof(szFile);
    file_dialogue.lpstrFilter = ("Engine Project File\0*.DPRJ");
    file_dialogue.nFilterIndex = 1;
    file_dialogue.lpstrFileTitle = szTitle;
    file_dialogue.lpstrTitle = "Select A Valid Project File (.dprj)";
    file_dialogue.nMaxFileTitle = sizeof(szTitle);
    //file_dialogue.lpstrInitialDir = "C:\\Users\\ellio\\OneDrive\\Documents\\Visual Studio 2019 - Copy\\projects\\Game Engine\\Projects";
    file_dialogue.lpstrInitialDir = Global::default_project_directory_path.c_str();
    file_dialogue.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    // Allow User to Search for Project
    if (GetOpenFileName(&file_dialogue) == TRUE)
    {
        // Test if File is a Duplicate
        for (int i = 0; i < instance_count; i++)
        {
            if (file_dialogue.lpstrFile == instances[i].path + "\\" + instances[i].name + ".dprj")
            {
                // Send Notification Message
                glScissor(0, 0, (GLsizei)Global::screenWidth, (GLsizei)Global::screenHeight);
                std::string message = "WARNING: PROJECT ALREADY ADDED\n\nThis Project Has Already Been Added to\nThe List of Projects\n\nThe Project Will be Automatically Opened\nUpon Closing This Notification";
                notification_->notificationMessage(EDITOR::NOTIFICATION_MESSAGES::NOTIFICATION_WARNING, message);

                // Store Current Project Path
                current_project_name = instances[i].name;
                current_project_path = instances[i].path;

                // Store Index of Project
                current_project_index = i;

                // Load Project
                loadProject();

                // Recompile Project
                recompileProject();

                return true;
            }
        }

        // Store Project Path and Project Name
        std::string temp_path = std::string(file_dialogue.lpstrFile);
        std::string temp_name = std::string(file_dialogue.lpstrFileTitle);
        int temp_string_size = (int)temp_path.size() - (int)temp_name.size() - 1;
        current_project_path.resize(temp_string_size);
        for (int i = 0; i < temp_string_size; i++)
            current_project_path[i] = temp_path[i];
        temp_string_size = (int)temp_name.size() - 5;
        current_project_name.resize(temp_string_size);
        for (int i = 0; i < temp_string_size; i++)
            current_project_name[i] = temp_name[i];

        // Generate Project Instance From Project
        ProjectInstance instance;
        instance.name = current_project_name;
        instance.path = current_project_path;
        instance.access_time = getDateAndTime();
        instance.raw_access_time = time(NULL);

        // Write Addition to the Project File
        addProjectToFile(instance);

        // Store Index of New Project
        current_project_index = 0;

        // Load Project
        loadProject();

        // Recompile Project
        recompileProject();

        return true;
    }

    return false;
}

void Editor::ProjectSelector::initializeProjectSelector()
{
	// Generate Window Vertex Object
    glGenVertexArrays(1, &windowVAO);
    glGenBuffers(1, &windowVBO);

    // Bind Window Vertex Object
    glBindVertexArray(windowVAO);
    glBindBuffer(GL_ARRAY_BUFFER, windowVBO);

    // Allocate Memory for Window
    glBufferData(GL_ARRAY_BUFFER, 672, NULL, GL_STATIC_DRAW);

    // Generate Outline Vertices
    float vertices[42];
    Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, 131.0f, 76.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 168, vertices);

    // Generate Background Vertices
    Vertices::Rectangle::genRectColor(0.0f, 0.0f, -1.0f, 130.0f, 75.0f, glm::vec4(0.3f, 0.3f, 0.3f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 168, 168, vertices);

    // Generate Highlight Vertices
    Vertices::Rectangle::genRectColor(-21.5, 0.0f, -1.0f, 82.0f, 9.0f, glm::vec4(0.0f, 0.0f, 0.8f, 1.0f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 336, 168, vertices);

    // Generate Selected Vertices
    Vertices::Rectangle::genRectColor(-21.5f, 0.0f, -1.0f, 83.0f, 10.0f, glm::vec4(0.8f, 0.8f, 0.8f, 0.6f), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 504, 168, vertices);

    // Enable Position Vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(0));
    glEnableVertexAttribArray(0);

    // Enable Color Vertices
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (void*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

    // Unbind Vertex Object
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Generate Search Box
    GUI::BoxData temp_box_data = Source::Render::Initialize::constrtuctBox(GUI::BOX_MODES::GENERAL_TEXT_BOX, 40.0f, 31.0f, -1.0f, 40.0f, 5.0f, false, "", glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
    box_search = GUI::Box(temp_box_data);
    box_search.setDataPointer(&search_text);

    // Generate Open Project Box
    temp_box_data = Source::Render::Initialize::constrtuctBox(GUI::BOX_MODES::NULL_BOX, 42.0f, 20.0f, -1.0f, 35.0f, 5.0f, true, "Open Project", glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
    box_open_project = GUI::Box(temp_box_data);

    // Generate Open Project Directory Box
    temp_box_data.position.y = 10.0f;
    temp_box_data.button_text = "Open Directory";
    box_open_project_directory = GUI::Box(temp_box_data);

    // Generate Remove Project Box
    temp_box_data.position.y = 0.0f;
    temp_box_data.button_text = "Remove Project";
    box_remove_project = GUI::Box(temp_box_data);

    // Generate Create Project Box
    temp_box_data.position.y = -10.0f;
    temp_box_data.button_text = "Create Project";
    box_create_project = GUI::Box(temp_box_data);

    // Generate Add Project Box
    temp_box_data.position.y = -20.0f;
    temp_box_data.button_text = "Add Project";
    box_add_project = GUI::Box(temp_box_data);

    // Generate Add Project Box
    temp_box_data = Source::Render::Initialize::constrtuctBox(GUI::BOX_MODES::TOGGLE_BOX, 47.0f, -30.0f, -1.0f, 3.0f, 3.0f, true, "", glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), glm::vec4(0.6f, 0.6f, 0.6f, 1.0f), glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
    box_startup_enabled = GUI::Box(temp_box_data);

    // Generate Master Element
    master_element = GUI::MasterElement(glm::vec2(0.0f, 0.0f), 100.0f, 100.0f);

    // Open Project File
    std::ifstream project_list_file;
    //project_list_file.open("C:\\Users\\ellio\\OneDrive\\Documents\\Visual Studio 2019 - Copy\\projects\\Game Engine\\FTHI\\Resources\\Data\\EngineData\\projects.dat");
    project_list_file.open("../Resources/Data/projects.dat");

    // Read Enable Startup Boolean
    project_list_file.read((char*)&startup_enabled, 1);
    box_startup_enabled.setDataPointer(&startup_enabled);

    // If Startup is Enabled, Read Rest of File and Allow User to Make a Selection
    if (startup_enabled)
    {
        project_list_file.close();
        select_project();
    }

    // If Startup is Not Enabled, Open the Most Recent Project
    else
    {
        char dummy_var;
        project_list_file.read(&dummy_var, 1);
        project_list_file.read(&dummy_var, 1);
        std::getline(project_list_file, current_project_name);
        std::getline(project_list_file, current_project_path);
        current_project_index = 0;
        project_list_file.close();
        loadProject();
    }
}

void Editor::ProjectSelector::select_project()
{
    // Read Project File
    ProjectInstance* instances;
    int instance_count;
    readProjectListFile(&instances, instance_count);

    // Generate the Scroll Bar
    project_selection_bar = GUI::VerticalScrollBar(21.0f, 24.0f, 1.0f, 59.0f, instance_count * 10.0f, 0.0f);

    // Disable Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Disable Depth Test
    glDisable(GL_DEPTH_TEST);

    // Bind ScrollBar
    Global::scroll_bar = &project_selection_bar;
    glfwSetScrollCallback(Global::window, Source::Listeners::ScrollBarCallback);

    // If Window is Opened Upon Startup, Perform Initial Rendering
    if (!project_initialized)
    {
        glClearColor(0.45f, 0.45f, 0.45f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    Global::colorShaderStatic.Use();
    glUniform1i(Global::staticLocColor, 1);
    glm::mat4 temp = glm::mat4(1.0f);
    bool looping = true;
    while (!glfwWindowShouldClose(Global::window) && looping)
    {
        // Clear Window
        glClearColor(0.45f, 0.45f, 0.45f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Handle Inputs
        glfwPollEvents();
        float modified_mouse_x = (float)Global::mouseX / Global::zoom_scale;
        float modified_mouse_y = (float)Global::mouseY / Global::zoom_scale;
        master_element.updateElement();

        // Draw Window
        glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(temp));
        glBindVertexArray(windowVAO);
        glDrawArrays(GL_TRIANGLES, 0, 12);
        glBindVertexArray(0);

        // Draw Boxes
        box_search.blitzElement();
        box_open_project.blitzElement();
        box_open_project_directory.blitzElement();
        box_remove_project.blitzElement();
        box_create_project.blitzElement();
        box_add_project.blitzElement();
        box_startup_enabled.blitzElement();

        // Start Scissor Region
        glScissor((GLint)192, (GLint)106, (GLsizei)590, (GLsizei)425);

        // Draw Selected, If Enabled
        if (current_project_index != -1)
        {
            glm::mat4 selected_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 19.0f - 10.0f * current_project_index + project_selection_bar.BarOffset, 0.0f));
            glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(selected_model));
            glBindVertexArray(windowVAO);
            glDrawArrays(GL_TRIANGLES, 18, 6);
            glBindVertexArray(0);
        }

        // Edit Text of Object if a TextBox is Selected
        if (Global::texting)
        {
            // If Left Click, Stop Texting
            if (Global::LeftClick)
            {
                Global::texting = false;
                glfwSetKeyCallback(Global::window, Source::Listeners::KeyCallback);
                Global::text_box->setFalse();
            }
        }

        // Test Mouse on ScrollBar
        else if (!scrolling && project_selection_bar.TestColloisions())
        {
            if (Global::LeftClick)
            {
                scrolling = true;
                scoll_offset = project_selection_bar.BarPosY - modified_mouse_y;
            }
        }

        // Scroll ScrollBar
        else if (scrolling)
        {
            // Deselect ScrollBar if Left Click is Not Held
            if (!Global::LeftClick)
            {
                scrolling = false;
            }

            // Else, Update ScrollBar Position
            else
            {
                project_selection_bar.Scroll(modified_mouse_y + scoll_offset);
            }
        }

        // Mouse is Inside the Boundaries for Selecting a Project
        else if (modified_mouse_x > -63.0f && modified_mouse_x < 20.0f && modified_mouse_y > -35.0f && modified_mouse_y < 24.0f)
        {
            // Distance Between Projects == 10

            int index = (int)floor((24.0f - modified_mouse_y + project_selection_bar.BarOffset) / 10.0f);

            // If Index is Not In Range, Do Nothing
            if (index >= instance_count)
            {
                index = -1;
            }

            // Else, Draw Highlighter and Test Mouse
            else
            {
                // Draw Highlighter
                glm::mat4 highlight_model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 19.0f - 10.0f * index + project_selection_bar.BarOffset, 0.0f));
                glUniformMatrix4fv(Global::modelLocColorStatic, 1, GL_FALSE, glm::value_ptr(highlight_model));
                glBindVertexArray(windowVAO);
                glDrawArrays(GL_TRIANGLES, 12, 6);
                glBindVertexArray(0);

                // Test Mouse Click
                if (Global::LeftClick)
                {
                    // If Current Project Index Equals Current Index, Deselect Index
                    if (current_project_index == index)
                    {
                        current_project_index = -1;
                    }

                    // Else, Set Index
                    else
                    {
                        current_project_index = index;
                    }

                    // Unbind Left Click
                    Global::LeftClick = false;
                }
            }
        }

        // Else, Test Mouse Collisions on Boxes
        else
        {
            // Test Mouse Collisions on Search Box
            if (box_search.updateElement() || box_search.texting) {}

            // Test Mouse Collisions on Create Project Box
            else if (box_create_project.updateElement())
            {
                Global::LeftClick = false;
                if (createProject())
                {
                    looping = false;
                }
            }

            // Test Mouse Collisions on Add Project Box
            else if (box_add_project.updateElement())
            {
                Global::LeftClick = false;
                if (addProject(instances, instance_count))
                {
                    looping = false;
                }
            }

            // Test Mouse Collisions on Startup Enabled Toggle Box
            else if (box_startup_enabled.updateElement()) {}

            // If a Project is Selected, Enable Collisions on Project Specific Boxes
            else if (current_project_index != -1)
            {
                // Test Mouse Collisions on Open Project Box
                if (box_open_project.updateElement())
                {
                    Global::LeftClick = false;
                    current_project_name = instances[current_project_index].name;
                    current_project_path = instances[current_project_index].path;
                    loadProject();
                    recompileProject();
                    looping = false;
                }

                // Test Mouse Collisions on Open Project Directory Box
                else if (box_open_project_directory.updateElement())
                {
                    Global::LeftClick = false;
                    ShellExecute(NULL, "explore", instances[current_project_index].path.c_str(), NULL, NULL, SW_SHOWDEFAULT);
                }

                // Test Mouse Collisions on Remove Project Box
                else if (box_remove_project.updateElement())
                {
                    Global::LeftClick = false;
                    removeProjectFromFile(current_project_index);
                    delete[] instances;
                    readProjectListFile(&instances, instance_count);
                    current_project_index = -1;
                    box_remove_project.setFalse();
                }
            }
        }

        // End Scissor Region
        glScissor(0, 0, (GLsizei)Global::screenWidth, (GLsizei)Global::screenHeight);

        // Draw ScrollBar
        project_selection_bar.blitzElement();

        // Basic Text Rendering
        Global::fontShader.Use();
        Source::Fonts::renderText("Project Selection", -63.0f, 30.0f, 0.2f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true);
        Source::Fonts::renderText("Search:", 7.0f, 30.0f, 0.1f, glm::vec4(0.9f, 0.9f, 0.9f, 0.9f), true);
        Source::Fonts::renderText("Startup Enabled:", 25.0f, -31.0f, 0.08f, glm::vec4(0.9f, 0.9f, 0.9f, 0.9f), true);

        // Draw Project Selection Text
        glScissor((GLint)192, (GLint)106, (GLsizei)590, (GLsizei)425);
        float name_y_start = 20.0f + project_selection_bar.BarOffset;
        float path_y_start = 17.0f + project_selection_bar.BarOffset;
        for (int i = 0; i < instance_count; i++)
        {
            Source::Fonts::renderText(instances[i].name, -61.0, name_y_start - 10.0f * i, 0.08f, glm::vec4(0.9f, 0.9f, 0.9, 1.0f), true);
            Source::Fonts::renderText(instances[i].access_time, 5.0f, name_y_start - 10.0f * i, 0.05f, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f), true);
            Source::Fonts::renderText(instances[i].path, -61.0, path_y_start - 10.0f * i, 0.05f, glm::vec4(0.9f, 0.9f, 0.9, 1.0f), true);
        }
        glScissor(0, 0, (GLsizei)Global::screenWidth, (GLsizei)Global::screenHeight);

        // Draw Boxes Text
        box_search.blitzText();
        box_open_project.blitzText();
        box_open_project_directory.blitzText();
        box_remove_project.blitzText();
        box_create_project.blitzText();
        box_add_project.blitzText();
        box_startup_enabled.blitzText();
        Global::colorShaderStatic.Use();

        // Swap Buffer
        glfwSwapBuffers(Global::window);
        glFinish();
    }

    // Free the Project Instance Data
    delete[] instances;

    // If Not Initialized, Exit Program
    if (!project_initialized)
        exit(1);

    // Unstatic Project
    Global::colorShaderStatic.Use();
    glUniform1i(Global::staticLocColor, 0);

    // Unbind Bind ScrollBar
    glfwSetScrollCallback(Global::window, Source::Listeners::ScrollCallback);
}

void Editor::ProjectSelector::uninitializeProjectSelector()
{
    // If Currently Loading a Project, Free the DLL
    if (project_initialized)
        FreeLibrary(test_dll);
}

Editor::ProjectSelector* Editor::ProjectSelector::get()
{
	return &instance;
}

Editor::ProjectSelector Editor::ProjectSelector::instance;
