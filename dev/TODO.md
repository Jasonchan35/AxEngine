- Mega vertex buffer

- GPU Indirect Draw
	- compute shader object culling
		- write indirect draw list
		- write PerMeshData (mvp)
		- vertex shader fetch mvp from PerMeshData[instanceId]
		
- Dx12
	- Material update 
	- move some CBV, Texture to root parameter
	- static sampler
	
- DirectStorage texture loading
	- vkCmdCopyMemoryIndirectKHR
	
- Vk
	- immutable sampler

- Memory Allocator, Arena
	- allocate page and paritioning by Arena NameId
	
- Gpu descriptor allocator arena
	- for material static descriptors

- GC

- IArrayStorage - init capacity = BUF_SIZE  - SmallExtra

- Runtime select bindless
- AxShaderTool - output bindless shader to separate file

- ECS

- InlineBuffer memory overrun detect signture
- Allocator

- PmrArray - hold allocator pointer

- Mesh Shader

- Nanite + Subdiv

==== 2025-01-20 =====
- [done] D3D12MA Allocator
- [done] Vulkan Memory Allocator
- [done] Vulkan Sparse Buffer (Mega Vertex/Index Buffer)
- [done] Win32 - SetProcessDpiAwarenessContext
- [done] Draw mesh with mvpMatrix (Vulkan-PushConstant, Dx12-RootConst)
- [done] HLSL StruturedBuffer
- [done] EditorUI InsepectorUIPanel
- [done] EditorUI SceneOutlinerUIPanel
- [done] AxCppHeaderTool
- [done] FreeType Font

==== 2025-12-28 =====
- [done] Type_EnumFlag<>
- [done] rename CommandBuffer -> RenderCommandList
- [done] Dx12
- [done] Dx12 RenderRequest - linear allocate CRV_Heap, Sampler_Heap
- [done] Dx12 One big chunk of Descriptor
- [done] Dx12 bindless
- [done] Vk bindless
- [done] IArray - binary search
- [done] ShaderPass_Backend -  replace ShaderParamDict

==== 2025-12-13 =====
- [done] AX_LOG
- [done] SIMD - Vec, Mat
- [done] UnitTest, can run single case from test_main
- [done] Reflection - MetaType and Rtti
- [done] HeaderTool
- [done] rename AxHeaderTool -> AxCppHeaderTool
- [done] consteval StrLit

