export module AxCore.StrView;

import AxCore.Base;

namespace ax {
	
	template<CON_CharType T>
	class MutStrView_ {
		//Copyable
		using This = MutStrView_;
	protected:
		T*	_data = nullptr;
		Int	_size = 0;
	public:
		using CharType = T;
	};
	
} // namespace