module;

export module AxRender:AX_Vulkan;

#if AX_RENDERER_VK
export import :AX_VkUtil;

export namespace ax /*::AxRender*/ {

class AX_VkInstance;
enum class AX_VkQueueFamilyIndex : u32 { Invalid = UINT32_MAX };

class AX_VkExtProcList : public NonCopyable {
public:
	static void s_create(VkInstance inst);
	static void s_destroy();

	static AX_VkExtProcList* s_instance();

	#define AX_MACRO_OP(E) \
		PFN_##E E = nullptr; \
	//------
		AX_VkExtProcList_ENUM_LIST(AX_MACRO_OP)
	#undef AX_MACRO_OP

private:
	AX_VkExtProcList(VkInstance inst);

	VkInstance _vkInst = VK_NULL_HANDLE;

	template<class FUNC>
	void _getExtProc(FUNC& outFunc, StrLit name);
};

class AX_VkPhysicalDevice : public NonCopyable {
public:
	const VkPhysicalDevice& handle()	{ return _phyDev; }
	operator const VkPhysicalDevice&()	{ return _phyDev; }

	void create(Int index, VkInstance inst, VkPhysicalDevice phyDev);

	bool hasExtension(const char* name);

	void logInfo(bool extensions) const;

	bool findQueueFamilyIndex		(AX_VkQueueFamilyIndex& outIndex, VkQueueFlags hasFlags) const;
	bool findGraphicFamilIndex		(AX_VkQueueFamilyIndex& outIndex) const;
	bool findComputeQueueFamilyIndex(AX_VkQueueFamilyIndex& outIndex) const;

	bool findMemoryTypeIndex		(AX_VkQueueFamilyIndex& outIndex, VkFlags typeBits, VkMemoryPropertyFlags requireMask) const;

	VkInstance inst() { return _inst; }
	operator VkInstance() { return inst(); }

	StrView deviceName() const { return StrView_c_str(_props.deviceName); }

	Span<VkQueueFamilyProperties>	queueFamilyProps() const	{ return _queueFamilyProps; }
	Span<VkExtensionProperties>		extensionProps	() const	{ return _extensionProps; }

	const VkPhysicalDeviceProperties&		props	() const	{ return _props; }
	const VkPhysicalDeviceMemoryProperties& memProps() const	{ return _memProps; }

	Int deviceMemorySize() const { return _deviceMemorySize; }

	void getMemoryInfo(RenderMemoryInfo& info);

	struct Features {
		Features();
		VkPhysicalDeviceFeatures2			v10 = {};
		VkPhysicalDeviceVulkan11Features	v11 = {};
		VkPhysicalDeviceVulkan12Features	v12 = {};
		VkPhysicalDeviceVulkan13Features	v13 = {};
	};
	const Features& features() const { return _features; }

private:
	VkPhysicalDevice	_phyDev	= VK_NULL_HANDLE;
	VkInstance			_inst	= VK_NULL_HANDLE;

	Features			_features;

	VkPhysicalDeviceProperties			_props = {};
	VkPhysicalDeviceMemoryProperties	_memProps = {};

	Array<VkExtensionProperties>	_extensionProps;
	Array<VkQueueFamilyProperties>	_queueFamilyProps;

	Int _index = 0;
	Int _majorVersion = 0;
	Int _minorVersion = 0;
	Int _deviceMemorySize = 0;
};

class AX_VkPhysicalDeviceList {
public:
	void create(VkInstance inst);
	Span<AX_VkPhysicalDevice>	devices()		{ return _devices; }
	AX_VkPhysicalDevice*		defaultDevice() { return &_devices[0]; }

private:
	Array<AX_VkPhysicalDevice, 4>	_devices;
};

struct AX_VkApplicationInfo : public VkApplicationInfo {
	AX_VkApplicationInfo();
};

struct AX_VkInstanceCreateInfo : public VkInstanceCreateInfo {
	AX_VkInstanceCreateInfo();

	AX_VkApplicationInfo appInfo;

	struct Props {
		Props();

		bool hasLayer(StrView name) const;
		bool hasExtension(StrView name) const;

		Array<VkLayerProperties,		64>	layers;
		Array<VkExtensionProperties,	64> extensions;
	};
	const Props	props;

	bool enableLayer		(StrLit name);
	bool enableExtension	(StrLit name);

