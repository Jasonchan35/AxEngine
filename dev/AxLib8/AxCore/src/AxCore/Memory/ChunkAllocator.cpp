module AxCore.ChunkAllocator;

#if 0

namespace ax {

ChunkAllocator::~ChunkAllocator() {
	clearAndFree();
}

void ChunkAllocator::_addToPools(Pool* p) {
	AX_ASSERT(p->next == nullptr);
	AX_ASSERT(p->data.size() > 0);
	p->next = _pools;
	_pools	= p;
}

void ChunkAllocator::_addToFreeChunk(Chunk* p) {
	AX_ASSERT(p->next == nullptr);
	p->next		= _freeChunks;
	_freeChunks = p;
	_freeChunkCount++;
}

Int ChunkAllocator::_createNewPool(Byte* buf, Int bufSize) {
	auto poolSize = _poolHeaderAlignedSize();
	if (bufSize < poolSize) { throw Error_Undefined(); }

	auto* newPool = new (buf) Pool();

	buf += poolSize;
	bufSize -= poolSize;
	newPool->data = MutByteSpan(buf, bufSize);

	_addToPools(newPool);

	Int chunkCount = bufSize / _blockSize();
	for (Int i = 0; i < chunkCount; i++) {
		auto* chunk = new (buf) Chunk();

		_addToFreeChunk(chunk);
		_totalChunkCount++;
		buf += _blockSize();
	}
	
	return chunkCount;
}

void ChunkAllocator::_getNewPool() {
	Int	 newPoolSize = _poolHeaderAlignedSize() + _nextNewPoolChunkCount * _blockSize();
	auto res = Base::allocFromParent(newPoolSize);

	auto chunkCount = _createNewPool(res.peekData(), res.size());
	if (chunkCount <= 0) { throw Error_Undefined(); }

	res.takeOwnership();
	_nextNewPoolChunkCount *= 2;
}

MemAllocResult<u8> ChunkAllocator::_getChunk(MemAllocRequest& req) {
	if (_chunkSize < AX_SIZEOF_POINTER || _alignment < AX_SIZEOF_POINTER) { throw Error_Undefined(); }
	if (req.dataSize > _chunkSize) { throw Error_Undefined(); }

	auto blockSize = _blockSize();
	if (blockSize > _blockSize()) { throw Error_Undefined(); }
	
	if (!_freeChunks) { _getNewPool(); }
	if (!_freeChunks) { throw Error_Undefined(); }

	auto* nextChunk = _freeChunks->next;

	_freeChunkCount--;

	_freeChunks = nextChunk;
	return MemAllocResult<u8>(this, reinterpret_cast<Byte*>(_freeChunks), blockSize);
}

void ChunkAllocator::clearAndFree() {
	if (allocatedCount() > 0) { AX_ASSERT(false); }

	auto* pool = _pools;
	while (pool) {
		auto* next = pool->next;
		ax_delete(pool);
		pool = next;
	}

	_pools		= nullptr;
	_freeChunks = nullptr;

	_freeChunkCount	 = 0;
	_totalChunkCount = 0;
}

void ChunkAllocator::init(Int chunkSize, Int alignment) {
	if (_pools) {
		AX_ASSERT(false);
		// cannot change chunk size after allocated
		throw Error_Undefined();
	}

	if (alignment < AX_SIZEOF_POINTER) {
		AX_ASSERT(false);
		alignment = AX_SIZEOF_POINTER;
	}

	if (chunkSize < AX_SIZEOF_POINTER) {
		AX_ASSERT(false);
		chunkSize = AX_SIZEOF_POINTER;
	}

	_chunkSize = chunkSize;
	_alignment = alignment;

	_nextNewPoolChunkCount = (512 - _poolHeaderAlignedSize()) / _blockSize();

	if (_nextNewPoolChunkCount < 1) { _nextNewPoolChunkCount = 1; }
}

ChunkAllocator::Pool* ChunkAllocator::_findPoolContainsPointer(void* p_) {
	Byte* p = reinterpret_cast<Byte*>(p_);

	auto* blk = _pools;
	while (blk) {
		Byte* s = blk->data.data();
		Byte* e = s + blk->data.size();
		if (Math::isInRange(p, s, e)) return blk;

		blk = blk->next;
	}
	return nullptr;
}

} // namespace
#endif