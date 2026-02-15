module;

export module AxRender:RenderObjectManager;
export import :GpuBuffer;

export namespace ax {



class RenderObjectManager : public RenderObject {
	AX_RTTI_INFO(RenderObjectManager, RenderObject)
public:
	static RenderObjectManager* s_instance();
	
	struct StructBufferPools {
		StructuredGpuBufferPool_<AxGpuData_MeshletGroup>	axGpuData_MeshletGroup;
		StructuredGpuBufferPool_<AxGpuData_MeshletCluster>	axGpuData_MeshletCluster;
		StructuredGpuBufferPool_<AxGpuData_MeshletVert>		axGpuData_MeshletVert;
		StructuredGpuBufferPool_<AxGpuData_MeshletPrim>		axGpuData_MeshletPrim;

		StructuredGpuBufferPool_<AxGpuData_TileLighting>	axGpuData_TileLighting;

		template<class FUNC>
		void visitPools(FUNC func) {
			func(axGpuData_MeshletGroup);
			func(axGpuData_MeshletCluster);
			func(axGpuData_MeshletVert);
			func(axGpuData_MeshletPrim);
			func(axGpuData_TileLighting);
		}		
	} _structBufferPools;	
	
};

} // namespace 