	void logInfo();

friend class AX_VkInstance;
protected:
	Array<const char*,	64> _enabledLayerNames;
	Array<const char*,	64> _enabledExtensionNames;
};

class AX_VkDevice : public NonCopyable  {
public:
	using Features = AX_VkPhysicalDevice::Features;

	VkDevice handle() { return _handle; }
	operator VkDevice() { return _handle; }

	AX_VkPhysicalDevice* physicalDevice() { return _phyDev; }
	operator VkPhysicalDevice() { return _phyDev->handle(); }

	VkInstance inst() { return _phyDev->inst(); }
	operator VkInstance() { return inst(); }

	~AX_VkDevice() { destroy(); }
	void destroy();

	AX_VkDevice& create(AX_VkPhysicalDevice& phyDev);

	VkResult waitIdle() { return vkDeviceWaitIdle(_handle); }

	const Features& enabledFeatures() const { return _enabledFeatures; }

	AX_VkQueueFamilyIndex graphQueueFamilyIndex() const { return _graphQueueFamilyIndex; }

	void getQueue(class AX_VkDeviceQueue& outQueue, AX_VkQueueFamilyIndex queueFamilyIndex, u32 queueIndex);
	void getGraphQueue(class AX_VkDeviceQueue& outQueue, u32 queueIndex) { getQueue(outQueue, _graphQueueFamilyIndex, queueIndex); }

	template<class OBJ>
	void setObjectDebugName(OBJ& objectHandle, StrView name) {
		auto t = AX_VkUtil::s_objectType<OBJ>();
		_setObjectDebugName(t, objectHandle, name);
	}

	template<class OBJ>
	void setObjectDebugTag(OBJ& objectHandle, u64 tagName, ByteSpan tag) {
		auto t = AX_VkUtil::s_objectType<OBJ>();
		_setObjectDebugTag(t, objectHandle, tagName, tag);
	}

#if AX_DEBUG_NAME
	void setDebugName(const String& name) { setObjectDebugName(_handle, name); }
#endif

private:
	bool _addEnabledExtensions(IArray<const char*>& arr, const char* name);
	void _setObjectDebugName(VkObjectType objectType, void* objectHandle, StrView name);
	void _setObjectDebugTag( VkObjectType objectType, void* objectHandle, u64 tagName, ByteSpan tag);


	VkDevice					_handle = VK_NULL_HANDLE;
	AX_VkPhysicalDevice*		_phyDev = nullptr;

	Features	_enabledFeatures = {};

	AX_VkQueueFamilyIndex		_graphQueueFamilyIndex = AX_VkQueueFamilyIndex::Invalid;
};

class AX_VkInstance : public NonCopyable {
public:
	const VkInstance& handle()		{ return _instance; }
	operator const VkInstance&()	{ return _instance; }

	~AX_VkInstance() { destroy(); }
	void destroy();

	void create(AX_VkInstanceCreateInfo& info);

private:
	VkInstance	_instance = VK_NULL_HANDLE;
};

class AX_VkDeviceQueue : public NonCopyable {
public:
	const VkQueue& handle() { return _handle; }
	operator const VkQueue&() { return _handle; }

	struct WaitSemaphores {
		WaitSemaphores() = default;
		WaitSemaphores(VkSemaphore sem, VkPipelineStageFlags dstStage) { add(sem, dstStage); }

		void add(VkSemaphore sem, VkPipelineStageFlags dstStage) {
			_semaphores.emplaceBack(sem);
			_dstStageFlags.emplaceBack(dstStage);
		}

		Span<VkSemaphore>			semaphores() const	  { return _semaphores; }
		Span<VkPipelineStageFlags>	dstStageFlags() const { return _dstStageFlags; }

	private:
		static constexpr Int N = 16;
		Array<VkSemaphore,			N>	_semaphores;
		Array<VkPipelineStageFlags, N>	_dstStageFlags;
	};

	void submit(	const WaitSemaphores&		waitSemaphores,
					Span<VkCommandBuffer>		commandBuffers,
					Span<VkSemaphore>			signalSemaphores,
					VkFence						fenceToSignal = VK_NULL_HANDLE);

	void submit(	Span<VkSubmitInfo> infos, VkFence fenceToSignal);

