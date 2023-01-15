#include <msclr\auto_gcroot.h>
#include <msclr\marshal_cppstd.h>
#include <string>

#ifdef _DEBUG
#define EXPORT __declspec( dllexport )
#else
#define EXPORT __declspec( dllimport )
#endif

using namespace System;
using namespace DebugVS;

extern "C" EXPORT void __stdcall echo();
extern "C" EXPORT void __stdcall openVisualStudioFile(std::string solution, std::string file);
extern "C" EXPORT void __stdcall openVisualStudioFileAtPoint(std::string solution, std::string file, int row, int column);
extern "C" EXPORT void __stdcall attachProcess(std::string solution, std::string symbols, int coreID);
extern "C" EXPORT void __stdcall removeProcess(std::string solution);

public ref class Managed
{
public:

	// Handle to the .NET DLL
	Program^ program = gcnew Program();

	// Instance of Ref Class
	static Managed^ managed = gcnew Managed();

	// Instance of Marshal Context
	msclr::interop::marshal_context context;

	void echo()
	{
		program->echo();
	}

	void attachProcess(std::string& solution, std::string symbols, int coreID)
	{
		program->attachProcess(context.marshal_as<String^>(solution), context.marshal_as<String^>(symbols), coreID);
	}

	void removeProcess(std::string& solution)
	{
		program->removeProcess(context.marshal_as<String^>(solution));
	}

	void openVisualStudioFile(std::string& solution, std::string& file)
	{
		program->openVisualStudioFile(context.marshal_as<String^>(solution), context.marshal_as<String^>(file));
	}

	void openVisualStudioFileAtPoint(std::string& solution, std::string& file, int row, int column)
	{
		program->openVisualStudioFileAtPoint(context.marshal_as<String^>(solution), context.marshal_as<String^>(file), row, column);
	}
};

EXPORT void __stdcall echo()
{
	Managed::managed->echo();
}

EXPORT void __stdcall attachProcess(std::string solution, std::string symbols, int coreID)
{
	Managed::managed->attachProcess(solution, symbols, coreID);
}

EXPORT void __stdcall removeProcess(std::string solution)
{
	Managed::managed->removeProcess(solution);
}

EXPORT void __stdcall openVisualStudioFile(std::string solution, std::string file)
{
	Managed::managed->openVisualStudioFile(solution, file);
}

EXPORT void __stdcall openVisualStudioFileAtPoint(std::string solution, std::string file, int row, int column)
{
	Managed::managed->openVisualStudioFileAtPoint(solution, file, row, column);
}

