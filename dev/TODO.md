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

- func(SPtr<T> & obj) <-- should always use & for SPtr<T> ?

- InlineBuffer memory overrun detect signture
- Allocator

- PmrArray - hold allocator pointer

- Nanite + Subdiv

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