	void present(	Span<VkSemaphore>		waitSemaphores,
					Span<VkSwapchainKHR>	swapchains,
					Span<u32>				imageIndexInSwapchains) { return _present(waitSemaphores, swapchains, imageIndexInSwapchains); }

	void present(	VkSemaphore				waitSemaphore,
					VkSwapchainKHR			swapchain,
					Span<Int>				imageIndexInSwapchain);

#if AX_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif

friend class AX_VkDevice;
protected:
	void _present(	Span<VkSemaphore>		waitSemaphores,
					Span<VkSwapchainKHR>	swapchains,
					Span<u32>				imageIndexInSwapchains);

	
	VkQueue		_handle = VK_NULL_HANDLE;
	AX_VkDevice*	_dev = nullptr;
};

class AX_VkSurfaceKHR : public NonCopyable {
public:
	const VkSurfaceKHR& handle() { return _surface; }
	operator const VkSurfaceKHR&() { return _surface; }

	~AX_VkSurfaceKHR() { destroy(); }
	void destroy();

#if AX_NATIVE_UI_WIN32
	void create_Win32(AX_VkDevice& dev, HINSTANCE hInstance, HWND hwnd);
#endif

#if AX_NATIVE_UI_X11
	void create_X11(AX_VkDevice& dev, AX_X11_Display* dpy, AX_X11_Window win);
#endif

#if AX_NATIVE_UI_MACOSX
	void create_MacOSX(AX_VkDevice& dev, NSView* view);
//	void create_metal(AX_VkPhysicalDevice& dev, const CAMetalLayer* layer);
#endif

	VkSurfaceCapabilitiesKHR	getCapabilities();

	Span<VkPresentModeKHR>		supportedPresentModes() { return _supportedPresentModes; }
	Span<VkSurfaceFormatKHR>	supportedFormats()		{ return _supportedFormats; }

	bool checkQueueFamilySupportPresent	(AX_VkQueueFamilyIndex queueFamilyIndex);
	bool findQueueFamilySupportPresent	(AX_VkQueueFamilyIndex& outIndex);

	bool isSupportPresentMode	(VkPresentModeKHR presentMode);
	bool isSupportFormat		(const VkSurfaceFormatKHR& f) { return isSupportFormat(f.format, f.colorSpace); }
	bool isSupportFormat		(VkFormat format, VkColorSpaceKHR colorSpace);

	AX_VkQueueFamilyIndex	 graphQueueFamilyIndex	() const { return _graphQueueFamilyIndex; }
	AX_VkQueueFamilyIndex	 presentQueueFamilyIndex() const { return _presentQueueFamilyIndex; }

	void getGraphQueue(  AX_VkDeviceQueue& outQueue, u32 queueIndex) { _dev->getQueue(outQueue, _graphQueueFamilyIndex,   queueIndex); }
	void getPresentQueue(AX_VkDeviceQueue& outQueue, u32 queueIndex) { _dev->getQueue(outQueue, _presentQueueFamilyIndex, queueIndex); }

	bool	isPresentQueueIsSeparated() const { return _graphQueueFamilyIndex != _presentQueueFamilyIndex; }

private:
	void _onCreate();

	VkSurfaceKHR					_surface = VK_NULL_HANDLE;
	AX_VkDevice*					_dev  = nullptr;
	Array<VkSurfaceFormatKHR, 32>	_supportedFormats;
	Array<VkPresentModeKHR,	8 >		_supportedPresentModes;

	AX_VkQueueFamilyIndex _graphQueueFamilyIndex	= AX_VkQueueFamilyIndex::Invalid;
	AX_VkQueueFamilyIndex _presentQueueFamilyIndex	= AX_VkQueueFamilyIndex::Invalid;
};

class AX_VkRenderPass : public NonCopyable {
public:

	const VkRenderPass& handle() { return _handle; }
	operator const VkRenderPass&() { return _handle; }

	AX_VkRenderPass() = default;
	~AX_VkRenderPass() { destroy(); }

	void destroy();

	AX_VkRenderPass& create(AX_VkDevice& dev, VkRenderPassCreateInfo& renderPassCreateInfo);
	AX_VkRenderPass& create(AX_VkDevice& dev, VkFormat colorFormat, VkFormat depthStencilFormat, bool clear);

#if AX_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif

private:
	VkRenderPass	_handle = VK_NULL_HANDLE;
	AX_VkDevice*	_dev;
};

class AX_VkSemaphore : public NonCopyable {
public:
	const VkSemaphore& handle() { return _handle; }
	operator const VkSemaphore&() { return _handle; }

