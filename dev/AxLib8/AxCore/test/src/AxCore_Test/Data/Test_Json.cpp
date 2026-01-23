
import AxCore_Test.PCH;

import AxCore.JsonIO;

namespace ax {

class Test_Json : public UnitTestClass {
public:
	void test_case1() {
		String json;
		JsonWriter wr(json);
		{
			wr.writeObject([&]{
				wr.writeMemberName("aa");
				wr.writeValue("123");
				wr.writeMember("bb", 456);

				wr.writeArray("arr", [&] {
					for (int i = 0; i < 5; i++) {
						wr.writeValue(i);
					}
				});

				{
					JsonValue jv;
					auto& obj		= *jv.setToObject();
					obj["m_name"]	= "John";
					obj["m_bool0"]	= false;
					obj["m_bool1"]	= true;
					obj["m_int"]	= 10;
					obj["m_float"]	= 3.4f;
					obj["m_double"] = 1.23;
					obj["m_null"]	= nullptr;
					for (auto& d : *obj["m_array"].setToArray(2)) {
						d = L"Unicode";
					}

					wr.writeMember("jv", jv);
				}
			});
			
		}

		AX_DUMP(json);

		{ // read back
			JsonReader rd(json, "");
			String aa;
			Int bb = -1;
			Array<Int> arr;
			JsonValue jv;

			{
				rd.beginObject();
				while (!rd.endObject()) {
					if (rd.isMember("bb")) {
						rd.readValue(bb);
						continue;
					}
					if (rd.readMember("aa", aa)) continue;
					if (rd.readMember("arr", arr)) continue;
					if (rd.readMember("jv", jv)) continue;

					rd.skipUnhandledMember(true);
				}
			}

			AX_TEST_EQ(aa, "123");
			AX_TEST_EQ(bb, 456);

			Int arrValue[] = {0, 1, 2, 3, 4};
			AX_TEST_EQ(arr, Span(arrValue));

			if (!AX_TEST_IF(jv.isObject())) return;

			auto& obj = jv.asObject();
			AX_TEST_EQ(obj["m_name"]	,"John");
			AX_TEST_EQ(obj["m_bool0"]	, false);
			AX_TEST_EQ(obj["m_bool1"]	, true);
			AX_TEST_EQ(obj["m_int"]		, 10);
			AX_TEST_EQ(obj["m_null"]	, nullptr);

			AX_TEST_ALMOST_EQ(obj["m_float"].asDouble() , static_cast<f64>(3.4f));
			AX_TEST_ALMOST_EQ(obj["m_double"].asDouble(), 1.23);

			auto* jsonArr = obj["m_array"].tryGetArray();
			AX_TEST_IF(jsonArr != nullptr);
			if (jsonArr) {
				AX_TEST_EQ(jsonArr->size(), 2);

				AX_TEST_EQ(jsonArr->at(0), "Unicode");
				AX_TEST_EQ(jsonArr->at(1), "Unicode");
			}
		}

		{ // read back but lambda
			JsonReader rd(json, "");
			String aa;
			Int bb = -1;
			Array<Int> arr;
			JsonValue jv;

			rd.readObject([&]() {
				if (rd.isMember("bb")) {
					rd.readValue(bb);
					return;
				}
				if (rd.readMember("aa", aa)) return;
				if (rd.readMember("arr", arr)) return;
				if (rd.readMember("jv", jv)) return;

				rd.skipUnhandledMember(true);
			});


			AX_TEST_EQ(aa, "123");
			AX_TEST_EQ(bb, 456);

			Int arrValue[] = {0, 1, 2, 3, 4};
			AX_TEST_EQ(arr, Span(arrValue));
		}
	}

	void test_jsonValue() {
		AX_DUMP(sizeof(JsonValue));
		AX_DUMP(sizeof(JsonObject));
		AX_DUMP(sizeof(JsonArray));

		JsonValue json;
		{
			auto& obj		= *json.setToObject();
			obj["m_name"]	= "John";
			obj["m_bool0"]	= false;
			obj["m_bool1"]	= true;
			obj["m_int"]	= 10;
			obj["m_float"]	= 3.4f;
			obj["m_double"] = 1.23;
			obj["m_null"]	= nullptr;
			for (auto& d : *obj["m_array"].setToArray(2)) {
				d = L"Unicode";
			}
		}
		AX_DUMP(json);
	}

	struct TestData {
		int		i = 0;
		String	s;

		template<class SE>
		void onJsonIO(SE& se) {
			se.named_io("i", i);
			se.named_io("s", s);
		}
	};

#if 0
	void test_jsonIO() {
		String json;
		{
			JsonIO_Writer	wr(json);

			TestData data;
			data.i = 100;
			data.s = "Hello World";

			wr.io(data);
		}

		AX_DUMP(json);

		{
			JsonIO_Reader rd(json, "");

			TestData data2;

			rd.io(data2);

			AX_TEST_EQ(data2.i, 100);
			AX_TEST_EQ(data2.s, "Hello World");
		}
	}
#endif

};

} // namespace ax

void Test_Json() {
	using namespace ax;
	AX_TEST_RUN_CASE(Test_Json::test_case1);
	AX_TEST_RUN_CASE(Test_Json::test_jsonValue);
//	AX_TEST_RUN_CASE(Test_Json::test_jsonIO);
}