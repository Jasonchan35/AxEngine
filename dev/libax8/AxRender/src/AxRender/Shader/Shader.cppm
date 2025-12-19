module;
export module AxRender:Shader;
export import :RenderState;
export import :Vertex;
export import :StockObjects;
export import :GpuBuffer;

export namespace ax /*::AxRender*/ {


AX_ENUM_CLASS(AX_ShaderPropType_EnumList, ShaderPropType, u16)

constexpr bool ShaderPropType_isTextureType(ShaderPropType t) {
	return t == ShaderPropType::Texture2D
		|| t == ShaderPropType::Texture3D
		|| t == ShaderPropType::TextureCube;
}

template<class T> struct ShaderPropType_get_;

template<class T> ShaderPropType ShaderPropType_get = ShaderPropType_get_<T>::value;
#define E(T, ...) \
	template<> struct ShaderPropType_get_<T> { static constexpr ShaderPropType value = ShaderPropType::T; };
//----
	AX_ShaderPropType_EnumList_WITHOUT_NONE(E)
#undef E

class ShaderStageInfo : public NonCopyable {
public:
	using BindPoint = ShaderParamBindPoint;
	using BindSpace = ShaderParamBindSpace;
	using BindCount = ShaderParamBindCount;

	struct Input {
		VertexSemantic	semantic = VertexSemantic::None;
		RenderDataType	dataType = RenderDataType::None;

		template<class SE>
		void onJsonIO(SE & se) {
			AX_JSON_IO(se, semantic);
			AX_JSON_IO(se, dataType);
		}
	};

	struct Variable {
		String			name;
		u32				offset   = 0;
		RenderDataType	dataType = RenderDataType::None;
		bool			rowMajor = true;

		constexpr bool operator==(const Variable& r) {
			return offset == r.offset && dataType == r.dataType && rowMajor == r.rowMajor;
		}

		template<class SE>
		void onJsonIO(SE & se) {
			AX_JSON_IO(se, name);
			AX_JSON_IO(se, offset);
			AX_JSON_IO(se, dataType);
			AX_JSON_IO(se, rowMajor);
		}
	};

	struct ParamBase {
		String			name;
		RenderDataType	dataType  = RenderDataType::None;
		BindSpace		bindSpace = BindSpace::Default;
		BindPoint		bindPoint = BindPoint::Invalid;
		BindCount		bindCount = 0;

		ShaderStageFlags stageFlags = ShaderStageFlags::None;

		template<class SE>
		void onJsonIO(SE & se) {
			AX_JSON_IO(se, name);
			AX_JSON_IO(se, dataType);
			AX_JSON_IO(se, stageFlags);
			AX_JSON_IO(se, bindSpace);
			AX_JSON_IO_ENUM_AS_INT(se, bindPoint);

			AX_JSON_IO(se, bindCount);
		}
	};

	struct ConstBuffer : public ParamBase {
		Int				dataSize  = 0;
		Array<Variable>	variables;

		template<class SE>
		void onJsonIO(SE & se) {
			ParamBase::onJsonIO(se);
			AX_JSON_IO(se, dataSize);
			AX_JSON_IO(se, variables);
		}

		const Variable* findVariable(StrView propName) const {
			for (auto& v : variables) {
				if (v.name == propName) return &v;
			}
			return nullptr;
		}
	};

	Array<Input>		inputs;
	Array<ConstBuffer>	constBuffers;

//----------
	struct Texture : public ParamBase {};
	Array<Texture>		textures;

//----------
	struct Sampler : public ParamBase {};
	Array<Sampler>		samplers;

//----------
	struct StorageBuffer : public ParamBase {
		bool	rawUAV = false;

		template<class SE>
		void onJsonIO(SE & se) {
			ParamBase::onJsonIO(se);
			AX_JSON_IO(se, rawUAV);
		}
	};
	Array<StorageBuffer>	storageBuffers;

//----------
	void loadFile(StrView filename_);

	String	filename;
	Vec3i	csWorkgroupSize {1,1,1};
	ShaderStageFlags	stageFlags = ShaderStageFlags::None;

	bool bindless = AxRenderConfig::bindless;

	template<class SE>
	void onJsonIO(SE & se) {
		AX_JSON_IO(se, bindless);

		if constexpr (se.isReader()) {
			if (bindless != AxRenderConfig::bindless) {
				throw Error_Undefined(Fmt("require compile shader with correct AX_RENDER_BINDLESS\n filename={}", filename));
			}
		}

		AX_JSON_IO(se, stageFlags);

		if (ax_bit_has(stageFlags, ShaderStageFlags::Compute)) {
			AX_JSON_IO(se, csWorkgroupSize);
		}

		AX_JSON_IO(se, inputs);
		AX_JSON_IO(se, constBuffers);
		AX_JSON_IO(se, storageBuffers);
		AX_JSON_IO(se, textures);
		AX_JSON_IO(se, samplers);
	}

};

struct ShaderPassInfo : public NonCopyable {
	String	name;