	AX_VkSemaphore() = default;
	~AX_VkSemaphore() { destroy(); }

	AX_VkSemaphore& create(AX_VkDevice& dev);
	void destroy();

#if AX_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif

private:
	VkSemaphore		_handle = VK_NULL_HANDLE;
	AX_VkDevice*	_dev = nullptr;
};

class AX_VkTimelineSemaphore : public NonCopyable {
public:
	const VkSemaphore& handle() { return _handle; }
	operator const VkSemaphore&() { return _handle; }

	AX_VkTimelineSemaphore() = default;
	~AX_VkTimelineSemaphore() { destroy(); }

	AX_VkTimelineSemaphore& create(AX_VkDevice& dev, u64 initialValue);
	void destroy();

	void signal(u64 value);
	bool wait(u64 value, const Nanoseconds& timeout = Nanoseconds::kMax());

#if AX_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif

private:
	VkSemaphore		_handle = VK_NULL_HANDLE;
	AX_VkDevice*	_dev = nullptr;
};

class AX_VkCommandPool : public NonCopyable {
public:
	const VkCommandPool& handle() { return _handle; }
	operator const VkCommandPool&() { return _handle; }

	~AX_VkCommandPool() { destroy(); }

	AX_VkCommandPool& create(AX_VkDevice& dev, AX_VkQueueFamilyIndex queueFamilyIndex);
	void destroy();

	AX_VkDevice*	device() { return _dev; }

#if AX_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif

private:
	VkCommandPool	_handle = VK_NULL_HANDLE;
	AX_VkDevice*	_dev = nullptr;
};

class AX_VkCommandBuffer : public NonCopyable {
public:
	const VkCommandBuffer& handle() { return _handle; }
	operator const VkCommandBuffer&() { return _handle; }

	~AX_VkCommandBuffer() { destroy(); }

	AX_VkCommandBuffer& create(AX_VkCommandPool& cmdPool);

	void destroy();

	void resetAndReleaseResource();

	void beginCommand();
	void endCommand();

	void pipelineBarrier(	AX_VkQueueFamilyIndex srcQueueFamilyIndex,
							AX_VkQueueFamilyIndex dstQueueFamilyIndex,
							VkImage image);

#if AX_DEBUG_NAME
	void setDebugName(const String& name);
#endif

	void debugLabelBegin(const String& name, const Color4f& color = Color4f::kZero());
	void debugLabelEnd();

private:
	VkCommandBuffer _handle = VK_NULL_HANDLE;
	AX_VkCommandPool* _cmdPool = nullptr;
};

class AX_VkImage : public NonCopyable {
public:
	const VkImage& handle() const { return _handle; }
	operator const VkImage&() { return _handle; }

	AX_VkImage() {}
	~AX_VkImage() { destroy(); }

	void destroy();

	AX_VkImage& createImage2D(	AX_VkDevice& dev,
								VkExtent2D frameSize,
								VkFormat format,
								uint32_t mipLevels,
								VkImageUsageFlags usage,
								VkImageLayout initialLayout);

	AX_VkImage& createDepthStencil(AX_VkDevice& dev, VkExtent2D frameSize, VkFormat format = VK_FORMAT_D32_SFLOAT);
	AX_VkImage& createFromBackBuffer(AX_VkDevice& dev, VkImage handle, VkExtent2D frameSize, VkFormat format);

	void setLayout(AX_VkCommandBuffer& cmdBuffer, VkImageLayout newLayout, VkPipelineStageFlags newStage);

	VkFormat	format() { return _format; }

	VkMemoryRequirements getMemoryRequirements();

	VkImageLayout	layout() const { return _layout; }
	AX_VkDevice*	device() { return _dev; }

	const VkExtent3D& size() const { return _size; }

#if AX_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif

private:
	void _create(AX_VkDevice& dev, VkImageCreateInfo& info);

	VkImage				 _handle	= VK_NULL_HANDLE;
	AX_VkDevice*		 _dev		= nullptr;
	VkExtent3D			 _size		= {};
	VkFormat			 _format	= VK_FORMAT_UNDEFINED;
	VkImageLayout		 _layout	= VK_IMAGE_LAYOUT_UNDEFINED;
	VkPipelineStageFlags _stage		= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	bool _isBackBuffer = false;
};

class AX_VkImageView : public NonCopyable {
public:
	const VkImageView& handle() { return _handle; }
	operator const VkImageView&() { return _handle; }

