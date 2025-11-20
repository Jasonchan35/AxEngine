
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

		try {
			auto c = sv[100];
			AX_UNUSED(c);
		} catch (Error& err) {
			std::cout << err.what() << std::endl;
		}

		func();
		
		using Data = u8;
		Array<Data, 5>	arr;

		for (Int i = 0; i < 127; i++) {
			arr.append(Data(i));
		}
		printf("AxCore_Test_main");

		for (Int i = 0; i < 200; i++) {
			arr.append(Data(i + 1000));
		}
		
		printf("AxCore_Test_main");
		return 0;
	}
	
} // namespace

int main() {
	return ax::my_main();
}
