
#include <cstdio>

import AxCore;
import AxCore.UnitTest;

namespace ax {

	int my_main() {
		StrView sz = "testing"_sv;

		Array<Int, 16>	arr;

		for (Int i = 0; i < 127; i++) {
			arr.append(i);
		}
		printf("AxCore_Test_main");

		for (Int i = 0; i < 200; i++) {
			arr.append(i + 1000);
		}
		
		printf("AxCore_Test_main");
		return 0;
	}
	
} // namespace

int main() {
	return ax::my_main();
}
