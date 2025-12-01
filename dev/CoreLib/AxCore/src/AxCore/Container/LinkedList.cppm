module;
#include "AxCore-pch.h"

export module AxCore.LinkedList;
export import AxCore.Array;
export import AxCore.WPtr;
export import AxCore.Formatter;

export namespace ax {

AX_SIMPLE_ERROR(Error_LinkedList)

template<class TAG>
struct LinkedList_Config_ {
};

using LinkedList_DefaultConfig = LinkedList_Config_<void>;

//! Intrusive Linked List
template<class T, class CONFIG = LinkedList_DefaultConfig> class LinkedList;
template<class T, bool REV> class LinkedList_Iter;

template<class T, class CONFIG = LinkedList_DefaultConfig>
class LinkedListNode : public NonCopyable {
	using Node = LinkedListNode;
	friend class LinkedList<T, CONFIG>;
	friend class LinkedList_Iter<      Node, true>;
	friend class LinkedList_Iter<const Node, true>;
	friend class LinkedList_Iter<      Node, false>;
	friend class LinkedList_Iter<const Node, false>;
protected:
	using _ListNodeConfig = CONFIG;
	
	Node*	_listNode_next = nullptr;
	Node*	_listNode_prev = nullptr;

	using _ListNodeOwner = T;
			T* _listNodeOwner()			{ return static_cast<      T*>(this); }
	const	T* _listNodeOwner() const	{ return static_cast<const T*>(this); }
public:
	~LinkedListNode() {
		AX_ASSERT(_listNode_prev == nullptr);
		AX_ASSERT(_listNode_next == nullptr);
	}
};

template<class NODE, bool REV>
class LinkedList_Iter {
	using This   = LinkedList_Iter;
	using Node   = NODE;
	using Config = typename NODE::_ListNodeConfig;
public:
	using Owner		= Type_KeepConst<NODE, typename NODE::_ListNodeOwner>;
	
	LinkedList_Iter(NODE* p) : _node(p) {}

	NODE* node() { return _node; }
	Owner* owner() { return _node->_listNodeOwner(); }
	
	operator Owner*		()  { return  owner(); }
	Owner&	operator*	()	{ return *owner(); }
	Owner*	operator->	()	{ return  owner(); }
	bool	operator==	( const This & r ) const { return _node == r._node; }
	bool	operator!=	( const This & r ) const { return _node != r._node; }
	void	operator++	()	{
		if(!_node) return;
		if constexpr (REV) {
			_node = _node->_listNode_prev;
		} else {
			_node = _node->_listNode_next;
		}
	}
private:
	NODE*	_node = nullptr;
};

//! Intrusive Linked Array
template<class T, class CONFIG>
class LinkedList : public NonCopyable {
public:
	using Node      = LinkedListNode<T, CONFIG>;

	static constexpr bool useSPtr = std::is_base_of_v<SPtrReferenableBase, T>; 
	using OUT_Ptr = std::conditional_t<useSPtr, SPtr<T>,   UPtr<T>>; 
	using IN_Ptr  = std::conditional_t<useSPtr, SPtr<T> &, UPtr<T> &&>; 
	
	LinkedList() = default;
	LinkedList(LinkedList && r) {
		swap(r);
	}

	~LinkedList() { clear(); }

	AX_INLINE Int size() const { return _size; }

	AX_INLINE T* append			(IN_Ptr newNode) noexcept				{ return _append(AX_FORWARD(newNode)); }
	AX_INLINE T* insert			(IN_Ptr newNode) noexcept				{ return _insertBefore(AX_FORWARD(newNode), _head);  }
	AX_INLINE T* insertBefore	(IN_Ptr newNode, T* before) noexcept	{ return _insertBefore(AX_FORWARD(newNode), before); }
	AX_INLINE T* insertAfter	(IN_Ptr newNode, T* after ) noexcept {
		return _insertBefore(AX_FORWARD(newNode), after ? _getNode(after)->_listNode_next : _head);
	}
	
	AX_INLINE OUT_Ptr remove(T* node) noexcept { return _remove(node); }

	AX_INLINE void swap(LinkedList & r) {
		std::swap(_head, r._head);
		std::swap(_tail, r._tail);
		std::swap(_size, r._size);
	}

