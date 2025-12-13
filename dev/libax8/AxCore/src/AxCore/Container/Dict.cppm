module;


export module AxCore.Dict;
export import AxCore.LinkedList;
export import AxCore.Array;
export import AxCore.SPtr;
export import AxCore.String;

export namespace ax {

class Dict_OrderedListConfig {};
class Dict_HashListConfig {};

template<class KEY, class VALUE>
class DictNode : public SPtrReferenable_NonThreadSafe
	, public LinkedListNode< DictNode<KEY, VALUE>, Dict_OrderedListConfig>
	, public LinkedListNode< DictNode<KEY, VALUE>, Dict_HashListConfig>
{
public:
	using Key   = KEY;
	using Value = VALUE;

	template<class... ARGS>
	DictNode(const Key & key, const HashInt& hash, ARGS&&... args)
		          : _key(key),         _hash(hash),   _value(args...) {}

	const	Key&	key() const		{ return _key; }
	const HashInt&	hash() const	{ return _hash; }

			Value&	value()			{ return _value; }
	const	Value&	value() const	{ return _value; }

protected:
	Key		_key;
	HashInt _hash;
	
public:
	Value	_value;
};

template<class NODE>
class Dict_FindIter {
public:
	using Value = typename NODE::Value;
	
	Dict_FindIter(NODE* p) : _node(p) {}
	AX_NODISCARD AX_INLINE constexpr explicit operator bool() const noexcept { return _node; } 
	
	AX_NODISCARD AX_INLINE constexpr NODE*	node		() noexcept	{ return _node; }
	AX_NODISCARD AX_INLINE constexpr Value&	value		() noexcept	{ return _node->value(); }

	AX_NODISCARD AX_INLINE constexpr Value&	operator*	() noexcept	{ return  value(); }
	AX_NODISCARD AX_INLINE constexpr Value*	operator->	() noexcept	{ return &value(); }
	
	AX_NODISCARD AX_INLINE constexpr bool	operator==	(const Dict_FindIter & rhs) const noexcept { return _node == rhs._node; }
	AX_NODISCARD AX_INLINE constexpr bool	operator!=	(const Dict_FindIter & rhs) const noexcept { return _node != rhs._node; }

	AX_INLINE constexpr void	operator++	()	{
		if (!_node) {
			AX_ASSERT(false); // no more node in the list
			return;
		}
		//find next node have the same key
		auto& curKey = _node->key(); 
		NODE* p = _node;
		while (p) {
			p = p->_listNode.next;
			// no more node
			if (!p) { _node = nullptr; break; }
			// found
			if (p->key() == curKey) { _node = p; break; }
		}
	}
private:
	NODE*	_node = nullptr;
};	

template<class NODE, class IN_KEY>
class Dict_FindEnumerator {
public:
	using FindIter = Dict_FindIter<NODE>;
	
	constexpr Dict_FindEnumerator() = default;
	constexpr Dict_FindEnumerator(const IN_KEY& key, FindIter begin) : _begin(begin), _key(&key) {}

	constexpr operator Dict_FindEnumerator<const NODE, const IN_KEY>() { return {_key, _begin}; }
	
	constexpr explicit operator bool() const { return _begin; }

	constexpr FindIter	begin	() { return _begin;  }
	constexpr FindIter	end		() { return nullptr; }
private:
	FindIter		_begin;
	const IN_KEY*	_key = nullptr;
};

template<class KEY, class VALUE>
struct Dict_Config_ {
	using InKey = KEY;
	static constexpr Int s_tableBufSize = 0;
};

template<class KEY, class VALUE> struct Dict_DefaultConfig : Dict_Config_<KEY, VALUE> {};

// using StrView as key for add() and find(), when the KEY is String  
template<class T, Int N, class VALUE>
struct Dict_DefaultConfig<String_<T,N>, VALUE> : Dict_Config_<String_<T,N>, VALUE> {
	using InKey = StrView_<T>;
};

template<class KEY, class VALUE, class CONFIG = Dict_DefaultConfig<KEY,VALUE>>
class Dict : public NonCopyable {
public:
	using Node			= DictNode<KEY, VALUE>;
	using OrderedList	= SPtrLinkedList<Node, Dict_OrderedListConfig>;
	using HashList		= SPtrLinkedList<Node, Dict_HashListConfig>;

