#pragma once

#if AX_RENDERER_DX12

#define D3D12MA_USING_DIRECTX_HEADERS 1

#if AX_OS_WINDOWS
	#if D3D12MA_USING_DIRECTX_HEADERS
		#include <directx/d3dx12.h>
		#include <dxguids/dxguids.h>

	#else
		#include <d3d12.h>
	#endif

	#include <dxgi1_6.h>

	#if _DEBUG
		#include <dxgidebug.h>
	#endif

	#pragma comment(lib, "d3d12.lib")
	#pragma comment(lib, "dxgi.lib")
	#pragma comment(lib, "dxguid.lib")

	#include <D3D12MemAlloc.h>
#endif

#endif
