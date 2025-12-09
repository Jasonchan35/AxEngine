module;

#if AX_RENDER_BINDLESS && AX_RENDERER_VK

export module AxRender:RenderRequest_Bindless_VK;
export import :CommandBuffer_VK;
export import :RenderPass_VK;
export import :RenderContext_VK;

export namespace ax::AxRender {

class RenderRequest_VK;

class RenderRequest_Bindless_VK {
public:

	template<class T>
	class Table : public NonCopyable {
	public:
		Table();

		void update(RenderRequest_VK* req);
	private:
		static constexpr bool isSampler		  = std::is_base_of_v<Sampler      , T>;
		static constexpr bool isTexture2D	  = std::is_base_of_v<Texture2D    , T>;
		static constexpr bool isStorageBuffer = std::is_base_of_v<StorageBuffer, T>;

		struct Temp {
			void reset() {
				copySlots.clear();
				copySets.clear();
				bufferInfos.clear();
				imageInfos.clear();
			}

			Array<u32>						copySlots;
			Array<VkCopyDescriptorSet>		copySets;
			Array<VkDescriptorBufferInfo>	bufferInfos;
			Array<VkDescriptorImageInfo>	imageInfos;
		} _temp;

		Array<VkWriteDescriptorSet>		_writeSets;

		Int					_slotLimit		= 0;
		BindPoint			_bindPoint		= BindPoint::Invalid;
		VkDescriptorType	_descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
	};


	void update(RenderRequest_VK* req);
	void getTable(Table<Sampler  >* & outTable) { outTable = &_samplerTable;   }
	void getTable(Table<Texture2D>* & outTable) { outTable = &_texture2DTable; }

	Table<Sampler>		_samplerTable;
	Table<Texture2D>	_texture2DTable;
};

} //namespace

#endif // AX_RENDERER_VK && AX_RENDER_BINDLESS