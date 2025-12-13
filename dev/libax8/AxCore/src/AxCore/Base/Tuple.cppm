module;


export module AxCore.Tuple;

import AxCore.BasicType;

export namespace ax {

template<class... ARGS> class Tuple;

template<
	class TUPLE0, Int... INDEX0,
	class TUPLE1, Int... INDEX1
> AX_INLINE constexpr
auto Tuple_join_helper(	const TUPLE0& t0, const IntSequence<INDEX0...>&, 
						const TUPLE1& t1, const IntSequence<INDEX1...>&) {
	return Tuple_make(
		t0.template getValue<INDEX0>() ...,
		t1.template getValue<INDEX1>() ...
	);
}

template<class TUPLE0, class TUPLE1>
AX_INLINE constexpr
auto Tuple_join(const TUPLE0& t0, const TUPLE1& t1) {
	return Tuple_join_helper(	t0, IntSequence_make<TUPLE0::Size>(),
								t1, IntSequence_make<TUPLE1::Size>());
}

template<class...T> struct Tuple_JoinType_Helper;

template<
	class TUPLE0, Int... INDEX0,
	class TUPLE1, Int... INDEX1
>
struct Tuple_JoinType_Helper<	TUPLE0, IntSequence<INDEX0...>, 
								TUPLE1, IntSequence<INDEX1...> >
{
	using Type = Tuple<	typename TUPLE0::template Element<INDEX0> ...,
						typename TUPLE1::template Element<INDEX1> ...
						>;
};

template<class TUPLE0, class TUPLE1>
struct Tuple_JoinType {
	using Type = typename Tuple_JoinType_Helper<	TUPLE0, IntSequence_make<TUPLE0::kSize>,
													TUPLE1, IntSequence_make<TUPLE1::kSize>
													>::Type;
};

template<class TUPLE, class FUNC, Int N>
struct Tuple_ForEach {
	static void onEach(TUPLE* tuple, FUNC func) {
		static constexpr Int index = N-1;
		static_assert(index >= 0 && index < TUPLE::kSize);

		Tuple_ForEach<TUPLE, FUNC, index>::onEach(tuple, func);
		func(index, tuple->template get<index>());
	}
};
template<class TUPLE, class FUNC> 
struct Tuple_ForEach<TUPLE, FUNC, 0> { static void onEach(TUPLE* tuple, FUNC h) {} };

template<class TUPLE, class HANDLER, Int N, class... ARGS>
struct Tuple_ForEachType {
	static void onEach(ARGS&&... args) {
		if constexpr (TUPLE::kSize <= 0) {
			return;
		} else {
			static constexpr Int index = N-1;
			static_assert(index >= 0 && index < TUPLE::kSize);

			using Elem = typename TUPLE::template Element<index>;

			if constexpr (index > 0) {
				Tuple_ForEachType<TUPLE, HANDLER, index, ARGS...>::onEach(AX_FORWARD(args)...);
			}

			HANDLER::template onEach<index, Elem>(AX_FORWARD(args)...);
		}
	}
};

template<class... ELEMENTS>
class Tuple : public std::tuple<ELEMENTS ...> {
	using This = Tuple;
	using Base = std::tuple<ELEMENTS...>;
public:
	static constexpr Int kSize = std::tuple_size_v<Base>;

	constexpr Tuple() = default;
	constexpr Tuple(const Tuple&) = default;
	constexpr Tuple(Tuple&&) = default;

	// when ARGS == 0, conflict with zero argument Tuple()
	template<class... ARGS> requires (sizeof...(ARGS) > 0 && sizeof...(ARGS) == sizeof...(ELEMENTS))
	constexpr Tuple(ELEMENTS&&... args) : Base(AX_FORWARD(args)...) {}

	template<Int INDEX> using Element = typename std::tuple_element<INDEX, Base>::type;

	template<Int INDEX> AX_INLINE constexpr       Element<INDEX>& get()       { return std::get<INDEX>(*this); }
	template<Int INDEX> AX_INLINE constexpr const Element<INDEX>& get() const { return std::get<INDEX>(*this); }

	template<class ELEMENT> AX_INLINE constexpr       ELEMENT& get()       { return std::get<ELEMENT>(*this); }
	template<class ELEMENT> AX_INLINE constexpr const ELEMENT& get() const { return std::get<ELEMENT>(*this); }

	// FUNC example: [](const auto&...args){  }
	template<class FUNC> constexpr auto apply(FUNC func) {
		Base* base = this;
		std::apply(func, *base);
	}
	
	template<class TUPLE2>
	constexpr auto join(const TUPLE2& tuple2) const {
		return Tuple_join(*this, tuple2);
	}

	template<class TUPLE2>
	using JoinType = typename Tuple_JoinType<This, TUPLE2>::Type;

	template<class FUNC>
	constexpr void forEach(FUNC func) {
		Tuple_ForEach<This, FUNC, kSize>::onEach(this, func);
	}

	template<class FUNC>
	constexpr void forEach(FUNC func) const {
		Tuple_ForEach<const This, FUNC, kSize>::onEach(this, func);
	}

	template<class HANDLER, class... ARGS>
	constexpr static void ForEachType(ARGS&&... args) {
		Tuple_ForEachType<This, HANDLER, kSize, ARGS...>::onEach(AX_FORWARD(args)...);
	}
};

template<class... ELEMENTS>
Tuple<ELEMENTS...> Tuple_make(ELEMENTS ... args) {
	return Tuple<ELEMENTS...>(AX_FORWARD(args)...);
}


} // namespace