	AX_VkImageView() = default;
	~AX_VkImageView() { destroy(); }

	void destroy();

	AX_VkImageView& create(AX_VkImage& image);
	AX_VkImageView& create(AX_VkDevice& dev, VkImage image, VkFormat format);
	AX_VkImageView& create(AX_VkDevice& dev, VkImage image, VkImageViewCreateInfo& info);

	VkFormat	format() const { return _format; }

#if AX_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif

private:
	VkImageView		_handle = VK_NULL_HANDLE;
	AX_VkDevice*	_dev = nullptr;
	VkFormat		_format = VK_FORMAT_UNDEFINED;
};

class AX_VkSampler : public NonCopyable {
public:
	const VkSampler& handle() const { return _handle; }
	operator const VkSampler&() const { return _handle; }

	AX_VkSampler() = default;
	~AX_VkSampler() { destroy(); }

	void destroy();
	void create(AX_VkDevice& dev, const VkSamplerCreateInfo& info);

#if AX_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif

private:
	VkSampler _handle = VK_NULL_HANDLE;
	AX_VkDevice* _dev = nullptr;
};

class AX_VkSwapchainKHR : public NonCopyable {
public:
	const VkSwapchainKHR& handle() { return _handle; }
	operator const VkSwapchainKHR&() { return _handle; }

	AX_VkSwapchainKHR() = default;
	~AX_VkSwapchainKHR() { destroy(); }

	void destroy();
	void create(AX_VkDevice& dev,
				AX_VkSurfaceKHR&	surface,
				const VkSurfaceFormatKHR& format,
				Int imageCount_,
				VkPresentModeKHR presentMode,
				AX_VkSwapchainKHR*	oldSwapchain = nullptr);

	void getImages(IArray<VkImage>& outImages);

	VkResult acquireNextImage(	u32& outImageIndex, 
								VkSemaphore semaphoreToSignal, 
								VkFence fenceToSignal, 
								Nanoseconds timeout = Nanoseconds::kMax());

private:
	VkSwapchainKHR _handle = nullptr;
	AX_VkDevice* _dev = nullptr;
};

class AX_VkFramebuffer : public NonCopyable {
public:
	const VkFramebuffer& handle() { return _handle; }
	operator const VkFramebuffer&() { return _handle; }

	AX_VkFramebuffer() = default;
	~AX_VkFramebuffer() { destroy(); }

	void destroy();
	AX_VkFramebuffer& create(AX_VkDevice& dev, VkRenderPass renderPass, Span<VkImageView> attachments, VkExtent2D frameSize);

	const VkExtent2D& frameSize() const { return _frameSize; }

#if AX_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif

private:
	VkFramebuffer	_handle = VK_NULL_HANDLE;
	AX_VkDevice*	_dev = nullptr;
	VkExtent2D		_frameSize = {};
};

class AX_VkBuffer : public NonCopyable {
public:
	const VkBuffer& handle() { return _handle; }
	operator const VkBuffer&() { return _handle; }
	~AX_VkBuffer() { destroy(); }

	AX_VkBuffer() = default;
	AX_VkBuffer(AX_VkBuffer && r) noexcept;

	void destroy();
	void create(AX_VkDevice& dev, VkDeviceSize bufferSize, VkBufferUsageFlags usage);

	VkMemoryRequirements getMemoryRequirements();

	AX_VkDevice*	device() { return _dev; }

#if AX_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif

private:
	VkBuffer			_handle = VK_NULL_HANDLE;
	AX_VkDevice*		_dev = nullptr;
	VkDeviceSize		_bufferSize = 0;
	VkBufferUsageFlags	_usage = 0;
};

class AX_VkDeviceMemory : public NonCopyable {
	using This = AX_VkDeviceMemory;
public:
	const VkDeviceMemory& handle() { return _handle; }
	operator const VkDeviceMemory&() { return _handle; }

	AX_VkDeviceMemory() = default;
	~AX_VkDeviceMemory() { destroy(); }

