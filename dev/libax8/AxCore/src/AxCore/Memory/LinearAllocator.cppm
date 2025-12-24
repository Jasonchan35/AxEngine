module;

export module AxCore.LinearAllocator;
export import AxCore.Array;
export import AxCore.UPtr;

export namespace ax {

class LinearAllocator : public NonCopyable {
public:
	void setChunkSize(Int n) { _chunkSize = n; }
	void clearAndFree();
	void reset();

	void* allocBytes(Int reqSize, Int align = 16);

	template<class T, class... ARGS>
	T* newObject(ARGS&&... args) {
		auto* p = allocBytes(AX_SIZEOF(T), AX_ALIGNOF(T));
		return new (p) T(AX_FORWARD(args)...);
	}
	
private:
	struct Chunk : public NonCopyable {
		Chunk(Int bufferSize);
		void* allocate(Int reqSize, Int align);
		void reset() { _used = 0; }
	private:
		Array<u8> _buffer;
		Int _used = 0;
	};

	Array<UPtr<Chunk>>	_chunks;
	Int _chunkSize = 16 * 1024;
};


inline void* LinearAllocator::allocBytes(Int reqSize, Int align) {
	if (reqSize > _chunkSize) throw Error_Undefined();
	
	if (_chunks.size()) {
		auto& t = _chunks.back();
		auto* p = t->allocate(reqSize, align);
		if (p) return p;
	}

	auto chunkSize = reqSize > _chunkSize ? reqSize : _chunkSize;

	auto newChunk = UPtr_new<Chunk>(AX_ALLOC_REQ, chunkSize);
	_chunks.append(std::move(newChunk));

	auto* p = _chunks.back()->allocate(reqSize, align);
	return p;
}

inline void LinearAllocator::clearAndFree() {
	_chunks.clear();
}

inline void LinearAllocator::reset() { for (auto& chunk : _chunks) { chunk->reset(); } }

inline LinearAllocator::Chunk::Chunk(Int bufferSize) { _buffer.resize(bufferSize); }

inline void* LinearAllocator::Chunk::allocate(Int reqSize, Int align) {
	auto offset = Math::alignTo(_used, align);
	auto available = _buffer.size() - offset;

	if (available < reqSize) {
		return nullptr;
	}

	_used = offset + reqSize;
	return _buffer.data() + offset;
}

} // namespace
