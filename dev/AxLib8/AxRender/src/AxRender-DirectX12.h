#pragma once

#if AX_RENDERER_DX12

#if AX_OS_WINDOWS
	#include <d3d12.h>
	#include <dxgi1_6.h>

	#if _DEBUG
		#include <dxgidebug.h>
	#endif

	#pragma comment(lib, "d3d12.lib")
	#pragma comment(lib, "dxgi.lib")
	#pragma comment(lib, "dxguid.lib")

	#include <D3D12MemoryAllocator/D3D12MemAlloc.h>
#endif

#endif
