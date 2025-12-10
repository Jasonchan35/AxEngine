module;
// 

export module AxCore.BinIO;
export import AxCore.BinIO_Writer;

export namespace ax {

struct BinIO {
	BinIO() = delete;
	
	template<class T> static void readFile			(StrView filename, T& obj);
	template<class T> static void writeFile			(StrView filename, T& obj, bool createDir = true, bool logResult = true);
	template<class T> static void writeFileIfChanged(StrView filename, T& obj, bool createDir = true, bool logResult = true);
};

template<class T>
struct BinIO_Handler< IArray<T> > {
	template<class SE>
	void onIO(SE& se, IArray<T>& v) {
		if constexpr (se.isReader()) {
			Int len = 0;
			se.io(len);
			v.resize(len);
			try {
				for (Int i = 0; i < len; i++) {
					se.io(v[i]);
				}
			} catch(...) {
				v.clear();
				throw;
			}
		} else {
			Int len = v.size();
			se.io(len);
			for (Int i = 0; i < len; i++) {
				se.io(v[i]);
			}
		}
	}
};

} // namespace