	AX_INLINE void appendMove(LinkedList && r) { _appendMove(AX_FORWARD(r)); }


	AX_INLINE 		T*	head()			{ return _head; }
	AX_INLINE const	T*	head() const	{ return _head; }

	AX_INLINE 		T*	tail()			{ return _tail; }
	AX_INLINE const	T*	tail() const	{ return _tail; }

	AX_INLINE OUT_Ptr	popHead() { return _head ? _remove(_head) : nullptr; }
	AX_INLINE OUT_Ptr	popTail() { return _tail ? _remove(_tail) : nullptr; }

	AX_INLINE void clear() {
		while(_head) {
			popHead();
		}
		AX_ASSERT(!_head && !_tail && _size == 0);
	}

	using Iter  = LinkedList_Iter<      Node, false>;
	using CIter = LinkedList_Iter<const Node, false>;

	AX_INLINE Iter begin()		{ return Iter(_head); }
	AX_INLINE Iter end()		{ return Iter(nullptr); }

	AX_INLINE CIter begin() const	{ return CIter(_head); }
	AX_INLINE CIter end()	const	{ return CIter(nullptr); }

	template<class CH>
	void onFormat(Format_<CH> & fmt) const {
		fmt << "[";
		bool first = true;
		for (auto & it : *this) {
			if (!first) {
				fmt << ",";
				first = false;
			}
			fmt << it;
		}
		fmt << "]";
	}

private:
	AX_INLINE Node* _getNode(T* obj) { return static_cast<Node*>(obj); }

	void _appendMove(LinkedList && r) {
		if (r.size() <= 0) return;
		if (size() <= 0) {
			AX_ASSERT(!_head && !_tail);
			swap(r);
			return;
		}

		r._head->_listNode_prev = _tail;
		  _tail->_listNode_next = r._head;

		_tail  = r._tail;
		_size += r._size;

		r._head = nullptr;
		r._tail = nullptr;
		r._size = 0;
	}

	T* _append(IN_Ptr newObj) {
		if (!newObj) throw Error_LinkedList();
		auto* newNode = _getNode(newObj);
		if (newNode->_listNode_next || newNode->_listNode_prev) {
			throw Error_LinkedList();
		}

		newNode->_listNode_prev = _tail;
		if (_tail) {
			_tail->_listNode_next = newNode;
		} else {
			_head = newNode;
		}
		_tail = newNode;
		_size++;

		if constexpr (useSPtr) {
			newObj->_addSPtrRefCount();
			return newObj.ptr();
		} else {
			return newObj.detach();
		}
	}
	
	T* _insertBefore(UPtr<T> &&  newNode, Node* before) {
		if (!before) {
			return append(AX_FORWARD(newNode));
		}
		
		if (!newNode) throw Error_LinkedList();
		
		if (newNode->_listNode_next || newNode->_listNode_prev) {
			throw Error_LinkedList();
		}

		newNode->_listNode_next = before;
		newNode->_listNode_prev = before->_listNode_prev;

		if (auto* prev = before->_listNode_prev) {
			prev->_listNode_next = newNode;
		} else {
			_head = newNode;
		}

		before->_listNode_prev = newNode;
		_size++;
		return newNode.detach();
	}

	OUT_Ptr _remove(Node* node) {
		if (!node) return nullptr;
		if (!_size) {
			AX_ASSERT(false);
			return nullptr;
		}

		auto& next = node->_listNode_next;
		auto& prev = node->_listNode_prev;

		if (next) {
			next->_listNode_prev = prev;
		} else {
			_tail = prev;
		}

		if (prev) {
			prev->_listNode_next = next;
		} else {
			_head = next;
		}

		next = nullptr;
		prev = nullptr;

		_size--;

		T* nodeOwner = node->_listNodeOwner();
		if constexpr (useSPtr) {
			SPtr<T> sp(nodeOwner);
			nodeOwner->_releaseSPtrRefCount();
			return sp;
		} else {
			return UPtr<T>::s_ref(nodeOwner);
		}
	}

	Node*	_head = nullptr;
	Node*	_tail = nullptr;
	Int		_size = 0;
};

} // namespace 