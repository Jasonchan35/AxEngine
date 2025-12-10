module;


export module AxCore.LinkedList;
export import AxCore.Array;
export import AxCore.WPtr;
export import AxCore.Formatter;

export namespace ax {

AX_SIMPLE_ERROR(Error_LinkedList)

//! Intrusive Linked List
template<class T, class CONFIG = void> class LinkedList;
template<class T, class CONFIG = void> class LinkedListNode;
template<bool REV, class T, class CONFIG> class LinkedList_Iter;

template<class T, class CONFIG>
class LinkedListNode : public NonCopyable {
	using Node = LinkedListNode;
	friend class LinkedList<T, CONFIG>;
	friend class LinkedList_Iter<true,        T, CONFIG>;
	friend class LinkedList_Iter<true,  const T, CONFIG>;
	friend class LinkedList_Iter<false,       T, CONFIG>;
	friend class LinkedList_Iter<false, const T, CONFIG>;
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

template<bool REV, class T, class CONFIG>
class LinkedList_Iter {
	using This    = LinkedList_Iter;
	using MutNode = LinkedListNode<std::remove_const_t<T>, CONFIG>;
	using Node    = Type_KeepConst<T, MutNode>;
	using Config  = CONFIG;
public:
	using Owner	 = T;
	
	LinkedList_Iter(Node* p) : _node(p) {}

	Node* node() { return _node; }
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
	Node*	_node = nullptr;
};

template<bool REV, class T, class CONFIG>
class LinkedList_ForEach : public NonCopyable {
	using List = LinkedList<T, CONFIG>;
	List* _list = nullptr;
public:
	LinkedList_ForEach(List* list) : _list(list) {}
	
	template<class TT>
	using Iter_ = LinkedList_Iter<REV, TT, CONFIG>;
	
	using Iter  = Iter_<T>;
	using CIter = Iter_<const T>;

	AX_INLINE Iter begin()		{ return Iter(_list->tail()); }
	AX_INLINE Iter end()		{ return Iter(nullptr); }
};

//! Intrusive Linked Array
template<class T, class CONFIG>
class LinkedList : public NonCopyable {
public:
	using Node      = LinkedListNode<T, CONFIG>;

	LinkedList() = default;
	LinkedList(LinkedList && r) {
		swap(r);
	}

	~LinkedList() { clear(); }

	AX_INLINE Int size() const { return _size; }

	AX_INLINE T* append			(UPtr<T> && newNode) noexcept				{ return _append(AX_FORWARD(newNode)); }
	AX_INLINE T* insert			(UPtr<T> && newNode) noexcept				{ return _insertBefore(AX_FORWARD(newNode), _head);  }
	AX_INLINE T* insertBefore	(UPtr<T> && newNode, T* before) noexcept	{ return _insertBefore(AX_FORWARD(newNode), before); }
	AX_INLINE T* insertAfter	(UPtr<T> && newNode, T* after ) noexcept {
		return _insertBefore(AX_FORWARD(newNode), after ? _getNode(after)->_listNode_next : _head);
	}
	
	AX_INLINE UPtr<T> remove(T* node) noexcept { return _remove(node); }

	AX_INLINE void swap(LinkedList & r) {
		std::swap(_head, r._head);
		std::swap(_tail, r._tail);
		std::swap(_size, r._size);
	}

	AX_INLINE void appendList(LinkedList && r) { _appendMove(AX_FORWARD(r)); }


	AX_INLINE 		T*	head()			{ return _head->_listNodeOwner(); }
	AX_INLINE const	T*	head() const	{ return _head->_listNodeOwner(); }

	AX_INLINE 		T*	tail()			{ return _tail->_listNodeOwner(); }
	AX_INLINE const	T*	tail() const	{ return _tail->_listNodeOwner(); }

	AX_INLINE UPtr<T>	popHead() { return _head ? _remove(_head) : nullptr; }
	AX_INLINE UPtr<T>	popTail() { return _tail ? _remove(_tail) : nullptr; }

