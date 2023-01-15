#include <msclr\auto_gcroot.h>

#ifdef _DEBUG
#define EXPORT __declspec( dllexport )
#else
#define EXPORT __declspec( dllimport )
#endif

using namespace System;
using namespace DebugVS;

extern "C" EXPORT void __stdcall test();

public ref class Managed
{
public:

	void test()
	{
		Program^ c = gcnew Program();
		c->test();
	}
};

EXPORT void __stdcall test()
{
	Managed^ managed = gcnew Managed();
	managed->test();
}

