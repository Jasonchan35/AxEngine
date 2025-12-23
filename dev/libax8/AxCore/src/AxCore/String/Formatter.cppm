module;


export module AxCore.Formatter;

export import AxCore.String;

export namespace ax {

template<class OBJ>
concept CON_onFormatParse_ = requires(const OBJ& obj) {
	{ OBJ::onFormatParse };
};

template<class OBJ, class FMT_CH>
concept CON_onFormat_ = requires(const OBJ& obj, Format_<FMT_CH> & fmt) {
	//	{ obj.onFormat(fmt) } -> std::same_as<void>;
	true;
};

template< class STR_CH, class IN_FMT_CH, class ... ARGS> 
inline void ax_format_to_internal(IString_<STR_CH> & output, const FormatString_<IN_FMT_CH, ARGS...> & fmt, const ARGS&... args) {
	// std::format only support char and wchar_t format string
	using FMT_CH = std::conditional_t<std::is_same_v<IN_FMT_CH, CharA>, CharA, CharW>;
	
	StrView_<FMT_CH> out_fmt;
	TempString_<FMT_CH> tmp_str;
	if constexpr (std::is_same_v<FMT_CH, IN_FMT_CH>) {
		out_fmt = fmt.get();
	} else {
		tmp_str.appendUtf(fmt.get());
		out_fmt = tmp_str;
	}

	auto func = [&](const StdFormatArgs_<FMT_CH> & format_args) {
		output.ensureCapacity(output.size() + fmt.size() + format_args._Estimate_required_capacity());
		std::vformat_to(IStringBackInserter_<STR_CH>(output), out_fmt.to_string_view(), format_args);
	};
	
	try {
		// std::make_format_args() must be call inside function call parameter to keep lifetime, otherwise the stored arg will be dangling 
		func(std::make_format_args<StdFormatContext_<FMT_CH>>(args...));
	} catch (std::exception& e) {
		auto msg = TempString::s_format("Format: Exception: {}, ArgCount={} format_string=[{}])", e.what(), sizeof...(args), out_fmt);
		ax_internal_log_error(msg.c_str());
		throw Error_Format();
	}
}

template<class CH, class... ARGS> constexpr 
void FmtTo(IString_<CH> & output, const FormatString_<CharA, ARGS...> & fmt, const ARGS&... args) {
	return ax_format_to_internal<CH, CharA, ARGS...>(output, fmt, AX_FORWARD(args)...);
}

template<class T, class... ARGS> constexpr
void FmtTo(IString_<T> & output, const FormatString_<CharW, ARGS...> & fmt, const ARGS&... args) {
	return ax_format_to_internal<T, CharW, ARGS...>(output, fmt, AX_FORWARD(args)...);
}

template<class T, class... ARGS> constexpr
void FmtTo(IString_<T> & output, const FormatString_<Char8, ARGS...> & fmt, const ARGS&... args) {
	return ax_format_to_internal<T, Char8, ARGS...>(output, fmt, AX_FORWARD(args)...);
}

template<class T, class... ARGS> constexpr
void FmtTo(IString_<T> & output, const FormatString_<Char16, ARGS...> & fmt, const ARGS&... args) {
	return ax_format_to_internal<T, Char16, ARGS...>(output, fmt, AX_FORWARD(args)...);
}

template<class T, class... ARGS> constexpr
void FmtTo(IString_<T> & output, const FormatString_<Char32, ARGS...> & fmt, const ARGS&... args) {
	return ax_format_to_internal<T, Char32, ARGS...>(output, fmt, AX_FORWARD(args)...);
}


template<class OUT_CH, class FMT_CH, class... ARGS> constexpr
TempString_<OUT_CH> Fmt_(FormatString_<FMT_CH, ARGS...> && fmt, const ARGS&... args) { TempString_<OUT_CH> str; FmtTo(str, AX_FORWARD(fmt), AX_FORWARD(args)...); return str; }

template <class... ARGS>
constexpr TempStringA Fmt(FormatString_<CharA, ARGS...>&& fmt, const ARGS&... args) {
	return Fmt_<CharA, CharA>(AX_FORWARD(fmt), AX_FORWARD(args)...);
}

template <class... ARGS>
constexpr TempStringW Fmt(FormatString_<CharW, ARGS...>&& fmt, const ARGS&... args) {
	return Fmt_<CharW, CharW>(AX_FORWARD(fmt), AX_FORWARD(args)...);
}

template <class... ARGS>
constexpr TempString8 Fmt(FormatString_<Char8, ARGS...>&& fmt, const ARGS&... args) {
	return Fmt_<Char8, Char8>(AX_FORWARD(fmt), AX_FORWARD(args)...);
}

template <class... ARGS>
constexpr TempString16 Fmt(FormatString_<Char16, ARGS...>&& fmt, const ARGS&... args) {
	return Fmt_<Char16, Char16>(AX_FORWARD(fmt), AX_FORWARD(args)...);
}

template <class... ARGS>
constexpr TempString32 Fmt(FormatString_<Char32, ARGS...>&& fmt, const ARGS&... args) {
	return Fmt_<Char32, Char32>(AX_FORWARD(fmt), AX_FORWARD(args)...);
}

} // namespace ax

#if 0
#pragma mark ----- global namespace - std::formatter ----------
#endif

// Wrapper to CustomClass::onFormat()
template<class T, class FMT_CH> requires ax::CON_onFormat_<T, FMT_CH>
struct std::formatter<T, FMT_CH> : public  ax::StdFormatter_<FMT_CH> {
	using Base = ax::StdFormatter_<FMT_CH>;

