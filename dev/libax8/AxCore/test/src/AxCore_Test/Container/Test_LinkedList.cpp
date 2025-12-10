
import AxCore_Test.PCH;

import AxCore.LinkedList;

namespace ax {

struct Test_LinkedList : public UnitTestClass {
	class TestData : public LinkedListNode<TestData> {
	public:
		TestData(Int value_) : value(value_) {
		}

		template<class CH>
		void onFormat(Format_<CH>& ctx) const {
			ctx << value;
		}

		Int	value = 0;
	};

	void test_case1() {
		LinkedList<TestData>	list;
		Array<TestData*>	arr;

		{
			Int N = 5;
		
			for (Int i = 0; i < N; i++) {
				auto uptr = UPtr_new<TestData>(AX_ALLOC_REQ, i);
				auto* p = list.append(std::move(uptr));
				arr << p;
			}

			Int i = 0;
			for (auto& e : list) {
				AX_TEST_EQ(e.value, i);
				i++;
			}
		}

		{			
			list.insertAfter(	UPtr_new<TestData>(AX_ALLOC_REQ, 100), arr[3]);
			list.insert(		UPtr_new<TestData>(AX_ALLOC_REQ, 101));
			list.insertBefore(	UPtr_new<TestData>(AX_ALLOC_REQ, 102), nullptr);
			list.insertAfter(	UPtr_new<TestData>(AX_ALLOC_REQ, 103), nullptr);
			list.insertAfter(	UPtr_new<TestData>(AX_ALLOC_REQ, 104), arr[4]);
			list.remove(arr[2]);
		}

		AX_LOG("list = {}", list);
		{
			static constexpr Int src[] = {103, 101, 0, 1, 3, 100, 4, 104, 102};
			auto span = Span(src);
			Int n = span.size();
			
			Int i = 0;
			for (auto& e : list) {
				AX_TEST_EQ(e.value, src[i]);
				i++;
			}
			AX_TEST_EQ(i, n);

			for (auto& e : list.revForEach()) {
				i--;
				AX_TEST_EQ(e.value, src[i]);
			}
			AX_TEST_EQ(i, 0);

			for (auto& e : span.revForEach()) {
				AX_TEST_EQ(e, src[n-i-1]);
				i++;
			}
		}
	}
};

} // namespace

void Test_LinkedList() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_LinkedList::test_case1)
}

