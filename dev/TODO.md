- Vulkan - Bindless
	- use only one destSet for bindless
	
- Dx12
	- RenderRequest - linear allocate CRV_Heap, Sampler_Heap
	- Material update 
	- move some CBV, Texture to root parameter


- GC
- using Texture = SPtr<OBJ_Texture>
- Type_EnumFlag<>
- rename AxTag::Zero_ -> AxZero ?

- IArrayStorage - init capacity = BUF_SIZE  - SmallExtra
- rename CommandBuffer -> RenderCommandList

- Runtime select bindless
- AxShaderTool - output bindless shader to separate file

- ECS

- func(SPtr<T> & obj) <-- should always use & for SPtr<T> ?

- using Ref<T> = SPtr<T, NonNullable>
- using WeakRef<T> = WPtr<T>
- using Ref<T> = GCPtr<T>

- InlineBuffer memory overrun detect signture
- Allocator

- PmrArray - hold allocator pointer

- Nanite + Subdiv


==== 2025-12-13 =====
- [done] AX_LOG
- [done] SIMD - Vec, Mat
- [done] UnitTest, can run single case from test_main
- [done] Reflection - MetaType and Rtti
- [done] HeaderTool
- [done] rename AxHeaderTool -> AxCppHeaderTool
- [done] consteval StrLit

