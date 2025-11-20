
#include "AxCore.h"

import AxCore;
import AxCore.UnitTest;
import <cstdio>;
import <iostream>;

namespace ax {

	void func(const SrcLoc& loc = SrcLoc()) {
		std::cout << "loc";
	}

	int my_main() {
		StrView sv = "testing"_sv;

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

		func();
		
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
		return 0;
	}
	
} // namespace

int main() {
	return ax::my_main();
}
