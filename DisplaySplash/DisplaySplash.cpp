// A Library that is Loaded During Engine Startup and Displays a Splash Screen

#ifdef BUILD_DLL
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif

#include <iostream>
#include <wincodec.h>
#include <wincodecsdk.h>
#pragma comment(lib, "WindowsCodecs.lib")

#include "resource.h"

extern "C" EXPORT void __stdcall openSplash();
extern "C" EXPORT bool __stdcall fadeSplash();
extern "C" EXPORT void __stdcall closeSplash();

// The Module Handle of the 
HINSTANCE dll_instance;

// The Window Objects
HWND splash_base, splash;

BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved)
{
	// Get Module Handle Here Becuase Windows was Designed Poorly
	dll_instance = hinstDll;

	return true;
}

// Function to Load and Open the Splash
EXPORT void __stdcall openSplash()
{
	// Initialize COM
	CoInitialize(NULL);

	// Get Device Context
	HDC hdc = GetDC(NULL);
	HDC hdc_compatable = CreateCompatibleDC(hdc);

	// Create a Stream for the Image
	HRSRC hrsrc = FindResourceA(dll_instance, MAKEINTRESOURCEA(IDB_PNG1), "PNG");
	DWORD image_size = SizeofResource(dll_instance, hrsrc);
	HGLOBAL splash_image = LoadResource(dll_instance, hrsrc);
	LPVOID image_source_ptr = LockResource(splash_image);
	HGLOBAL image_data = GlobalAlloc(GMEM_MOVEABLE, image_size);
	LPVOID image_data_ptr = GlobalLock(image_data);
	CopyMemory(image_data_ptr, image_source_ptr, image_size);
	GlobalUnlock(image_data);
	IStream* image_stream = nullptr;
	CreateStreamOnHGlobal(image_data, TRUE, &image_stream);

	// Get Image
	IWICBitmapSource* bitmap_source = nullptr;
	IWICBitmapDecoder* bitmap_decoder = nullptr;
	CoCreateInstance(CLSID_WICPngDecoder, NULL, CLSCTX_INPROC_SERVER, __uuidof(bitmap_decoder), reinterpret_cast<void**>(&bitmap_decoder));
	bitmap_decoder->Initialize(image_stream, WICDecodeMetadataCacheOnLoad);
	IWICBitmapFrameDecode* bitmap_frame = NULL;
	bitmap_decoder->GetFrame(0, &bitmap_frame);
	WICConvertBitmapSource(GUID_WICPixelFormat32bppBGRA, bitmap_frame, &bitmap_source);

	// Get Bitmap Info
	UINT bitmap_width, bitmap_height;
	bitmap_source->GetSize(&bitmap_width, &bitmap_height);
	BITMAPINFO bitmap_info;
	ZeroMemory(&bitmap_info, sizeof(bitmap_info));
	bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmap_info.bmiHeader.biWidth = bitmap_width;
	bitmap_info.bmiHeader.biHeight = -((LONG)bitmap_height);
	bitmap_info.bmiHeader.biPlanes = 1;
	bitmap_info.bmiHeader.biBitCount = 32;
	bitmap_info.bmiHeader.biCompression = BI_RGB;

	// Allocate a DIB
	HBITMAP bitmap_dib = NULL;
	void* pvImageBits = NULL;
	bitmap_dib = CreateDIBSection(hdc, &bitmap_info, DIB_RGB_COLORS, &pvImageBits, NULL, 0);
	const UINT cbStride = bitmap_width * 4;
	const UINT cbBufferSize = cbStride * bitmap_height;
	bitmap_source->CopyPixels(NULL, cbStride, cbBufferSize, static_cast<BYTE*>(pvImageBits));

	// Free Several Objects Related to Image Loading
	bitmap_frame->Release();
	bitmap_source->Release();
	image_stream->Release();

	// Generate the Splash Window Class
	WNDCLASS window_class = { 0 };
	window_class.lpfnWndProc = DefWindowProc;
	window_class.hInstance = dll_instance;
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	window_class.lpszClassName = L"SplashWindow";
	RegisterClass(&window_class);

	// Generate the Window Base
	splash_base = CreateWindow(window_class.lpszClassName, NULL, WS_POPUP, NULL, NULL, NULL, NULL, NULL, NULL, dll_instance, NULL);

	// Generate the Window Object
	splash = CreateWindowEx(WS_EX_LAYERED, window_class.lpszClassName, NULL, WS_POPUP | WS_VISIBLE, NULL, NULL, NULL, NULL, splash_base, NULL, dll_instance, NULL);

	// Get Size and Image of Splash
	POINT point_zero = { 0 };
	BITMAP bitmap;
	GetObject(bitmap_dib, sizeof(bitmap), &bitmap);
	SIZE splash_size = { bitmap.bmWidth, bitmap.bmHeight };

	// Get Location to Display Splash
	MONITORINFO monitor_info = { 0 };
	monitor_info.cbSize = sizeof(monitor_info);
	GetMonitorInfo(MonitorFromPoint(point_zero, MONITOR_DEFAULTTOPRIMARY), &monitor_info);
	POINT splash_pos = { 0 };
	splash_pos.x = monitor_info.rcWork.left + (monitor_info.rcWork.right - monitor_info.rcWork.left - splash_size.cx / 2) / 2;
	splash_pos.y = monitor_info.rcWork.left + (monitor_info.rcWork.bottom - monitor_info.rcWork.top - splash_size.cy / 2) / 2;

	// Set Blend Function
	BLENDFUNCTION blend = { 0 };
	blend.BlendOp = AC_SRC_OVER;
	blend.SourceConstantAlpha = 255;
	blend.AlphaFormat = AC_SRC_ALPHA;

	// Attatch Bitmap
	HBITMAP old_dib = (HBITMAP)SelectObject(hdc_compatable, bitmap_dib);

	// Update the Splash Screen
	UpdateLayeredWindow(splash, hdc, &splash_pos, &splash_size, hdc_compatable, &point_zero, NULL, &blend, ULW_ALPHA);

	// Free More Variables
	SelectObject(hdc_compatable, old_dib);
	DeleteDC(hdc_compatable);
	ReleaseDC(NULL, hdc);
}

// Function to Allow the Splash Screen to Fade Away
EXPORT bool __stdcall fadeSplash()
{
	static BYTE alpha = 255;
	alpha--;
	SetLayeredWindowAttributes(splash, NULL, alpha, LWA_ALPHA | LWA_COLORKEY);
	return alpha > 80;
}

// Function to Close the Splash
EXPORT void __stdcall closeSplash()
{
	DestroyWindow(splash);
	DestroyWindow(splash_base);
}