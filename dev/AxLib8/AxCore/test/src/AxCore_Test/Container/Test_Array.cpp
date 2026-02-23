import AxCore_Test.PCH;
#include "AxUnitTest.h"

namespace ax {

class Test_Array : public UnitTestClass {
public:
	void test_case1() {
		// constexpr Array<u8> u8arr;
		
		Array<Int> arr;
		for (Int i = 0; i < 6; ++i) {
			arr.append(i * 10);
		}

		const auto& const_arr = arr;
		
		auto* p = const_arr.find(30);
		if (AX_TEST_IF(p != nullptr)) {
			auto index = const_arr.getIndexFromElementPtr(p);
			if (AX_TEST_IF(index)) {
				AX_TEST_EQ(index.value(), 3);
			}
			AX_TEST_EQ(*p, 30);
		}
	}
	
	void test_binarySearch() {
		Array<Int> arr;
		for (Int i = 0; i < 16; ++i) {
			if (i % 3 == 0)
				arr.append(i * 10);
			else 
				arr.append(8 - i);
		}
		
		AX_LOG("arr ={}", arr);
		arr.sort();
		AX_LOG("arr ={}", arr);
		auto* p = arr.binarySearch(7);
		if (AX_TEST_IF(p != nullptr)) {
			auto index = arr.getIndexFromElementPtr(p);
			AX_TEST_EQ(index, 10);
		}
	}

	void test_insertAt_beginning() {
		Array<Int> arr;
		arr.append(10);
		arr.append(20);
		arr.append(30);
		
		auto& elem1 = arr.insertAt(0);
		elem1 = 5;
		
		AX_TEST_EQ(arr.size(), 4);
		AX_TEST_EQ(arr[0], 5);
		AX_TEST_EQ(arr[1], 10);
		AX_TEST_EQ(arr[2], 20);
		AX_TEST_EQ(arr[3], 30);


		auto& elem2 = arr.insertAt(1);
		elem2 = 6;
		AX_TEST_EQ(arr.size(), 5);
		AX_TEST_EQ(arr[0], 5);
		AX_TEST_EQ(arr[1], 6);
		AX_TEST_EQ(arr[2], 10);
		AX_TEST_EQ(arr[3], 20);
		AX_TEST_EQ(arr[4], 30);

	}

	
	void test_insertAt_range() {
		Array<Int> arr;
		arr.append(100);
		arr.append(200);
		arr.append(300);
		arr.append(400);
		arr.append(500);

		arr.insertAt(IntRange_StartAndSize(3, 10));
		AX_TEST_EQ(arr.size(), 15);
		AX_TEST_EQ(arr[0], 100);
		AX_TEST_EQ(arr[1], 200);
		AX_TEST_EQ(arr[2], 300);
		AX_TEST_EQ(arr[13], 400);
		AX_TEST_EQ(arr[14], 500);

		
	}

};

} // namespace

void Test_Array() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Array::test_case1)
	AX_TEST_RUN_CASE(Test_Array::test_binarySearch)
	AX_TEST_RUN_CASE(Test_Array::test_insertAt_range)

	AX_TEST_RUN_CASE(Test_Array::test_insertAt_beginning)	
}

