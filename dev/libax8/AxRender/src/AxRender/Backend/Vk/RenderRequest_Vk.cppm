module;

export module AxRender:RenderRequest_Vk;

#if AX_RENDERER_VK
export import :CommandBuffer_Vk;
export import :RenderContext_Vk;
export import :RenderRequest_Bindless_Vk;
export import :RenderSystem_Vk;
export import :RenderPass_Vk;

export namespace ax /*::AxRender*/ {

class RenderRequest_Vk : public RenderRequest_Backend {
	AX_RTTI_INFO(RenderRequest_Vk, RenderRequest_Backend)
public:
	using BindPoint = ShaderParamBindPoint;
	
	RenderRequest_Vk(const CreateDesc& desc);

	RenderContext_Vk*	renderContext_vk()		{ return rttiCastCheck<RenderContext_Vk>(_renderContext); }
	RenderPass_Vk*		currentRenderPass_vk()	{ return rttiCastCheck<RenderPass_Vk   >(_currentRenderPass); }
	CommandBuffer_Vk&	uploadCmdBuf_vk()		{ return _uploadCmdBuf_vk; }
	CommandBuffer_Vk&	graphCmdBuf_vk()		{ return _graphCmdBuf_vk; }

	void _updatedBindlessResources();
	
	RenderSystem_Vk*	renderSystem_vk() { return rttiCastCheck<RenderSystem_Vk>(_renderSystem); }

	AX_RenderRequest_Backend_FunctionInterfaces(override)

	AX_VkDevice*		_device_vk = nullptr;

	CommandBuffer_Vk	_uploadCmdBuf_vk; // submit earlier than graphCmdBuf
	AX_VkSemaphore		_uploadCmdSem_vk;

	CommandBuffer_Vk	_graphCmdBuf_vk;
	AX_VkSemaphore		_graphCmdSem_vk;
	
	AX_VkSemaphore		_imageAcquiredSemaphore_vk;
	AX_VkFence			_completedFence_vk;

	AX_VkDescriptorPool	_descriptorPool;
	


	struct WriteDescSetHelper;

	struct WriteDescSetScope {
		WriteDescSetScope(WriteDescSetHelper* helper)
			: _helper(helper) {
		}

		void writeToDevice(VkDevice dev) {
			auto writeSpan = _helper->_writeDescriptorSets.span();
			Span<VkCopyDescriptorSet> copySpan;
			vkUpdateDescriptorSets(dev, 
				AX_VkUtil::castUInt32(writeSpan.size()), writeSpan.data(),
				AX_VkUtil::castUInt32( copySpan.size()),  copySpan.data());
		}
		

		void addConstBufferInfo(BindPoint        bindPoint,
		                          VkDescriptorSet& descSet,
		                          u32              arrayElementIndex,
		                          VkBuffer         buffer,
		                          Int              offset,
		                          Int              range
		) {
			if (!buffer) throw Error_Undefined();
			auto* info   = _helper->_linearAllocator.newObject<VkDescriptorBufferInfo>();
			info->buffer = buffer;
			info->offset = AX_VkUtil::castUInt32(offset);
			info->range  = AX_VkUtil::castUInt32(range);
			_add(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, bindPoint, descSet, arrayElementIndex, info, nullptr);
		}

		void addStorageBufferInfo(BindPoint        bindPoint,
		                            VkDescriptorSet& descSet,
		                            u32              arrayElementIndex,
		                            VkBuffer         buffer,
		                            VkDeviceSize     offset,
		                            VkDeviceSize     range
		) {
			if (!buffer) throw Error_Undefined();
			auto* info   = _helper->_linearAllocator.newObject<VkDescriptorBufferInfo>();
			info->buffer = buffer;
			info->offset = offset;
			info->range  = range;
			_add(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, bindPoint, descSet, arrayElementIndex, info, nullptr);
		}

		void addImageInfo(BindPoint        bindPoint,
		                    VkDescriptorSet& descSet,
		                    u32              arrayElementIndex,
		                    VkImageView      imageView,
		                    VkImageLayout    imageLayout
		) {
			if (!imageView) throw Error_Undefined();
			if (!imageLayout) throw Error_Undefined();
			
			auto* info        = _helper->_linearAllocator.newObject<VkDescriptorImageInfo>();
			info->sampler     = nullptr;
			info->imageView   = imageView;
			info->imageLayout = imageLayout;
			_add(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, bindPoint, descSet, arrayElementIndex, nullptr, info);
		}

		void addSamplerInfo(BindPoint        bindPoint,
						   VkDescriptorSet& descSet,
						   u32              arrayElementIndex,
						   VkSampler        sampler
		) {
			if (!sampler) throw Error_Undefined();
			auto* info        = _helper->_linearAllocator.newObject<VkDescriptorImageInfo>();
			info->sampler     = sampler;
			info->imageView   = nullptr;
			info->imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			_add(VK_DESCRIPTOR_TYPE_SAMPLER, bindPoint, descSet, arrayElementIndex, nullptr, info);
		}		

	private:
		auto _add(VkDescriptorType        descType,
		          BindPoint               bindPoint,
		          VkDescriptorSet&        descSet,
		          u32                     arrayElementIndex,
		          VkDescriptorBufferInfo* bufInfo,
		          VkDescriptorImageInfo*  imageInfo
		) {
			auto& wds           = _helper->_writeDescriptorSets.emplaceBack();
			wds.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			wds.pNext           = nullptr;
			wds.dstSet          = descSet;
			wds.dstBinding      = AX_VkUtil::castUInt32(ax_enum_int(bindPoint));
			wds.dstArrayElement = arrayElementIndex;
			wds.descriptorType  = descType;
			wds.descriptorCount = 1;
			wds.pBufferInfo     = bufInfo;
			wds.pImageInfo      = imageInfo;

			if (!wds.dstSet) throw Error_Undefined();
		};

		WriteDescSetHelper* _helper = nullptr;;
	};
	
	struct WriteDescSetHelper {
		AX_NODISCARD WriteDescSetScope scopeStart() {
			_writeDescriptorSets.clear();
			_linearAllocator.reset();
			return this;
		}

		WriteDescSetHelper() {
			_writeDescriptorSets.ensureCapacity(64);
		}

	protected:
		friend struct WriteDescSetScope;
		Array<VkWriteDescriptorSet> _writeDescriptorSets;
		LinearAllocator             _linearAllocator;
	};
	
	WriteDescSetHelper _writeDescSetHelper;
};

} // namespace
#endif // AX_RENDERER_VK