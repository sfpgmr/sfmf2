#include "stdafx.h"

#define BOOST_ASSIGN_MAX_PARAMS 7
#include <boost/assign.hpp>
#include <boost/assign/ptr_list_of.hpp>
#include <boost/assign/ptr_list_inserter.hpp>

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include "sf_windows_base.h"
#include "exception.h"

// DLLのリンク
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"dwrite.lib")
#pragma comment(lib,"dwmapi.lib")

#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "D3DCompiler.lib" )
#pragma comment( lib, "DirectXTK.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "Shlwapi.lib" ) 
#pragma comment( lib, "DWMApi.lib" )
#pragma comment( lib,"msimg32.lib")
#pragma comment(lib,"dcomp.lib")

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

