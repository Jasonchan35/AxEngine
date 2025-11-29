module;
#include "AxPlatform-pch.h"

export module AxPlatform.LinkedList;
export import AxPlatform.Array;
export import AxPlatform.UPtr;

export namespace ax {

AX_SIMPLE_ERROR(Error_LinkedList)

//! Intrusive Linked List
template<class T> class LinkedList;
template<class T, bool REV> class LinkedList_Iter;

template<class T>
class LinkedListNode : public NonCopyable {
	friend class LinkedList<T>;
	friend class LinkedList_Iter<      T, true>;
	friend class LinkedList_Iter<const T, true>;
	friend class LinkedList_Iter<      T, false>;
	friend class LinkedList_Iter<const T, false>;
protected:
	//! separate node data to another class, so T cannot access via protected
	T*	_listNode_next = nullptr;
	T*	_listNode_prev = nullptr;
};

template<class T, bool REV>
class LinkedList_Iter {
	using This = LinkedList_Iter;
public:
	LinkedList_Iter(T* p) : _p(p) {}

	operator T*			()  { return  _p; }
	T&		operator*	()	{ return *_p; }
	T*		operator->	()	{ return  _p; }
	bool	operator==	( const This & r ) const { return _p == r._p; }
	bool	operator!=	( const This & r ) const { return _p != r._p; }
	void	operator++	()	{
		if(!_p) return;
		if constexpr (REV) {
			_p = _p->_listNode_prev;
		} else {
			_p = _p->_listNode_next;
		}
	}
private:
	T*	_p = nullptr;
};

//! Intrusive Linked Array
template<class T>
class LinkedList : public NonCopyable {
public:
	using Node = LinkedListNode<T>;
	LinkedList() = default;
	LinkedList(LinkedList && r) {
		swap(r);
	}

	~LinkedList() { clear(); }

	AX_INLINE Int size() const { return _size; }

	AX_INLINE T* append			(UPtr<T> && newNode) noexcept				{ return _append(AX_FORWARD(newNode)); }
	AX_INLINE T* insert			(UPtr<T> && newNode) noexcept				{ return _insertBefore(AX_FORWARD(newNode), _head);  }
	AX_INLINE T* insertBefore	(UPtr<T> && newNode, T* before) noexcept	{ return _insertBefore(AX_FORWARD(newNode), before); }
	AX_INLINE T* insertAfter	(UPtr<T> && newNode, T* after ) noexcept	{ return _insertBefore(AX_FORWARD(newNode), after ? after->_listNode_next : _head); }

	AX_INLINE UPtr<T> remove(T* node) noexcept { return _remove(node); }

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

	AX_INLINE UPtr<T>	popHead() { return _head ? _remove(_head) : nullptr; }
	AX_INLINE UPtr<T>	popTail() { return _tail ? _remove(_tail) : nullptr; }

	AX_INLINE void clear() {
		while(_head) {
			popHead();
		}
		AX_ASSERT(!_head && !_tail && _size == 0);
	}

	using Iter  = LinkedList_Iter<      T, false>;
	using CIter = LinkedList_Iter<const T, false>;

	AX_INLINE Iter begin()		{ return Iter(_head); }
	AX_INLINE Iter end()		{ return Iter(nullptr); }

	AX_INLINE CIter begin() const	{ return CIter(_head); }
	AX_INLINE CIter end()	const	{ return CIter(nullptr); }

private:
	void _appendMove(LinkedList && r) {
		if (r.size() <= 0) return;
		if (size() <= 0) {
			AX_ASSERT(!_head && !_tail);
			swap(r);
			return;
		}

		r._head->_listNode_prev = _tail;
		  _tail->_listNode_next = r._head;

		_tail = r._tail;
		_size += r._size;

		r._head = nullptr;
		r._tail = nullptr;
		r._size = 0;
	}

	T* _append(UPtr<T> &&  newNode) {
		if (!newNode) throw Error_LinkedList();
		if (newNode->_listNode_next || newNode->_listNode_prev) {
			throw Error_LinkedList();
		}

		newNode->_listNode_prev = _tail;
		if (_tail) {
			_tail->_listNode_next = newNode.ptr();
		} else {
			_head = newNode.ptr();
		}
		_tail = newNode.ptr();
		_size++;
		return newNode.detach();
	}

	T* _insertBefore(UPtr<T> &&  newNode, T* before) {
		if (!before) {
			return append(AX_FORWARD(newNode));
		}

		if (!newNode) throw Error_LinkedList();
		if (newNode->_listNode_next || newNode->_listNode_prev) {
			throw Error_LinkedList();
		}

		newNode->_listNode_next = before;
		newNode->_listNode_prev = before->_listNode_prev;

		if (before->_listNode_prev) {
			before->_listNode_prev->_listNode_next = newNode.ptr();
		} else {
			_head = newNode.ptr();
		}

		before->_listNode_prev = newNode.ptr();
		_size++;
		return newNode.detach();
	}

	UPtr<T> _remove(T* node) {
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
		return UPtr<T>::s_ref(node);
	}

	T*		_head = nullptr;
	T*		_tail = nullptr;
	Int		_size = 0;
};

} // namespace 