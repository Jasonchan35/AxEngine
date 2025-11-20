
#include "AxCore.h"

import AxCore;
import AxCore.UnitTest;
import <cstdio>;
import <iostream>;

namespace ax {

	void test_string() {
		auto sv = StrView("testing");
		auto str = String("abcde");

		std::cout << "sizeof(String) = " << ax_sizeof<String> << std::endl;

		for (auto& c : sv) {
			std::cout << char(c) << std::endl;
		}
		std::cout << std::endl;

		try {
			auto c = sv[100];
			AX_UNUSED(c);
		} catch (Error& err) {
			std::cout << err.what() << std::endl;
		}
	}

	void test_array() {
		
		using Data = u16;
		Array<Data, 5>	arr;

		for (Int i = 0; i < 127; i++) {
			arr.append(Data(i));
		}
		printf("AxCore_Test_main");

		for (Int i = 0; i < 200; i++) {
			arr.append(Data(i + 1000));
		}

		for (auto& t : arr) {
			std::cout << t << " ";
		}
		std::cout << std::endl;
		
		printf("AxCore_Test_main");
	}
	
} // namespace

int main() {
//	ax::test_array();
	ax::test_string();
	return 0;
}