	using Key			= KEY;
	using Value			= VALUE;
	using InKey			= typename CONFIG::InKey;
	using  FindEnumator	= Dict_FindEnumerator<Node, InKey>;
	using CFindEnumator	= Dict_FindEnumerator<const Node, const InKey>;
	using FindIter		= Dict_FindIter<Node>;
	static constexpr Int s_tableBufSize = CONFIG::s_tableBufSize;

	void clear() { _clear(); }

	AX_NODISCARD AX_INLINE constexpr Int size() const { return _orderedList.size(); }
	
	template<class... ARGS>
	AX_INLINE Node& addNode(const InKey & key, ARGS&&... args) { return _addNode(key, AX_FORWARD(args)...); }
	
	template<class... ARGS>
	AX_INLINE Value& add(const InKey & key, ARGS&&... args) { return addNode(key, AX_FORWARD(args)...).value(); }

	AX_INLINE		Value* 		find			(const InKey& key)						{ return _find(key, HashInt::s_make(key)); }
	AX_INLINE const Value* 		find			(const InKey& key) const				{ return ax_const_cast(this)->_find(key, HashInt::s_make(key)); }
	AX_INLINE		Value* 		find_hash		(const InKey& key, HashInt hash)		{ return _find(key, hash); }
	AX_INLINE const Value* 		find_hash		(const InKey& key, HashInt hash) const	{ return ax_const_cast(this)->_find(key, hash); }

	AX_INLINE		Node* 		findNode		(const InKey& key)						{ return _findNode(key, HashInt::s_make(key)); }
	AX_INLINE const Node* 		findNode		(const InKey& key) const				{ return ax_const_cast(this)->_findNode(key, HashInt::s_make(key)); }
	AX_INLINE		Node* 		findNode_hash	(const InKey& key, HashInt hash)		{ return _findNode(key, hash); }
	AX_INLINE const Node* 		findNode_hash	(const InKey& key, HashInt hash) const	{ return ax_const_cast(this)->_findNode(key, hash); }

	AX_INLINE  FindEnumator 	findAll			(const InKey& key)						{ return _findAll(key, HashInt::s_make(key)); }
	AX_INLINE CFindEnumator 	findAll			(const InKey& key) const				{ return ax_const_cast(this)->_findAll(key, HashInt::s_make(key)); }

	AX_INLINE  FindEnumator		findAll_hash	(const InKey& key, HashInt hash)		{ return _findAll(key, hash); }
	AX_INLINE CFindEnumator		findAll_hash	(const InKey& key, HashInt hash) const	{ return ax_const_cast(this)->_findAll(key, hash); }
	
	AX_INLINE bool				erase			(const InKey& key)						{ return _erase(key, HashInt::s_make(key)); }
	AX_INLINE bool				erase_hash		(const InKey& key, HashInt hash)		{ return _erase(key, hash); }

	template<class TT>
	using Iter_ = typename OrderedList::template Iter_<TT>;
	
	using  Iter = Iter_<      Node>;
	using CIter = Iter_<const Node>;

	 Iter begin()		{ return _orderedList.begin(); }
	CIter begin() const	{ return _orderedList.begin(); }

	 Iter end()			{ return _orderedList.end(); }
	CIter end() const	{ return _orderedList.end(); }

	template<class TT>
	class KeyIter {
	public:
		KeyIter(Iter_<TT> p) : _p(p) {}
		Value*	operator->()		{ return _p->key(); }
		Value&	operator*()			{ return _p->key(); }
		void	operator++()		{ ++_p; }
		bool operator!=(const KeyIter& r) { return _p != r._p; }
	private:
		Iter_<TT> _p;
	};

	template<class TT>
	class ForEachKey {
	public:
		ForEachKey(Iter_<TT> begin, Iter_<TT> end) : _begin(begin), _end(end) {}
		KeyIter<TT> begin() { return _begin; }
		KeyIter<TT> end()   { return _end;   }
	private:
		Iter_<TT> _begin = nullptr;
		Iter_<TT> _end   = nullptr;
	};