	AX_INLINE void clear() {
		while(popHead()) {}
		AX_ASSERT(!_head && !_tail && _size == 0);
	}

	template<class TT>
	using Iter_ = LinkedList_Iter<false, TT, CONFIG>;
	
	using Iter  = Iter_<T>;
	using CIter = Iter_<const T>;

	AX_INLINE Iter begin()		{ return Iter(_head); }
	AX_INLINE Iter end()		{ return Iter(nullptr); }

	AX_INLINE CIter begin() const	{ return CIter(_head); }
	AX_INLINE CIter end()	const	{ return CIter(nullptr); }

	constexpr auto revForEach() { return LinkedList_ForEach<true, T, CONFIG>(this); }

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

	T* _append(UPtr<T> && newObj) {
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

		return newObj.detach();
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

	UPtr<T> _remove(Node* node) {
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
		return UPtr<T>::s_ref(nodeOwner);
	}

	Node*	_head = nullptr;
	Node*	_tail = nullptr;
	Int		_size = 0;
};


template<class T, class CONFIG = void>
class SPtrLinkedList : public NonCopyable {
	using _List = LinkedList<T, CONFIG>;
	_List _list;
	using This = SPtrLinkedList;
public:
	SPtrLinkedList() = default;
	SPtrLinkedList(SPtrLinkedList && r) = default;
	
	AX_INLINE constexpr	~SPtrLinkedList() { clear(); }
	
	AX_INLINE constexpr void clear() { while (popHead()) {} }

	AX_NODISCARD AX_INLINE constexpr T* head() { return _list.head(); }

	AX_INLINE constexpr T* append		(T* newNode) noexcept				{ return _list.append(s_inputPtr(newNode)); }
	AX_INLINE constexpr T* insert		(T* newNode) noexcept				{ return _list.insert(s_inputPtr(newNode)); }
	AX_INLINE constexpr T* insertBefore	(T* newNode, T* before) noexcept	{ return _list.insertBefore(s_inputPtr(newNode), before); }
	AX_INLINE constexpr T* insertAfter	(T* newNode, T* after ) noexcept	{ return _list.insertAfter (s_inputPtr(newNode), after ); }
	
	constexpr void appendList(This && r) { _list.appendList(std::move(r._list)); }
	
	AX_INLINE constexpr SPtr<T> popHead() 		{ return s_outputPtr(_list.popHead());		}
	AX_INLINE constexpr SPtr<T> popTail() 		{ return s_outputPtr(_list.popTail());		}
	AX_INLINE constexpr SPtr<T> remove(T* node)	{ return s_outputPtr(_list.remove(node));	}
	
	AX_NODISCARD AX_INLINE constexpr Int size() const { return _list.size(); }

	template<class TT>
	using Iter_ = typename _List::template Iter_<TT>;
	using Iter  = Iter_<T>;
	using CIter = Iter_<const T>;
	AX_NODISCARD AX_INLINE constexpr Iter  begin()			{ return _list.begin(); }
	AX_NODISCARD AX_INLINE constexpr Iter  end()			{ return _list.end();   }
	AX_NODISCARD AX_INLINE constexpr CIter begin() const	{ return _list.begin(); }
	AX_NODISCARD AX_INLINE constexpr CIter end()   const	{ return _list.end();   }

private:
	AX_NODISCARD AX_INLINE constexpr UPtr<T> s_inputPtr(T* p) {
		if (!p) return nullptr;
		p->_addSPtrRefCount();
		return UPtr_ref(p);
	}
	
	AX_NODISCARD AX_INLINE constexpr SPtr<T> s_outputPtr(UPtr<T> && p) {
		if constexpr (true) {
			return SPtr<T>::s_ref_DontAddRefCount(p.detach());
		} else {
			auto sp = SPtr_fromUPtr(p);
			if (sp) sp->_releaseSPtrRefCount();
			return sp;
		}
	}
};

} // namespace 