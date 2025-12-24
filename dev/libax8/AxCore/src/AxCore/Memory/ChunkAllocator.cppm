module;

export module AxCore.ChunkAllocator;

#if 0
export import AxCore.Span;
export import AxCore.MemAllocator;

export namespace ax {

struct ChunkAllocator_Pool {
	ChunkAllocator_Pool* next = nullptr;
	MutByteSpan	data;
};

class ChunkAllocator : public MemAllocator {
	using Base = MemAllocator;
	using Pool = ChunkAllocator_Pool;
public:
	ChunkAllocator(MemAllocator* parent = ax_default_allocator()) : Base(parent) {}
	virtual ~ChunkAllocator() override;

	void init(Int chunkSize, Int alignment = AX_SIZEOF_POINTER * 2);
	void clearAndFree();

	virtual MemAllocResult<u8> onAlloc(const MemAllocRequest& req) override;
	virtual void onDealloc(void* data) override;

private:
	AX_INLINE Int	_poolHeaderAlignedSize() const	{ return Math::alignTo(AX_SIZEOF(Pool), _alignment); }
	AX_INLINE Int	_blockSize() const				{ return Math::alignTo(_chunkSize, _alignment); }
	
	struct Chunk {
		Chunk*	next = nullptr;
	};

	MemAllocResult<u8> _getChunk(MemAllocRequest& req);

	Pool*	_findPoolContainsPointer(void* p_);

	void	_getNewPool();
	void	_addToPools(Pool* p);
	void	_addToFreeChunk(Chunk* p);
	Int		_createNewPool(Byte* buf, Int bufSize);


	Int		_chunkSize	= 0;
	Int		_alignment  = AX_SIZEOF_POINTER;

	Chunk*	_freeChunks = nullptr;
	Pool*	_pools		= nullptr;

	Int		_nextNewPoolChunkCount = 0;
	Int		_freeChunkCount = 0;
	Int		_totalChunkCount = 0;
};

} // namespace
#endif