	void destroy();
	AX_VkDeviceMemory& createForImage(AX_VkImage&  img, VkMemoryPropertyFlags requireMask);
	AX_VkDeviceMemory& createForBuffer(AX_VkBuffer& buf, VkMemoryPropertyFlags requireMask);

	void copyData(ByteSpan data, Int offset = 0, VkMemoryMapFlags flags = 0);

	void		_unmapMemory();
	MutByteSpan _mapMemory(IntRange range, VkMemoryMapFlags flags = 0);
	
	using ScopedMapMemory = ScopedMemFuncProxy0<MutByteSpan, This, &This::_unmapMemory>;
	ScopedMapMemory mapMemory(VkMemoryMapFlags flags = 0) { return mapMemory(IntRange(_bufferSize), flags); }
	ScopedMapMemory mapMemory(IntRange range, VkMemoryMapFlags flags = 0) { return ScopedMapMemory(_mapMemory(range, flags), this); }

	void flushMappedMemoryRanges(Span<IntRange> ranges);
	void InvalidateMappedMemoryRanges(Span<IntRange> ranges);

#if AX_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif

private:
	void _fillVkMappedMemoryRange(IArray<VkMappedMemoryRange>& outArray, Span<IntRange> ranges);

	void _create(AX_VkDevice& dev, Int bufferSize, VkFlags memoryTypeBits, VkMemoryPropertyFlags requireMask);
	void _create(AX_VkDevice& dev, Int bufferSize, AX_VkQueueFamilyIndex memTypeIndex);

	VkDeviceMemory _handle = VK_NULL_HANDLE;
	AX_VkDevice* _dev = nullptr;
	Int _bufferSize = 0;
};
inline void AX_VkDeviceMemory::_unmapMemory() {
	vkUnmapMemory(*_dev, _handle);
}

class AX_VkFence : public NonCopyable {
public:
	const VkFence& handle() { return _handle; }
	operator const VkFence&() { return _handle; }

	AX_VkFence() = default;
	~AX_VkFence() { destroy(); }

	void destroy();

	AX_VkFence& create(AX_VkDevice& dev, bool signaled);

	void reset();
	bool wait(uint64_t nanoseconds = UINT64_MAX);

	bool check(bool doReset);

#if AX_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif

private:
	VkFence	_handle = VK_NULL_HANDLE;
	AX_VkDevice* _dev = nullptr;
};

class AX_VkShaderModule : public NonCopyable {
public:
	const VkShaderModule& handle() { return _handle; }
	operator const VkShaderModule&() { return _handle; }

	AX_VkShaderModule() = default;
	~AX_VkShaderModule() { destroy(); }

	void destroy();
	void create			(AX_VkDevice& dev, ByteSpan byteCode);
	void createFromFile	(AX_VkDevice& dev, StrView  filename);

#if AX_DEBUG_NAME
	void setDebugName(const String& name) { if (_dev) _dev->setObjectDebugName(_handle, name); }
#endif

private:
	VkShaderModule	_handle = VK_NULL_HANDLE;
	AX_VkDevice*	_dev = nullptr;
};

class AX_VkDescriptorSetLayoutBindings : public NonCopyable {
public:
	Array<VkDescriptorSetLayoutBinding, 32> bindings;
	Array<VkDescriptorBindingFlags,     32> bindingFlags;

	VkDescriptorSetLayoutBinding& addBinding(VkDescriptorType		  type,
											 ShaderResourceBindPoint  bindPoint,
											 Int					  descriptorCount,
											 ShaderStageFlags		  stageFlags,
											 VkDescriptorBindingFlags flags);
};

class AX_VkDescriptorSetLayout : public NonCopyable {
public:
	const VkDescriptorSetLayout& handle() const { return _handle; }
	operator const VkDescriptorSetLayout&() const { return _handle; }
	~AX_VkDescriptorSetLayout() { destroy(); }

	AX_VkDescriptorSetLayout() = default;
	AX_VkDescriptorSetLayout(AX_VkDescriptorSetLayout&& r) noexcept;

	void destroy();
	void create(AX_VkDevice& dev, 
				Span<VkDescriptorSetLayoutBinding> bindings,
				Span<VkDescriptorBindingFlags>     bindingFlags,
				VkDescriptorSetLayoutCreateFlags   flags = 0);

