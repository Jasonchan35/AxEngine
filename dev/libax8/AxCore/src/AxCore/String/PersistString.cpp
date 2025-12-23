module;

module AxCore.PersistString;

namespace ax {

template<class T>
class PersistString_<T>::Manager : public NonCopyable {
	using This = Manager;
public:
	Manager() {
		ax_default_allocator(); // ensure allocator create before this class
	}

	using PersistStr = PersistString_<T>;
	using Data       = PersistString_Data<T>;
	using Key        = String_<T, 24>;

	struct MTData {
		Dict<Key, Data> dict;
	};
	SpinLockProtected<MTData> _mtData;

	AX_INLINE static This* s_instance() {
		static GlobalSingleton<This> s;
		return s.ptr();
	}

	AX_INLINE PersistStr lookup(StrView_<T> inStr) {
		if (!inStr) return PersistStr();

		auto mt = _mtData.scopedLock();

		if (auto* pData = mt->dict.find(inStr)) {
			return PersistStr(pData);
		}
		
		auto& node = mt->dict.addNode(inStr);
		auto& data = node.value();
		auto& key  = node.key();
		
		data.str  = StrLit_<T>::s_from_PersistString(key.data(), key.size());
		data.hash = node.hash();
		return &data;
	}
};

template<class T>
auto PersistString_<T>::s_make(StrView_<T> s) -> This {
	return Manager::s_instance()->lookup(s);
}


#define	E(T)	\
/*---- The explicit instantiation ---*/ \
	template class PersistString_Data<T>; \
	template class PersistString_<T>; \
//--------
	AX_TYPE_LIST_CHAR(E)
#undef	E

} // namespace
