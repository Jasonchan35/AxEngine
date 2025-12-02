module;
#include "AxCore-pch.h"

export module AxCore.Dict;
export import AxCore.LinkedList;
export import AxCore.Array;
export import AxCore.SPtr;

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
		          : _key(key),         _hash(hash),    value(args...) {}

	const	Key&	key() const		{ return _key; }
	const HashInt&	hash() const { return _hash; }

	friend class DictNode;
protected:
	Key		_key;
	HashInt _hash;
	
public:
	Value	value;
};

template<class NODE>
class Dict_FindIter {
public:
	using Value = typename NODE::Value;
	
	Dict_FindIter(NODE* p) : _node(p) {}
	AX_NODISCARD AX_INLINE constexpr explicit operator bool() const noexcept { return _node; } 
	
	AX_NODISCARD AX_INLINE constexpr NODE*	node		() noexcept	{ return _node; }
	AX_NODISCARD AX_INLINE constexpr Value&	value		() noexcept	{ return _node->value; }

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
class Dict_FindEnumator {
public:
	using FindIter = Dict_FindIter<NODE>;
	
	Dict_FindEnumator() = default;
	Dict_FindEnumator(const IN_KEY& key, FindIter begin) : _begin(begin), _key(&key) {}

	explicit operator bool() const { return _begin; }

	FindIter	begin	() { return _begin;  }
	FindIter	end		() { return nullptr; }
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
	using Node = DictNode<KEY, VALUE>;
	using OrderedList = SPtrLinkedList<Node, Dict_OrderedListConfig>;
	using HashList    = SPtrLinkedList<Node, Dict_HashListConfig>;
public:
	using Key          = KEY;
	using Value        = VALUE;
	using InKey        = typename CONFIG::InKey;
	using FindEnumator = Dict_FindEnumator<Node, InKey>;
	using FindIter     = Dict_FindIter<Node>;
	static constexpr Int s_tableBufSize = CONFIG::s_tableBufSize;

	void clear() {
		_hashTable.clear();
		_orderedList.clear();
	}

	template<class... ARGS>
	Node& addNode(const InKey & key, ARGS&&... args) {
		auto hash = HashInt::s_make(key);
		HashList& hashList = _getListForAdd(hash);
		SPtr<Node> node = SPtr_new<Node>(AX_ALLOC_REQ, key, hash, AX_FORWARD(args)...);
		hashList.append(node);
		return *_orderedList.append(node);
	}

	template<class... ARGS>
	Value& add(const InKey & key, ARGS&&... args) { return addNode(key, AX_FORWARD(args)...).value; }

	FindEnumator findAll(const InKey& key, const HashInt& hash) {
		if (_hashTable.size() <= 0) {
			return FindEnumator(key, nullptr);
		}
		auto& list = _getList(hash);
		return FindEnumator(key, _findInHashList(list, hash, key));
	}

	FindEnumator findAll(const InKey& key) {
		auto hash = HashInt::s_make(key);
		return findAll(key, hash);
	}
	
	Value* find(const InKey& key) { return find(key, HashInt::s_make(key)); }
	Value* find(const InKey& key, HashInt hash) {
		auto enumator = findAll(key, hash);
		if (auto it = enumator.begin()) {
			return &it.value(); // return the first element
		} else {
			return nullptr;
		}
	}

private:
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

	OrderedList _orderedList;
	Array<HashList, s_tableBufSize> _hashTable;
};

} // namespace