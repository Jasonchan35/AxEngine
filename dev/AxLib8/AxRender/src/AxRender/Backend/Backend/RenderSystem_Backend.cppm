module;

export module AxRender:RenderSystem_Backend;
export import :RenderSystem;

export namespace ax /*::AxRender*/ {

#define AX_RenderObject_ForwardDeclareBackend(OBJ, ...)	\
	class OBJ ## _Backend; \
//---

AX_RenderObject_LIST(AX_RenderObject_ForwardDeclareBackend, AX_EMPTY, AX_EMPTY);

template<class T> class BindlessTable;

class RenderSystem_Backend : public RenderSystem {
	AX_RTTI_INFO(RenderSystem_Backend, RenderSystem)
public:
	AX_DOWNCAST_GET_INSTANCE()

	RenderSystem_Backend(const CreateDesc& desc);
	virtual ~RenderSystem_Backend() override;

	AX_RenderObject_LIST(AX_RenderSystem_NewObject, AX_EMPTY, =0)

	RenderRequest_Backend*		nextRenderRequest();
	
	void waitAllRenderCompleted();

	RenderRequest_Backend* getRenderRequest(Int i);

protected:
	virtual void onCreate() override;
	virtual void onDestroy() override;
	virtual void onFileChanged(FileDirWatcher_Result& result) override;

private:
	struct PrivateData;
	UPtr<PrivateData>	_privateData;
};


} // namespace
