- GC
- using Texture = SPtr<OBJ_Texture>
- Type_EnumFlag<>
- rename AxTag::Zero_ -> AxZero ?

- NameId consteval string literal to StrLit + Id

- ECS

- func(SPtr<T> & obj) <-- should always use & for SPtr<T> ?

- using Ref<T> = SPtr<T, NonNullable>
- using WeakRef<T> = WPtr<T>
- using Ref<T> = GCPtr<T>

- InlineBuffer memory overrun detect signture
- Allocator

- PmrArray - hold allocator pointer


==== 2025-12-13 =====
- [done] AX_LOG
- [done] SIMD - Vec, Mat
- [done] UnitTest, can run single case from test_main
- [done] Reflection - MetaType and Rtti
- [done] HeaderTool
- [done] rename AxHeaderTool -> AxCppHeaderTool
- [done] consteval StrLit

