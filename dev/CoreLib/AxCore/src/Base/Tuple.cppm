module;
#include "AxBase.h"

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
	return Tuple_join_helper(	t0, IntSequence_make<TUPLE0::kSize>(),
								t1, IntSequence_make<TUPLE1::kSize>());
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
		static constexpr Int INDEX = N-1;
		static_assert(INDEX >= 0 && INDEX < TUPLE::kSize);

		Tuple_ForEach<TUPLE, FUNC, INDEX>::onEach(tuple, func);
		func(INDEX, tuple->template get<INDEX>());
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
			static constexpr Int INDEX = N-1;
			static_assert(INDEX >= 0 && INDEX < TUPLE::kSize);

			using Elem = typename TUPLE::template Element<INDEX>;

			if constexpr (INDEX > 0) {
				Tuple_ForEachType<TUPLE, HANDLER, INDEX, ARGS...>::onEach(AX_FORWARD(args)...);
			}

			HANDLER::template onEach<INDEX, Elem>(AX_FORWARD(args)...);
		}
	}
};

template<class... ELEMENTS>
class Tuple : public std::tuple<ELEMENTS ...> {
	using This = Tuple;
	using Base = std::tuple<ELEMENTS...>;
public:
	static constexpr Int kSize = std::tuple_size<Base>::value;

	AX_INLINE constexpr Tuple(ELEMENTS... args) : Base(AX_FORWARD(args)...) {}

	template<Int INDEX> using Element = typename std::tuple_element<INDEX, Base>::type;

	template<Int INDEX> AX_INLINE constexpr       Element<INDEX>& get()       { return std::get<INDEX>(*this); }
	template<Int INDEX> AX_INLINE constexpr const Element<INDEX>& get() const { return std::get<INDEX>(*this); }

	template<class TUPLE2>
	auto join(const TUPLE2& tuple2) const {
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
	constexpr static void s_forEachType(ARGS&&... args) {
		Tuple_ForEachType<This, HANDLER, kSize, ARGS...>::onEach(AX_FORWARD(args)...);
	}
};

template<class... ELEMENTS>
Tuple<ELEMENTS...> Tuple_make(ELEMENTS ... args) {
	return Tuple<ELEMENTS...>(AX_FORWARD(args)...);
}


} // namespace