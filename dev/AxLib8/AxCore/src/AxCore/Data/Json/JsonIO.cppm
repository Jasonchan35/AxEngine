module;


export module AxCore.JsonIO;
export import AxCore.Logger;
export import AxCore.File;
export import AxCore.JsonIO_Writer;
export import AxCore.Uuid;

export namespace ax {

struct JsonIO  {
	JsonIO() = delete;
	
	template<class T> static void readFile			(StrView filename, T& obj);
	template<class T> static void readJson			(StrView  json, T& obj, StrView filename = StrView());

	template<class T> static void writeFile			(StrView filename, T& obj, const File_WriteOpt& opt = {});
	template<class T> static void writeJson			(IString& json, T& obj, StrView filename = StrView());

	template<class T> static void writeFileIfChanged(StrView filename, T& obj, const File_WriteOpt& opt = {});
};

template<class T> inline
void JsonIO::readFile(StrView filename, T& obj) {
	TempStringA json;
	File::readUtf8(filename, json);
	JsonIO_Reader rd(json, filename);
	rd.io(obj);
}

template<class T> inline
void JsonIO::writeFile(StrView filename, T& obj, const File_WriteOpt& opt) {
	TempString json;
	JsonIO_Writer wr(json, filename);
	wr.io(obj);
	File::writeFile(filename, json, opt);
}

template<class T> inline
void JsonIO::writeFileIfChanged(StrView filename, T& obj, const File_WriteOpt& opt) {
	TempString json;
	JsonIO_Writer wr(json, filename);
	wr.io(obj);
	File::writeFileIfChanged(filename, json, opt);
}

template<class T> inline
void JsonIO::writeJson(IString& outJson, T& obj, StrView filename) {
	JsonIO_Writer wr(outJson, filename);
	wr.io(obj);
}

template<class T> inline
void JsonIO::readJson(StrView json, T& obj, StrView filename) {
	JsonIO_Reader rd(json, filename);
	rd.io(obj);
}

//-----------------

template<>
struct JsonIO_Handler<Uuid> {
	template<class SE>
	static void onJsonIO(SE& se, Uuid& value) {
		if constexpr (se.isReader()) {
			TempString tmp;
			se.io(tmp);
			value.parse(tmp);
		} else {
			TempString tmp;
			value.getString(tmp);
			se.io(tmp);
		}
	}
};

template<class CH, class ID>
struct JsonIO_Handler<NameId_<CH, ID>> {
	template<class SE>
	static void onJsonIO(SE& se, NameId_<CH, ID>& value) {
		if constexpr (se.isReader()) {
			TempString tmp;
			se.io(tmp);
			value = NameId::s_make(tmp);
		} else {
			TempString tmp = value.toString();
			se.io(tmp);
		}
	}
};

template<class T> requires Type_IsEnum<T>
struct JsonIO_Handler<T> {
	template<class SE>
	static void onJsonIO(SE& se, T& value) {
		if constexpr (se.isReader()) {
			TempString tmp;
			se.io(tmp);
			T t;
			if (!tmp.tryParse(t)) {
				AX_ASSERT(false);
				return;
			}
			value = t;
		} else {
			se.writer.writeValue(ax_enum_str(value));
		}
	}
};

template<class T, class DEL>
struct JsonIO_Handler< UPtr<T, DEL> > {
	template<class SE>
	static void onJsonIO(SE& se, UPtr<T, DEL>& value) {
		if constexpr (se.isReader()) {
			auto p = UPtr_new<T, DEL>(AX_NEW);
			if (p) {
				se.io(*p);
			}
		} else {
			if (value == nullptr) {
				se.writer.writeNull();
			} else {
				se.io(*value);
			}
		}
	}
};

template<class T>
struct JsonIO_Handler< SPtr<T> > {
	template<class SE>
	static void onJsonIO(SE& se, SPtr<T>& value) {
		if constexpr (se.isReader()) {
			auto p = SPtr_new<T>(AX_NEW);
			if (p) {
				se.io(*p);
			}
		} else {
			if (value == nullptr) {
				se.writer.writeNull();
			} else {
				se.io(*value);
			}
		}
	}
};


} // namespace
