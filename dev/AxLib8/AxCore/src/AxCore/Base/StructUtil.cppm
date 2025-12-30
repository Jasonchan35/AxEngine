module;

export module AxCore.StructUtil;
export import AxCore.BasicType;
export import AxCore.Tuple; 

export namespace ax {

template<class T_STRUCT>
struct StructUtil {
private:
	template<class... ARGS>
	using ToVoid = void;

	struct Dummy {
		template<typename Type>
		operator Type() const;
	};

	template<class E, class IntSeq = IntSequence<>, class = void>
	struct FieldCount_Helper : IntSeq {
		static_assert(std::is_standard_layout_v<E>);
		static_assert(!std::is_polymorphic_v<E>);
	};

	template<class E, Int... Indices>
	struct FieldCount_Helper<E, IntSequence<Indices...>, ToVoid<decltype(E{
		                         (static_cast<void>(Indices), std::declval<Dummy>())...,
		                         std::declval<Dummy>()})>>
		: FieldCount_Helper<E, IntSequence<Indices..., sizeof...(Indices)>> {
	};

public:
	static constexpr Int kFieldCount = FieldCount_Helper<T_STRUCT>().size();

	constexpr static auto toTuple(T_STRUCT&& object) noexcept {
		if constexpr (kFieldCount == 16) {
			          auto&& [p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15] = object;
			return Tuple_make(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);

		} else if constexpr (kFieldCount == 15) {
			          auto&& [p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14] = object;
			return Tuple_make(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
			
		} else if constexpr (kFieldCount == 14) {
			          auto&& [p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13] = object;
			return Tuple_make(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);

		} else if constexpr (kFieldCount == 13) {
			          auto&& [p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12] = object;
			return Tuple_make(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);

		} else if constexpr (kFieldCount == 12) {
			          auto&& [p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11] = object;
			return Tuple_make(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
			
		} else if constexpr (kFieldCount == 11) {
			          auto&& [p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10] = object;
			return Tuple_make(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);

		} else if constexpr (kFieldCount == 10) {
			          auto&& [p0, p1, p2, p3, p4, p5, p6, p7, p8, p9] = object;
			return Tuple_make(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9);

		} else if constexpr (kFieldCount == 9) {
			          auto&& [p0, p1, p2, p3, p4, p5, p6, p7, p8] = object;
			return Tuple_make(p0, p1, p2, p3, p4, p5, p6, p7, p8);
			
		} else if constexpr (kFieldCount == 8) {
			          auto&& [p0, p1, p2, p3, p4, p5, p6, p7] = object;
			return Tuple_make(p0, p1, p2, p3, p4, p5, p6, p7);

		} else if constexpr (kFieldCount == 7) {
			          auto&& [p0, p1, p2, p3, p4, p5, p6] = object;
			return Tuple_make(p0, p1, p2, p3, p4, p5, p6);

		} else if constexpr (kFieldCount == 6) {
			          auto&& [p0, p1, p2, p3, p4, p5] = object;
			return Tuple_make(p0, p1, p2, p3, p4, p5);

		} else if constexpr (kFieldCount == 5) {
			          auto&& [p0, p1, p2, p3, p4] = object;
			return Tuple_make(p0, p1, p2, p3, p4);
			
		} else if constexpr (kFieldCount == 4) {
			          auto&& [p0, p1, p2, p3] = object;
			return Tuple_make(p0, p1, p2, p3);
			
		} else if constexpr (kFieldCount == 3) {
			          auto&& [p0, p1, p2] = object;
			return Tuple_make(p0, p1, p2);

		} else if constexpr (kFieldCount == 2) {
			          auto&& [p0, p1] = object;
			return Tuple_make(p0, p1);

		} else if constexpr (kFieldCount == 1) {
			          auto&& [p0] = object;
			return Tuple_make(p0);

		} else if constexpr (kFieldCount == 0) {
			return Tuple_make();
			
		} else {
			static_assert(false);
		}
	}

	using TupleType = decltype(toTuple(T_STRUCT()));

}; // StructUtil

} // namespace ax