	ForEachKey<      Node>	keys()			{ return ForEachKey<      Node>(begin(), end()); }
	ForEachKey<const Node>	keys() const	{ return ForEachKey<const Node>(begin(), end()); }

	template<class TT>
	class ValueIter {
	public:
		ValueIter(Iter_<TT> p) : _p(p) {}
		Value*	operator->()		{ return _p->value(); }
		Value&	operator*()			{ return _p->value(); }
		void	operator++()		{ ++_p; }
		bool operator!=(const ValueIter& r) { return _p != r._p; }
	private:
		Iter_<TT> _p;
	};

	template<class TT>
	class ForEachValue {
	public:
		ForEachValue(Iter_<TT> begin, Iter_<TT> end) : _begin(begin), _end(end) {}
		ValueIter<TT> begin() { return _begin; }
		ValueIter<TT> end()   { return _end;   }
	private:
		Iter_<TT> _begin = nullptr;
		Iter_<TT> _end   = nullptr;
	};

	ForEachValue<      Node>	values()		{ return ForEachValue<      Node>(begin(), end()); }
	ForEachValue<const Node>	values() const	{ return ForEachValue<const Node>(begin(), end()); }

private:
	void _clear() {
		_hashTable.clear();
		_orderedList.clear();
	}

	HashList& _getListForAdd(const HashInt& hash) {
		auto n = _orderedList.size();
		auto newTableSize = _hashTable.size();
		if (newTableSize <= 0) {
			newTableSize = 4; // first resize table
		}else if( newTableSize < n / 8 + 1 ) {
			newTableSize = n / 4;
		}
		_resizeTable(newTableSize);
		return _getList(hash);
	}
	
	HashList&	_getList(const HashInt& hash) {
		auto n = static_cast<HashInt::Value>(_hashTable.size());
		AX_ASSERT(n > 0);
		Int index = static_cast<Int>(hash.value % n);
		return _hashTable[index];
	}
	
	void _resizeTable(Int newSize) {
		if (newSize == _hashTable.size()) return;
		_hashTable.clear();
		_hashTable.resize(newSize);

		if (newSize <= 0) return;
		//re-insert to table
		for(Node& node : _orderedList) {
			auto& hashList = _getList(node.hash());
			hashList.append(&node);
		}
	}

	template<class InKey>
	Node*	_findInHashList(HashList& list, HashInt hash, const InKey& key) {
		for(auto& node : list) {
			if (node.hash() == hash && node.key() == key) {
				return &node;
			}
		}
		return nullptr;
	}	

	template<class... ARGS>
	Node& _addNode(const InKey & key, ARGS&&... args) {
		auto hash = HashInt::s_make(key);
		HashList& hashList = _getListForAdd(hash);
		SPtr<Node> node = SPtr_new<Node>(AX_ALLOC_REQ, key, hash, AX_FORWARD(args)...);
		hashList.append(node);
		return *_orderedList.append(node);
	}

	FindEnumator _findAll(const InKey& key, HashInt hash) {
		if (_hashTable.size() <= 0) {
			return FindEnumator(key, nullptr);
		}
		auto& list = _getList(hash);
		return FindEnumator(key, _findInHashList(list, hash, key));
	}

	Node* _findNode(const InKey& key, HashInt hash) {
		auto enumator = _findAll(key, hash);
		if (auto it = enumator.begin()) {
			return it.node(); // return the first element
		} else {
			return nullptr;
		}
	}

	Value* _find(const InKey& key, HashInt hash) {
		if (auto* node = _findNode(key, hash)) {
			return &node->value();
		} else {
			return nullptr;
		}
	}

	bool _erase(const InKey& key, HashInt hash) {
		if (auto* node = _findNode(key, hash)) {
			_getList(hash).remove(node);
			_orderedList.remove(node);
			return true;
		} else {
			return false;
		}
	}
	
	
	OrderedList _orderedList;
	Array<HashList, s_tableBufSize> _hashTable;
};

} // namespace