	String	vsFunc;
	String	psFunc;
	String	gsFunc;
	String	csFunc;

	RenderState	renderState;

	const String&	getFuncName(ShaderStageFlags mask) const;

	template<class SE>
	void onJsonIO(SE& se) {
		AX_JSON_IO(se, name);
		AX_JSON_IO(se, vsFunc);
		AX_JSON_IO(se, psFunc);
		AX_JSON_IO(se, gsFunc);
		AX_JSON_IO(se, renderState);
	}
};

struct ShaderPropInfo : public NonCopyable {
	ShaderPropInfo() {
		defaultValue = {};
	}

	ShaderPropType	propType = ShaderPropType::None;

	String	name;
	String	displayName;

	union DefaultValue {
	#define E(T, ...) T v_##T; \
	//----
		AX_ShaderPropType_Numbers_EnumList(E)
	#undef E
		Color3f	v_Color3f;
		Color4f	v_Color4f;
		StockTextureId	v_stockTextureId;
	} defaultValue;

	String			asset;
	SamplerState	samplerState;

	template<class SE>
	void onJsonIO(SE& se) {
		AX_JSON_IO(se, name);
		AX_JSON_IO(se, propType);

		AX_JSON_IO(se, displayName);
		AX_JSON_IO(se, asset);

		switch (propType) {
		#define E(T, ...) \
			case ShaderPropType::T:	se.named_io("defaultValue",  defaultValue.v_##T);	break; \
		//---
			AX_ShaderPropType_Numbers_EnumList(E)
		#undef E
		//---
			case ShaderPropType::Color3f:	se.named_io("defaultValue",  defaultValue.v_Color3f);	break;
			case ShaderPropType::Color4f:	se.named_io("defaultValue",  defaultValue.v_Color4f);	break;

			case ShaderPropType::Texture2D:
			case ShaderPropType::Texture3D:
			case ShaderPropType::TextureCube: {
				se.named_io("defaultValue",  defaultValue.v_stockTextureId);

				AX_JSON_IO(se, samplerState);
			} break;

			default: throw Error_Undefined();
		}
	}
};

class ShaderDeclareInfo : public NonCopyable {
public:
	Array<ShaderPropInfo>	props;
	Array<ShaderPassInfo>	passes;

	ShaderPassInfo*	findPass(StrView name);

	bool isGlobalCommonShader = false;

	template<class SE>
	void onJsonIO(SE& se) {
		AX_JSON_IO(se, isGlobalCommonShader);
		AX_JSON_IO(se, props);
		AX_JSON_IO(se, passes);
	}
};

class ShaderResultInfo : NonCopyable {
public:
	ShaderDeclareInfo		declare;
	Array<ShaderStageInfo>	passStages;

	template<class SE>
	void onJsonIO(SE& se) {
		AX_JSON_IO(se, declare);
		AX_JSON_IO(se, passStages);
	}
};

class ShaderParamSpace_CreateDesc : public NonCopyable {
public:
	using BindSpace = ShaderParamBindSpace;
	BindSpace bindSpace = BindSpace::Invalid;
};

class ShaderParamSpace : public RenderObject {
	AX_RTTI_INFO(ShaderParamSpace, RenderObject)
public:
	using CreateDesc = ShaderParamSpace_CreateDesc;
	using BindPoint  = ShaderParamBindPoint;
	using BindSpace  = ShaderParamBindSpace;
	using BindCount  = ShaderParamBindCount;

	BindSpace	bindSpace() const { return _bindSpace; }

	static SPtr<ShaderParamSpace> s_new(const MemAllocRequest& req, const CreateDesc& desc);


protected:
	ShaderParamSpace(const CreateDesc& desc) : _bindSpace(desc.bindSpace) {}

private:
	BindSpace _bindSpace = BindSpace::Invalid;
};

class Shader_CreateDesc : public NonCopyable {
public:
	StrView		assetPath;
};

class Shader : public RenderObject {
	AX_RTTI_INFO(Shader, RenderObject)
public:
	using BindSpace = ShaderParamBindSpace;
	using CreateDesc = Shader_CreateDesc;
	using ResourceKey = String;

	const ResourceKey& resourceKey() const { return _assetPath; }

	Shader(const CreateDesc& desc);

	static SPtr<This> s_new(const MemAllocRequest& req, const CreateDesc& desc);
	static SPtr<This> s_new(const MemAllocRequest& req, StrView assetPath);

	const String& assetPath() const { return _assetPath; }
protected:

	String		_assetPath;
};

} // namespace 