	constexpr auto parse(ax::StdFormatParseContext_<FMT_CH>& ctx) {
		if constexpr (ax::CON_onFormatParse_<T>) {
			T::onFormatParse(ctx);
			return ctx.end();
		} else {
			return Base::parse(ctx);
		}
	}

	auto format(const T& obj, ax::StdFormatContext_<FMT_CH>& ctx) const {
		ax::Format_<FMT_CH> format(*this, ctx);
		ax::FormatHandler<T, FMT_CH> handler;
		handler.onFormat(obj, format);
		return ctx.out();
	}
};

#if 0
#pragma mark ----------- FormatHandler ------------
#endif
export namespace ax {

template <class T, class FMT_CH> requires Type_IsFundamental<T>
class FormatHandler<T, FMT_CH> {
public:
	void onFormat(const T & obj, Format_<FMT_CH> & fmt) {
		std::formatter<T, FMT_CH> tmp;
		tmp.format(obj, fmt.formatContext);
	}
};

template <class T, class FMT_CH> requires Type_IsConvertibleToStrViewX<T>
class FormatHandler<T, FMT_CH> {
public:
	void onFormat(const T & obj, Format_<FMT_CH> & fmt) {
		
		if constexpr (std::is_same_v<T, FMT_CH>) {
			auto sv = StrView_<FMT_CH>(obj); 
			fmt.formatter.format(sv.to_string_view(), fmt.formatContext);
		} else {
			auto utf = TempString_<FMT_CH>::s_utf(obj);
			fmt.formatter.format(utf.to_string_view(), fmt.formatContext);
		}
	}
};

template <class T, class FMT_CH>
class FormatHandler<MutSpan<T>, FMT_CH> {
public:
	using Obj = MutSpan<T>;
	void onFormat(const Obj & obj, Format_<FMT_CH> & fmt) {
		fmt << "[";
		Int i = 0;
		for (auto& it : obj) {
			if (i > 0) fmt << ", ";
			fmt << it;
			++i;
		}
		fmt << "]";
	}
};

template <class T, class FMT_CH>
class FormatHandler<Span_FindResult<T>, FMT_CH> {
public:
	using Obj = Span_FindResult<T>;
	void onFormat(const Obj & obj, Format_<FMT_CH> & fmt) {
		fmt << "([" << obj.index << "], " << obj.value << ")";
	}
};

template <CON_IsIArray OBJ, class FMT_CH> 
class FormatHandler<OBJ, FMT_CH> {
public:
	void onFormat(const OBJ & obj, Format_<FMT_CH> & fmt) {
		fmt << obj.span();
	}
};

template <class A, class B, class FMT_CH>
class FormatHandler<Pair<A,B>, FMT_CH> {
public:
	using Obj = Pair<A,B>;
	void onFormat(const Obj & obj, Format_<FMT_CH> & fmt) {
		fmt << Fmt("({},{})", obj.first, obj.second);
	}
};

template <class T, class FMT_CH>
class FormatHandler<Range_<T>, FMT_CH> {
public:
	using Obj = Range_<T>;
	void onFormat(const Obj & obj, Format_<FMT_CH> & fmt) {
		fmt << Fmt("Range({}~{}:Size:{})", obj.start(), obj.stop(), obj.size());
	}
};

template <class FMT_CH>
class FormatHandler<std::exception, FMT_CH> {
public:
	using Obj = std::exception;
	void onFormat(const Obj & obj, Format_<FMT_CH> & fmt) {
		fmt << StrView_c_str(obj.what());
	}
};

template <class FMT_CH>
class FormatHandler<SrcLoc, FMT_CH> {
public:
	using Obj = SrcLoc;
	void onFormat(const Obj & obj, Format_<FMT_CH> & fmt) {
		fmt << Fmt("(SrcLoc:{}:{})", obj.file(), obj.line());
	}
};

template <class FMT_CH>
class FormatHandler<Error, FMT_CH> {
public:
	using Obj = Error;
	void onFormat(const Obj & obj, Format_<FMT_CH> & fmt) {
		fmt << StrView_c_str(obj.what());
	}
};

template <class V, class FMT_CH>
class FormatHandler<Opt<V>, FMT_CH> {
public:
	using Obj = Opt<V>;
	void onFormat(const Obj & obj, Format_<FMT_CH> & fmt) {
		if (!obj) {
			fmt << "nullopt";
		} else {
			fmt << *obj;
		}
	}
};

template<class T> requires Type_IsEnum<T>
constexpr TempString ax_enum_str(const T& v) {
	TempString outStr;
	if constexpr (Type_IsEnumFlag<T>) {
		using IntType = Type_EnumInt<T>;
		TempString str;
		IntType value = ax_enum_int(v);
		Int c = 0;
		Int bitCount = AX_SIZEOF(value) * 8;
		for (Int i = 0; i < bitCount; ++i) {
			auto mask = IntType(1) << i;
			if (value & mask) {
				if (c) str << " | ";
				str << ax_enum_entry_strlit(static_cast<T>(mask));
				c++;
			}
		}
		outStr << str;
	} else {
		outStr << ax_enum_entry_strlit(v);
	}
	return outStr;
}

template <class T, class FMT_CH> requires Type_IsEnum<T>
class FormatHandler<T, FMT_CH> {
public:
	void onFormat(const T & v, Format_<FMT_CH> & fmt) {
		fmt << ax_enum_str(v);
	}
};


} // namespace