	void create(AX_VkDevice& dev,
				AX_VkDescriptorSetLayoutBindings& bindings,
				VkDescriptorSetLayoutCreateFlags flags = 0)
	{
		create(dev, bindings.bindings, bindings.bindingFlags, flags);
	}

private:
	VkDescriptorSetLayout _handle = nullptr;
	AX_VkDevice* _dev = nullptr;
};

inline
void AX_vkUpdateDescriptorSets(VkDevice dev, Span<VkWriteDescriptorSet> writeSpan, Span<VkCopyDescriptorSet> copySpan) {
	vkUpdateDescriptorSets(dev, 
		AX_VkUtil::castUInt32(writeSpan.size()), writeSpan.data(),
		AX_VkUtil::castUInt32( copySpan.size()),  copySpan.data());
}

inline void AX_vkCmdBindDescriptorSets(
	VkCommandBuffer			commandBuffer,
	VkPipelineBindPoint		pipelineBindPoint,
	VkPipelineLayout		layout,
	uint32_t				firstSet, 
	Span<VkDescriptorSet>	descriptorSets,
	Span<uint32_t>			dynamicOffsets)
{
	vkCmdBindDescriptorSets(commandBuffer, pipelineBindPoint,
							layout, firstSet,
							AX_VkUtil::castUInt32(descriptorSets.size()), descriptorSets.data(),
							AX_VkUtil::castUInt32(dynamicOffsets.size()), dynamicOffsets.data());
}

class AX_VkPipelineLayout : public NonCopyable {
public:
	const VkPipelineLayout& handle() const { return _handle; }
	operator const VkPipelineLayout&() const { return _handle; }
	~AX_VkPipelineLayout() { destroy(); }

	AX_VkPipelineLayout() = default;

	void destroy();
	void create(AX_VkDevice& dev, Span<VkDescriptorSetLayout>	descriptorSets);

private:
	VkPipelineLayout _handle = VK_NULL_HANDLE;
	AX_VkDevice* _dev = nullptr;
};

class AX_VkPipelineCache : public NonCopyable {
public:
	const VkPipelineCache& handle() { return _handle; }
	operator const VkPipelineCache&() { return _handle; }

	AX_VkPipelineCache() = default;

	~AX_VkPipelineCache() { destroy(); }

	void destroy();
	void create(AX_VkDevice& dev);

private:
	VkPipelineCache _handle = VK_NULL_HANDLE;
	AX_VkDevice*	_dev = nullptr;
};

class AX_VkPipeline : public NonCopyable {
public:
	const VkPipeline& handle() { return _handle; }
	operator const VkPipeline&() { return _handle; }

	AX_VkPipeline() = default;
	~AX_VkPipeline() { destroy(); }

	void destroy();
	void create(AX_VkDevice& dev, VkPipelineCache cache, const VkGraphicsPipelineCreateInfo& info);
	void create(AX_VkDevice& dev, VkPipelineCache cache, const VkComputePipelineCreateInfo&  info);

private:
	VkPipeline	 _handle = VK_NULL_HANDLE;
	AX_VkDevice* _dev = nullptr;
};

class AX_VkDebugReportCallbackEXT : public NonCopyable {
public:
	~AX_VkDebugReportCallbackEXT() { destroy(); }

	void create(VkInstance inst, PFN_vkDebugReportCallbackEXT callback);
	void destroy();

private:
	VkInstance					_inst	= VK_NULL_HANDLE;
	VkDebugReportCallbackEXT	_handle	= VK_NULL_HANDLE;
};

class AX_VkDescriptorPool : public NonCopyable {
public:
	const VkDescriptorPool& handle() { return _handle; }
	operator const VkDescriptorPool&() { return _handle; }
	~AX_VkDescriptorPool() { destroy(); }

	void destroy();
	void create(AX_VkDevice& dev, Span<VkDescriptorPoolSize> poolSizes, Int maxSets, VkDescriptorPoolCreateFlags flags);

	VkDescriptorSet allocDescriptorSet(VkDescriptorSetLayout descSetLayout);

	void resetPool() {
		vkResetDescriptorPool(*_dev, _handle, 0);
	}

	AX_VkDevice& device() { return *_dev; }

private:
	VkDescriptorPool  _handle = VK_NULL_HANDLE;
	AX_VkDevice* _dev = nullptr;
};

} // namespace ax /*::AxRender*/

#endif // AX_RENDERER_VK