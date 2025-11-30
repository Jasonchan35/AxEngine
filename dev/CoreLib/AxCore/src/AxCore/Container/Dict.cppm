module;
#include "AxCore-pch.h"

export module AxCore.Dict;
export import AxCore.LinkedList;
export import AxCore.Array;

export namespace ax {

template<class KEY, class VALUE>
class DictNode : protected LinkedListNode< DictNode<KEY, VALUE> > {
	using Base = LinkedListNode< DictNode<KEY, VALUE> >;
public:
	using Key = KEY;
	using Value = VALUE;

	DictNode(const KEY & key_, VALUE && value_) : key(std::move(key_)), value(std::move(value_)) {}
	
	KEY key;
	VALUE value;
};

template<class KEY, class VALUE>
class Dict : public NonCopyable {
	using Node = DictNode<KEY, VALUE>;
	using List = LinkedList<Node>;
public:

	template<class... ARGS>
	VALUE& add(const KEY & key, ARGS&&... args) {
		
		
		auto node = UPtr_new<Node>(ax_default_allocator(), key, AX_FORWARD(args));
	}

private:

	
	Array<List> _table;	
};

} // namespace