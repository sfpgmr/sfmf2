﻿// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//


#pragma once
#include <SDKDDKVer.h>

#ifndef UNICODE
#define UNICODE
#endif
// STL

#define DIRECTINPUT_VERSION 0x0800
//#define BOOST_ALL_NO_LIB

#include <stdint.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <exception>
#include <memory>
#include <string>
#include <map>
#include <locale>
#include <fstream>
#include <algorithm>
#include <functional>
#include <thread>

#define _USE_MATH_DEFINES
#include "math.h"

// Boost
#include <boost/archive/xml_woarchive.hpp>
#include <boost/archive/xml_wiarchive.hpp>
//#include <boost/archive/text_woarchive.hpp>
//#include <boost/archive/text_wiarchive.hpp>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
//#include <boost/archive/binary_woarchive.hpp>
//#include <boost/archive/binary_wiarchive.hpp>
//#include <boost/archive/binary_oarchive.hpp>
//#include <boost/archive/binary_iarchive.hpp>

#include <boost/serialization/export.hpp>
//#include <boost/serialization/is_abstract.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/void_cast.hpp>

#include <boost/scoped_ptr.hpp> 
#include <boost/scoped_array.hpp> 
#include <boost/shared_ptr.hpp> 
#include <boost/shared_array.hpp> 
#include <boost/intrusive_ptr.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>
//#include <boost/thread.hpp>
#include <boost/optional.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/variant.hpp>
#include <boost/any.hpp>
#include <boost/signals2.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/ptr_container/ptr_container.hpp>
//#include <boost/thread/condition.hpp>
//#include <boost/serialization/ptr_vector.hpp>
#include <boost/ptr_container/ptr_array.hpp>
#include <boost/ptr_container/serialize_ptr_container.hpp>
//#include "serialization.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/fstream.hpp>
#include "sf_com.h"
//#include <d3d10_1.h>
//#include <d3d10.h>
//#include <d3d11.h>
//#include <d3dx10.h>
//#include <d3dx11.h>
#include "dinput.h"
#include <wincodec.h>
#include <windows.h>
#include <windowsx.h>
#include <wincodec.h>
#include <wincodecsdk.h>

#include <comdef.h>
#include "avrt.h"
#include "mmsystem.h"

// Direct Input

#include "dinput.h"


// DXGI

#include "dxgi.h"
#include <dxgi1_2.h>
#include <dxgidebug.h>
#include "d3d11_2.h"
#include <d3d11shader.h>
#include <DDSTextureLoader.h>
//#include "d3dx11.h"
//#include <d3dx11effect.h>
//#include <d3dxGlobal.h>
#include <d3dcompiler.h>
#include "DirectXMath.h"
#include <d3dcsx.h>
#include <d3d11shadertracing.h>
#include <SimpleMath.h>
#include <VertexTypes.h>
#include <GeometricPrimitive.h>
#include <CommonStates.h>
#include <Effects.h>
#include <Model.h>

// Direct2D

#include <d2d1_2.h>
#include <d2d1helper.h>
#include <d2d1effects_1.h>
#include <d2d1effecthelpers.h>

// Direct Write

#include <dwrite.h>
#include <dwrite_2.h>

// DWM

#include "dwmapi.h" 
#include "Shobjidl.h"

// DirectComposition

#include <dcomp.h>

#include <wincodec.h>
#include <wincodecsdk.h>

// Media Foundation

#pragma warning(disable:4996)
#include <mmreg.h>
#include <mfapi.h>
#include <mfidl.h>

#include <evr.h>
//#include <evr9.h>

#include <Mfreadwrite.h>
#include <mferror.h>
#include <Mfsharingengine.h>
#include <Wmcodecdsp.h>
#include <Mfmediacapture.h>
#include <Wmcontainer.h>
#include <Mfcaptureengine.h>
#include <Mfmp2dlna.h>
#include "Codecapi.h"
#include "Strmif.h"
#include "Propsys.h"


// DirectX TK

#include "SimpleMath.h"

#include "exception.h"
//#include "singleton.h"
//#include "code_converter.h"
//#include "logger.h"


// TODO: プログラムに必要な追加ヘッダーをここで参照してください。
#include "code_converter.h"
#include "logger.h"
#include "debugout.h"
//#include "dxerr.h"
#include <wrl.h>
#using <windows.winmd>
#using <platform.winmd>
#include <concrt.h>
#include <ppl.h>
#include <ppltasks.h>
#include <agents.h>
#define XBYAK64
#include "xbyak/xbyak.h"
#include "xbyak/xbyak_util.h"
#include <immintrin.h>

//#define _WRL_PTR_TYPEDEF(x) typedef Microsoft::WRL::ComPtr<x> x ## Ptr
#include "sfhelper.h"
#include "sfmf.h"
#include <chrono>

#include "app_base_directory.h"

// 


//#include <Box2D.h>
//_WRL_PTR_TYPEDEF();
//_WRL_PTR_TYPEDEF();
